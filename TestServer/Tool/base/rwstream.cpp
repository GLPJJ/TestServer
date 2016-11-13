#include "rwstream.h"
#include "log.h"
#include <math.h>

#ifdef WIN32
#include <WinSock2.h>
#else
#include "unistd.h"
#include "sys/socket.h"
#include "arpa/inet.h"
#include "netinet/in.h"
#endif

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif//max
#ifndef min
#define min(a,b) (((a)>(b))?(b):(a))
#endif//min

namespace Tool
{
	////////////////////////////////read operation//////////////////////////////////////////
	bool ReadStream::read(/* out */ int & i)
	{
		static const int VALUE_SIZE = sizeof(int);
		LOGI("%s short = %d\n",__FUNCTION__,VALUE_SIZE);

		if(!readCom(&i,sizeof(i),NULL))
			return false;
		if(net)
			i = ntohl(i);
		return skip(VALUE_SIZE,false);
	}
	bool ReadStream::read(/* out */ short & i)
	{
		static const int VALUE_SIZE = sizeof(short);
		LOGI("%s short = %d\n",__FUNCTION__,VALUE_SIZE);

		if(!readCom(&i,sizeof(i),NULL))
			return false;
		if(net)
			i = ntohs(i);
		return skip(VALUE_SIZE,false);
	}
	bool ReadStream::read(/* out */ char & c)
	{
		static const int VALUE_SIZE = sizeof(char);
		LOGI("%s short = %d\n",__FUNCTION__,VALUE_SIZE);

		if(!readCom(&c,sizeof(c),NULL))
			return false;

		return skip(VALUE_SIZE,false);
	}
	bool ReadStream::readLength(unsigned int & outlen)
	{
		if ( !readLengthWithoutOffset(outlen) )
			return false;

		// ƫ�Ƶ����ݵ�λ��
		return skip((int)htype,false);
	}
	bool ReadStream::readLengthWithoutOffset(unsigned int & outlen)
	{
		switch(htype)
		{
		case BINARY_PACKLEN_LEN:
			{
				unsigned short tmp;
				unsigned int len_readed;
				if(!readCom(&tmp,sizeof(tmp),&len_readed))
					return false;
				if(net)
					outlen = ntohs(tmp);
				else
					outlen = tmp;
				break;
			}
		case TEXT_PACKLEN_LEN:
			{
				unsigned int tmp;
				unsigned int len_readed;
				if(!readCom(&tmp,sizeof(tmp),&len_readed))
					return false;
				if(net)
					outlen = ntohl(tmp);
				else
					outlen = tmp;
				break;
			}
		}
		return true;
	}

	unsigned int ReadStream::readAll(char * szBuffer, unsigned int iLen)
	{
		if(!szBuffer || !iLen)
			return 0;

		unsigned int minLen = min(iLen, getSize());
		memcpy(szBuffer, getData(), minLen);
		return minLen;
	}
	bool ReadStream::read(char* str, unsigned int strlen, /* out */ unsigned int& outlen)
	{
		if(!str || !strlen)
			return false;

		unsigned int fieldlen;
		//��ȡ�ַ������ȣ�����Э�����ݳ������͡�
		if ( !readLength(fieldlen) ) {
			return false;
		}

		// user buffer is not enough
		outlen = fieldlen;
		if ( strlen < outlen ) {
			outlen = strlen;
		}

		if(!readCom(str,outlen,NULL))
			return false;
		return skip(fieldlen,false);
	}

	BinaryReadStream::BinaryReadStream(const char* ptr_, unsigned int len_,PACKAGELEN_TYPE htype_)
		:ReadStream(htype_),start(ptr_),len(len_),cur(ptr_),end(ptr_+len_)
	{
		skip((int)htype,false);
	}

	bool BinaryReadStream::readCom(/*out*/void* buffer,/*in*/unsigned int len_to_read,/*out*/unsigned int* len_readed)
	{
		//����ȡ���ݣ�����bufferΪ��
		if(!buffer||!len_to_read)
		{
			if(len_readed)
				*len_readed = 0;
			return true;
		}
		//û����ô�����ݶ�ȡ
		if(cur+len_to_read > end)
		{
			unsigned int len = end-cur;
			if(len)
				memcpy(buffer,cur,len);
			if(len_readed)
				*len_readed = len;
			return false;
		}

		memcpy(buffer,cur,len_to_read);
		if(len_readed)
			*len_readed = len_to_read;
		return true;
	}

