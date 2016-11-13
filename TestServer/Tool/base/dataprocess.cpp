#include "dataprocess.h"
#include "datablock.h"
#include "package.h"
#include "clientsocket.h"

#ifdef WIN32
	#include <winsock2.h>
#else
	#include "sys/socket.h"
#endif

#include <string.h>
#include <stdlib.h>


namespace Tool
{
	size_t DataProcess::getBuflen(char *buf)
	{
		if(m_pttype == PROTOCOLTYPE_BINARY)
		{
			if(m_hdlen == HEADER_LEN_2)
			{
				short len;
				memcpy(&len,buf,sizeof(len));
				len = ntohs(len);
				return (size_t)len;
			}
			else if(m_hdlen == HEADER_LEN_4)
			{
				int len;
				memcpy(&len,buf,sizeof(len));
				len = ntohl(len);
				return (size_t)len;
			}
		}
		else if(m_pttype == PROTOCOLTYPE_TEXT)
		{
			char lenbuf[10] = {0};
			memcpy(lenbuf,buf,m_hdlen);
			//字符串转长整数，以16进制返回结果
			long len = strtol(lenbuf, NULL, 16);
			return (size_t)len;
		}
		return 0;
	}
	int DataProcess::process(ClientSocketBase *pClient)
	{
		DataBlock *recvdb = pClient->getRB();
		char *buf = recvdb->getBuf();
		char *ptr = buf;
		size_t pos = recvdb->getPos();

		while(true)
		{
			// 长度不超过包头长度的时候
			if( (size_t)(buf + pos - ptr) <= (size_t)m_hdlen)
				break;
			//取包头长度
			size_t buflen = getBuflen(ptr);
			if(buflen == 0)//异常数据
			{
				recvdb->initPos();
				return -1;
			}
			//不足一个包长度的时候
			if( (ptr + buflen) > (buf + pos) )
				break;

			//获取到一个完整包，解析包
			
			if(onPackage(pClient,&Package(ptr,buflen)) != 0)
			{
				//解析失败
				recvdb->initPos();
				return -1;
			}
			//把它close了,而不是返回-1
			if(pClient->isClosed())
			{
				recvdb->initPos();
				return 0;
			}
			//解析下一个包
			ptr += buflen;
		}

		//剩余的不足一个包长度的数据，把它拷贝下来。
		size_t  remain = (size_t)(buf + pos - ptr);
		if( remain == 0)
			recvdb->initPos();
		else
			recvdb->copy(0,ptr,remain);

		return 0;
	}
}

