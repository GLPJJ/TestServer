#include "../Tool.h"
#include <string.h>

namespace Tool
{
	DataBlock::DataBlock(unsigned int size)
		: m_pos(0),m_size(size)
	{
		m_buf = (char*)calloc_(size);
	}
	DataBlock::~DataBlock() { 
		free_(m_buf);
	}

	int DataBlock::copy(unsigned int pos,const char *buf,unsigned int buflen)
	{
		if(!buf || !buflen)
			return -1;

		unsigned int tmppos = pos +  buflen;
		//未超出容量
		if(  tmppos <= m_size )
		{
			memcpy(m_buf+pos,buf,buflen);
			m_pos = tmppos;
		}
		else 
		{
			unsigned int newSize = m_size;
			while(newSize < tmppos)
				newSize = newSize << 2;
			
			char *tmpbuf = (char*)calloc_(newSize);
			if(!tmpbuf)
				return 0;

			memcpy(tmpbuf,m_buf,m_pos);
			free_(m_buf);
			memcpy(tmpbuf + pos,buf,buflen);

			m_buf = tmpbuf;
			m_pos = tmppos;
			m_size = newSize;
		}

		return buflen;
	}
}
