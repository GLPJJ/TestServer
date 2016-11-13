#include "object.h"
#include "datablock.h"
#include <string.h>

namespace Tool
{
	DataBlock::DataBlock(size_t size)
		: m_pos(0),m_size(size)
	{
		m_buf = (char*)calloc_(size);
	}
	DataBlock::~DataBlock() { 
		free_(m_buf);
	}

	size_t DataBlock::copy(size_t pos,const char *buf,size_t buflen)
	{
		if(!buf || !buflen)
			return -1;

		size_t tmppos = pos +  buflen;
		//未超出容量
		if(  tmppos <= m_size )
		{
			memcpy(m_buf+pos,buf,buflen);
			m_pos = tmppos;
		}
		else 
		{
			size_t newSize = m_size;
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
