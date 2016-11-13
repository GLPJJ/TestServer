#ifndef GLP_REACTOR_H_
#define GLP_REACTOR_H_

#include "base.h"
#include <map>
#include <set>
#include <time.h>
#include "eventhandler.h"
#include "serversocket.h"

namespace Tool
{
	class Mutex;

	typedef void (* RUNKEEPLIVE)();
	class Reactor
	{
		DISALLOW_COPY_AND_ASSIGN(Reactor);
	public:
		Reactor():m_funKeepLive(NULL){}
		virtual ~Reactor(){}
		virtual int registerTimer(TMEventHandler *pHandler,time_t to) = 0;
		virtual int registerIdle(IdleEventHandler *pHandler) = 0;
		virtual int registerReadEvent(FDEventHandler *pHandler) = 0;
		virtual int registerWriteEvent(FDEventHandler *pHandler) = 0;

		virtual int unRegisterTimer(TMEventHandler *pHandler) = 0;
		virtual int unRegisterIdle(IdleEventHandler *pHandler) = 0;	
		virtual int unRegisterEvent(FDEventHandler *pHandler) = 0;
		virtual int unRegisterReadEvent(FDEventHandler *pHandler) = 0;
		virtual int unRegisterWriteEvent(FDEventHandler *pHandler) = 0;

		virtual bool run(){if(m_funKeepLive)m_funKeepLive();return true;}
		virtual int stop() = 0;

		RUNKEEPLIVE m_funKeepLive;
	};

	typedef std::map<SOCKET,FDEventHandler* > MAPSOCKETFDH;
    typedef std::set<SOCKET> SETSOCKET;
	typedef std::set<IdleEventHandler*> SETIDLEH;

	class EventHandlerSet
	{
		typedef struct TMEHInfo
		{
			TMEventHandler *pHandler;
			time_t life;//寿命
			time_t regtime;//注册时间
		}TMEHINFO;
		typedef std::map<int,TMEHINFO > MAPINTTMHINFO;

	public:	
		//加入到超时map中，如果发现已经存在则修改
		void addTMEventHandler(TMEventHandler *pHandler,time_t to);
		//从超时map中删除
		void delTMEventHandler(TMEventHandler *pHandler){if(pHandler)m_TMEHMap.erase(pHandler->getTimerID());}
		//加入到空闲set中
		void addIdleEventHandler(IdleEventHandler *pHandler){if(pHandler)m_IdleEHList.insert(pHandler);}
		//从空闲set中删除
		void delIdleEventHandler(IdleEventHandler *pHandler){if(pHandler)m_IdleEHList.erase(pHandler);}
		//加入到socketmap中
		void addFDEventHandler(FDEventHandler *pHandler){if(pHandler)m_FDEHMap[pHandler->getFD()] = pHandler;}
		//从socketmap中删除
		void delFDEventHandler(SOCKET s){m_FDEHMap.erase(s);}
        //加入关闭set中
        void addCloseSocket(SOCKET s){
			m_setCloseSocket.insert(s);
		}
		//遍历超时列表
		void scan();
		//处理空闲列表
		void idle();
        //处理需要关闭的描述符
        void dealClose();
	public:
		MAPSOCKETFDH m_FDEHMap;
	private:
        SETSOCKET    m_setCloseSocket;
		MAPINTTMHINFO m_TMEHMap;
		SETIDLEH m_IdleEHList;
	};


	class NetRector : public Reactor
	{
	public:
		NetRector();
		virtual ~NetRector();

	public:
		/*
		@param to:单位，秒
		*/
		virtual int registerTimer(TMEventHandler *pHandler,time_t to);
		virtual int registerReadEvent(FDEventHandler *pHandler);
		virtual int registerWriteEvent(FDEventHandler *pHandler);
		virtual int registerIdle(IdleEventHandler *pHandler);

		virtual int unRegisterTimer(TMEventHandler *pHandler);
		virtual int unRegisterEvent(FDEventHandler *pHandler);
		virtual int unRegisterReadEvent(FDEventHandler *pHandler);
		virtual int unRegisterWriteEvent(FDEventHandler *pHandler);
		virtual int unRegisterIdle(IdleEventHandler *pHandler);

		virtual int stop();

	protected:
		Mutex* m_cs;
		fd_set m_readset;
		fd_set m_writeset;

		bool m_bRunning;
		EventHandlerSet m_Set;
	};

	class NetClientReactor : public NetRector
	{
	public:
		NetClientReactor(){}
		virtual ~NetClientReactor(){}

	public:
		virtual bool run();
	};

	class NetServerReactor : public NetRector
	{
	public:
		NetServerReactor(){}
		virtual ~NetServerReactor(){}

	public:
		virtual bool run();

	private:
		
	};
}
#endif//GLP_REACTOR_H_
