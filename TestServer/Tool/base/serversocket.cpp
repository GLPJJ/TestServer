#include "../Tool.h"
#include <stdlib.h>

namespace Tool{

	int ListenSocketBase::Listen()
	{
		m_fd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);//socket ÃèÊö·û
		if( INVALID_SOCKET==m_fd )
		{
			Log("server create socket error %d",errno);
			return -1;
		}

		if(setAddrReuse() != 0)
			return -1;

		struct sockaddr_in ipAddr ={0};
		ipAddr.sin_family=AF_INET;
		ipAddr.sin_port=htons(m_port);
		ipAddr.sin_addr.s_addr=htonl(INADDR_ANY);

		if( 0 != bind(m_fd,(struct sockaddr*)&ipAddr,sizeof(ipAddr)) )
		{
			Log("server socket bind error %d",errno);
			close();
			return -1;
		}

		return listen(m_fd,SOMAXCONN);
	}

	void ListenSocketBase::OnFDRead()
	{
		
	}
}

