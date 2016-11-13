#include "serversocket.h"
#include "log.h"
#include "object.h"
#include "clientsocket.h"
#include <stdlib.h>

namespace Tool{

	int ListenSocketBase::listen()
	{
		m_fd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);//socket 描述符
		if( INVALID_SOCKET==m_fd )
		{
			Log("create socket error %d\n",errno);
			return -1;
		}
		Log("服务器创建网络描述符 fd=%d\n",m_fd);

		if(setAddrReuse() != 0){
			Log("setAddrReuse error %d\n",errno);
			close();
			return -1;
		}
		Log("服务器设置地址重用成功\n");

		struct sockaddr_in ipAddr ={0};
		ipAddr.sin_family=AF_INET;
		ipAddr.sin_port=htons(m_port);
		ipAddr.sin_addr.s_addr=htonl(INADDR_ANY);

		if( 0 != bind(m_fd,(struct sockaddr*)&ipAddr,sizeof(ipAddr)) )
		{
			Log("socket bind error %d\n",errno);
			close();
			return -1;
		}
		Log("服务器绑定地址成功\n");

		if(::listen(m_fd,SOMAXCONN) != 0){
			Log("socket listen error %d\n",errno);
			return -1;
		}
		Log("服务器监听成功\n");

		registerRead();
		return 0;
	}

	void ListenSocketBase::onFDRead()
	{
		sockaddr addr;
		int len = sizeof(addr);
		SOCKET clientSocket = accept(m_fd,&addr,&len);
		if(clientSocket == INVALID_SOCKET){
			onAcceptError(errno);
		} else {
			onAccept(clientSocket,&addr);
		}
	}


	ListenSocketBase2::~ListenSocketBase2()
	{
		std::map<int,ClientDataBase*>* dict = m_pClientMap.getMap();
		std::map<int,ClientDataBase*>::iterator it = dict->begin();

		while(it != dict->end()){
			delete_(ClientDataBase,it->second);
			it = dict->erase(it);
		}
	}

	void ListenSocketBase2::onAccept(int fd,sockaddr* addr)
	{
		if(m_pClientMap.get(fd) == NULL){//可能吗？
			
			sockaddr_in* pAddr = (sockaddr_in*)addr;
			//in_addr
			Log("客户端新连接:接入 fd = %d,ip = %s\n",fd,inet_ntoa(pAddr->sin_addr));

			new_(ClientDataBase,pClient);
			if(pClient){
				pClient->fd = fd;
				memcpy(&pClient->addr,addr,sizeof(sockaddr));
				pClient->uid = 0;
				m_pClientMap.put(fd,pClient);
			}
		}
	}
	void ListenSocketBase2::onAcceptError(int code)
	{
		Log("%s error = %d",__FUNCTION__,code);
	}
}

