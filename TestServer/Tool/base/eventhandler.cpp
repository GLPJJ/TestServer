#include "eventhandler.h"
#include "reactor.h"

namespace Tool
{
	int TMEventHandler::registerTimer(time_t to)
	{
		return getReactor()->registerTimer(this,to);
	}
	int TMEventHandler::unRegisterTimer()
	{
		return getReactor()->unRegisterTimer(this);
	}
	void TMEventHandler::close()
	{
		unRegisterTimer();
	}
	int IdleEventHandler::registerIdle()
	{
		return getReactor()->registerIdle(this);
	}
	int IdleEventHandler::unRegisterIdle()
	{
		return getReactor()->unRegisterIdle(this);
	}
	void IdleEventHandler::close()
	{
		unRegisterIdle();
	}
	int FDEventHandler::registerRead()
	{
		return getReactor()->registerReadEvent(this);
	}
	int FDEventHandler::registerWrite()
	{
		return getReactor()->registerWriteEvent(this);
	}
	int FDEventHandler::unRegisterRead()
	{
		return getReactor()->unRegisterReadEvent(this);
	}
	int FDEventHandler::unRegisterWrite()
	{
		return getReactor()->unRegisterWriteEvent(this);
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
		getReactor()->unRegisterEvent(this);
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