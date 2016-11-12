#include "eventhandler.h"
#include "reactor.h"

namespace Tool
{
	int TMEventHandler::registerTimer(time_t to)
	{
		return GetReactor()->registerTimer(this,to);
	}
	int TMEventHandler::unRegisterTimer()
	{
		return GetReactor()->unRegisterTimer(this);
	}
	void TMEventHandler::close()
	{
		GetReactor()->unRegisterTimer(this);
	}
	int IdleEventHandler::registerIdle()
	{
		return GetReactor()->registerIdle(this);
	}
	int IdleEventHandler::unRegisterIdle()
	{
		return GetReactor()->unRegisterIdle(this);
	}
	void IdleEventHandler::close()
	{
		GetReactor()->unRegisterIdle(this);
	}
	int FDEventHandler::registerRead()
	{
		return GetReactor()->registerReadEvent(this);
	}
	int FDEventHandler::registerWrite()
	{
		return GetReactor()->registerWriteEvent(this);
	}
	int FDEventHandler::unRegisterRead()
	{
		return GetReactor()->unRegisterReadEvent(this);
	}
	int FDEventHandler::unRegisterWrite()
	{
		return GetReactor()->unRegisterWriteEvent(this);
	}
	//设置非阻塞 socket
	int FDEventHandler::setNonBlocking()
	{
#ifdef WIN32
		u_long l = 1;//非0：非阻塞；0：阻塞
		return ioctlsocket(m_fd,FIONBIO,&l);
#else 
		int flags = fcntl(m_fd, F_GETFL, 0);    
		if(fcntl(m_fd, F_SETFL, flags|O_NONBLOCK) == -1)
			return -1;
#endif
		return 0;
	}

	int FDEventHandler::setAddrReuse()
	{
		int on=1;
		return setsockopt(m_fd,SOL_SOCKET,SO_REUSEADDR,(char*)&on,sizeof(on));
	}
	void FDEventHandler::close()
	{
		GetReactor()->unRegisterEvent(this);
		m_fd = INVALID_SOCKET;
	}
	void FDEventHandler::closeSocket()
	{
#ifdef WIN32
		closesocket(m_fd);
#else
		close(m_fd);
#endif
		m_fd = INVALID_SOCKET;
	}
}