	bool BinaryReadStream::skip(int offset,bool peek)
	{
		if(cur+offset > end)
			return false;
		if(!peek)
			cur += offset;
		return true;
	}
	////////////////////////////////write operation//////////////////////////////////////////
	BinaryWriteStream::BinaryWriteStream(char* ptr_, unsigned int len_,PACKAGELEN_TYPE htype_)
		:WriteStream(htype_), start(ptr_), len(len_), cur(ptr_),end(ptr_+len_)
	{
		skip((int)htype,false);
	}
	bool WriteStream::write(const char* str, unsigned int length)
	{
		if(!skip(length+(int)htype))
			return false;

		if ( !writeLength(length) ) {
			return false;
		}

		if(!writeCom(str,length,NULL))
			return false;
		return skip(length,false);
	} 
	bool WriteStream::write(int i)
	{
		if(net)
			i = htonl(i);
		static const int offset = sizeof(i);
		LOGI("%s int sizeof = %d\n",__FUNCTION__,offset);
		if(!writeCom(&i,offset,NULL))
			return false;
		return skip(offset,false);
	}
	bool WriteStream::write(short i)
	{
		if(net)
			i = htons(i);
		static const int offset = sizeof(i);
		LOGI("%s short sizeof = %d\n",__FUNCTION__,offset);
		if(!writeCom(&i,offset,NULL))
			return false;
		return skip(offset,false);
	}
	bool WriteStream::write(char c)
	{
		static const int offset = sizeof(c);
		LOGI("%s char sizeof = %d\n",__FUNCTION__,offset);
		if(!writeCom(&c,offset,NULL))
			return false;
		return skip(offset,false);
	}
	bool WriteStream::writeWithoutLength(const char* str, unsigned int length)
	{
		if(!writeCom(str,length,NULL))
			return false;
		return skip(length,false);
	} 
	bool WriteStream::writeLength(unsigned int length)
	{
		switch(htype)
		{
		case BINARY_PACKLEN_LEN:
			{
				unsigned short nShort = (unsigned short)length;
				static const int offset = sizeof(unsigned short);
				LOGI("unsigned short sizeof = %d\n",offset);
				if(net)
					nShort = htons(nShort);
				if(!writeCom(&nShort,offset,NULL))
					return false;
				skip(offset,false);
				break;
			}
		case TEXT_PACKLEN_LEN:
			{
				unsigned long nLong = (unsigned long)length;
				static const int offset = sizeof(unsigned long);
				LOGI("unsigned long sizeof = %d\n",offset);
				if(net)
					nLong = htonl(nLong);
				if(!writeCom(&nLong,offset,NULL))
					return false;
				skip(offset,false);
				break;
			}
		}
		return true;
	}
	bool BinaryWriteStream::skip(int offset,bool peek)
	{
		if(cur+offset > end)
			return false;
		if(!peek)
			cur += offset;
		return true;
	}
	bool BinaryWriteStream::writeCom(const void* buffer,unsigned int len_to_write,unsigned int* len_writed)
	{
		//�쳣���
		if(!buffer || !len_to_write)
		{
			if(len_writed)
				*len_writed = 0;
			return false;
		}
		//����д��
		if(cur+len_to_write > end)
		{
			unsigned int len = (unsigned int)(end - cur);
			if (len)
				memcpy(cur,buffer,len);
			if (len_writed)
				*len_writed = len;
			return false;
		}

		memcpy(cur,buffer,len_to_write);
		if(len_writed)
			*len_writed = len_to_write;
		return true;
	}

	bool BinaryWriteStream::isValid() const
	{
		unsigned int datalen = getSize();
		return datalen > BINARY_PACKLEN_LEN && datalen <= PACKAGE_MAXLEN;
	}
	void BinaryWriteStream::flush()
	{
		unsigned short datalen = (unsigned short)getSize();
		datalen = htons(datalen);
		unsigned short* ps = reinterpret_cast<unsigned short*>(start);
		*ps = datalen;
	}
}
