#include "reactor.h"
#include "mutex.h"
#include "log.h"

namespace Tool
{
	//////////////////////////EventHandlerSet 的实现////////////////////////////////////////////////
	void EventHandlerSet::addTMEventHandler(TMEventHandler *pHandler,time_t to)
	{
		if(!pHandler)
			return ;
		std::map<int,TMEHINFO>::iterator it = m_TMEHMap.find(pHandler->getTimerID());
		if(it != m_TMEHMap.end())
		{
			it->second.life = to;
			it->second.regtime = time(NULL);
		}
		else
		{
			TMEHINFO info;
			info.pHandler = pHandler;
			info.life = to;
			info.regtime = time(NULL);

			m_TMEHMap.insert(std::make_pair(pHandler->getTimerID(),info));
		}
	}
	void EventHandlerSet::dealClose()
	{
		if(m_setCloseSocket.empty())
			return ;

		SETSOCKET::iterator it=m_setCloseSocket.begin();
		for (; it!=m_setCloseSocket.end(); it++)
		{
			SOCKET socket = *it;
			if(socket == SOCKET_ERROR)
				continue;
#ifdef _WIN32
			::closesocket(socket);
#else
			close(socket);
#endif
		}
		m_setCloseSocket.clear();
	}
	void EventHandlerSet::idle()
	{
		std::set<IdleEventHandler*>::iterator it;
		for(it = m_IdleEHList.begin();it != m_IdleEHList.end();it++)
			(*it)->onRun();
	}
	void EventHandlerSet::scan()
	{
		static time_t lasttime = 0;//上次的处理时间
		if(lasttime != 0 && (time(NULL) - lasttime) < 1)
			return;

		std::map<int,TMEHINFO> tmmap = m_TMEHMap;
		std::map<int,TMEHINFO>::iterator it2,it3;
		for(it2 = tmmap.begin();it2 != tmmap.end();it2++)
		{
			if(  (time(NULL) - it2->second.regtime) > it2->second.life )  
			{
				it3 = m_TMEHMap.find(it2->first);
				if(it3 != m_TMEHMap.end())
					it3->second.regtime = time(NULL);
				//处理超时
				it2->second.pHandler->onTimeOut();
			}
		}
		lasttime = time(NULL);
	}

	///////////////////////NetReactor 的实现///////////////////////////////////////////////////
	NetRector::NetRector():m_bRunning(true)
	{
		FD_ZERO(&m_readset);
		FD_ZERO(&m_writeset);
		m_cs = Mutex::CreateCriticalSection();
	}
	NetRector::~NetRector()
	{
		Mutex::Destroy(m_cs);
	}
	int NetRector::registerIdle(IdleEventHandler *pHandler)
	{
		MutexScoped lock(m_cs);
		m_Set.addIdleEventHandler(pHandler);
		return 0;
	}
	int NetRector::unRegisterIdle(IdleEventHandler *pHandler)
	{
		MutexScoped lock(m_cs);
		m_Set.delIdleEventHandler(pHandler);
		return 0;
	}
	int NetRector::registerTimer(TMEventHandler *pHandler,time_t to)
	{
		MutexScoped lock(m_cs);
		m_Set.addTMEventHandler(pHandler,to);
		return 0;
	}
	int NetRector::registerReadEvent(FDEventHandler *pHandler)
	{
		MutexScoped lock(m_cs);
		//还没加入读fd_set
		if(FD_ISSET(pHandler->getFD(),&m_readset) == 0)
			FD_SET(pHandler->getFD(),&m_readset);//加入写fd_set
		m_Set.addFDEventHandler(pHandler);//加入map
		return 0;
	}
	int NetRector::registerWriteEvent(FDEventHandler *pHandler)
	{
		MutexScoped lock(m_cs);
		//如果不在写fd_set中
		if(FD_ISSET(pHandler->getFD(),&m_writeset) == 0)
			FD_SET(pHandler->getFD(),&m_writeset);//加入写fd_set
		m_Set.addFDEventHandler(pHandler);//加入map
		return 0;
	}

	int NetRector::unRegisterTimer(TMEventHandler *pHandler)
	{
		MutexScoped lock(m_cs);
		m_Set.delTMEventHandler(pHandler);
		return 0;
	}
	int NetRector::unRegisterReadEvent(FDEventHandler *pHandler)
	{
		MutexScoped lock(m_cs);
		if(FD_ISSET(pHandler->getFD(),&m_readset))
			FD_CLR(pHandler->getFD(),&m_readset);
		if(FD_ISSET(pHandler->getFD(),&m_writeset) == 0)
			m_Set.delFDEventHandler(pHandler->getFD());
		return 0;
	}
	int NetRector::unRegisterWriteEvent(FDEventHandler *pHandler)
	{
		MutexScoped lock(m_cs);
		if(FD_ISSET(pHandler->getFD(),&m_writeset))
			FD_CLR(pHandler->getFD(),&m_writeset);
		if(FD_ISSET(pHandler->getFD(),&m_readset) == 0)
			m_Set.delFDEventHandler(pHandler->getFD());
		return 0;
	}
	int NetRector::unRegisterEvent(FDEventHandler *pHandler)
	{
		MutexScoped lock(m_cs);
		if (FD_ISSET(pHandler->getFD(),&m_writeset))
			FD_CLR(pHandler->getFD(),&m_writeset);
		if (FD_ISSET(pHandler->getFD(),&m_readset))
			FD_CLR(pHandler->getFD(),&m_readset);
		m_Set.delFDEventHandler(pHandler->getFD());
		m_Set.addCloseSocket(pHandler->getFD());
		return 0;
	}

	int NetRector::stop()
	{
		m_bRunning = false;
		return 0;
	}

	bool NetClientReactor::run()
	{
		m_bRunning = true;

		while(true)
		{
			//执行可能的心跳包
			Reactor::run();

			int maxfd = -1;
			fd_set readset;
			fd_set writeset;
			MAPSOCKETFDH tmpFDMap;
			FD_ZERO(&readset);
			FD_ZERO(&writeset);
			tmpFDMap.clear();

			{
				MutexScoped lock(m_cs);

				readset = m_readset;
				writeset = m_writeset;
				tmpFDMap = m_Set.m_FDEHMap;
				//把一些无效的描述符关闭
				m_Set.dealClose();

				//遍历超时列表，查看是否有socket连接超时
				m_Set.scan();
				//空闲处理
				m_Set.idle();
			}

			if(tmpFDMap.empty())
			{
				SleepMs(100);
				if(!m_bRunning) {
					break;
				}
				continue;
			}
			else
			{
				MAPSOCKETFDH::iterator it = tmpFDMap.begin();
				for(;it!=tmpFDMap.end();it++)
				{
					if (maxfd < (int)it->first)
						maxfd =(int) it->first;
				}
			}
			maxfd = maxfd+1;//最大描述符+1

			struct timeval tv = {0,50};//50微秒
			int nfds = select(maxfd,&readset,&writeset,NULL,&tv);
			if(nfds == SOCKET_ERROR)
			{
#ifdef _WIN32 
				LOGE("%s : select error :  %d\n",__FUNCTION__,WSAGetLastError());
#else
				LOGE("%s : select error : %d\n",__FUNCTION__,errno);
#endif
			}

			if(!m_bRunning && nfds <= 0)
				break;

			MAPSOCKETFDH::iterator it = tmpFDMap.begin();
			for(; it != tmpFDMap.end(); it++)
			{
				SOCKET sock = it->first;

				if(FD_ISSET(sock,&writeset))
				{
					if(it->second)
						it->second->onFDWrite();
				}
				if(FD_ISSET(sock,&readset))
				{
					if(it->second)
						it->second->onFDRead();
				}
			}
		}
		return false;//结束线程
	}

	bool NetServerReactor::run()
	{
		m_bRunning = true;

		while(true)
		{
			//执行可能的心跳包
			Reactor::run();

			int maxfd = -1;
			fd_set readset;
			fd_set writeset;
			MAPSOCKETFDH tmpFDMap;
			FD_ZERO(&readset);
			FD_ZERO(&writeset);
			tmpFDMap.clear();

			{
				MutexScoped lock(m_cs);

				readset = m_readset;
				writeset = m_writeset;
				tmpFDMap = m_Set.m_FDEHMap;
				//把一些无效的描述符关闭
				m_Set.dealClose();

				//遍历超时列表，查看是否有socket连接超时
				m_Set.scan();
				//空闲处理
				m_Set.idle();
			}

			if(tmpFDMap.empty())
			{
				SleepMs(100);
				if(!m_bRunning) {
					break;
				}
				continue;
			}
			else
			{
				MAPSOCKETFDH::iterator it = tmpFDMap.begin();
				for(;it!=tmpFDMap.end();it++)
				{
					if (maxfd < (int)it->first)
						maxfd =(int) it->first;
				}
			}
			maxfd = maxfd+1;//最大描述符+1

			struct timeval tv = {0,50};//50微秒
			int nfds = select(maxfd,&readset,&writeset,NULL,&tv);
			if(nfds == SOCKET_ERROR)
			{
				LOGE("%s : server select error :  %d\n",__FUNCTION__,errno);
			}

			if(!m_bRunning && nfds <= 0)
				break;

			MAPSOCKETFDH::iterator it = tmpFDMap.begin();
			for(; it != tmpFDMap.end(); it++)
			{
				SOCKET sock = it->first;

				if(FD_ISSET(sock,&writeset))
				{
					if(it->second)
						it->second->onFDWrite();
				}
				if(FD_ISSET(sock,&readset))
				{
					if(it->second)
						it->second->onFDRead();
				}
			}
		}
		return false;
	}
}
