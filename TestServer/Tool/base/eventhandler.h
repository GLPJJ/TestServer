#ifndef GLP_EVENTHANDLER_H_
#define GLP_EVENTHANDLER_H_

#ifdef WIN32
	#include <Winsock2.h>
	#pragma comment(lib,"Ws2_32.lib")
#else
	typedef int SOCKET;
	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
	#include "unistd.h"
	#include "sys/socket.h"
	#include "arpa/inet.h"
	#include "netinet/in.h"
	#include <sys/types.h>
	#include <fcntl.h>
	#include <sys/stat.h>
	#include <errno.h>
	#include <netdb.h>
#endif
#include <time.h>
#include "base.h"

namespace Tool
{
	class Reactor;
	//提供一个反应器
	class EventHandler
	{
		DISALLOW_COPY_AND_ASSIGN(EventHandler);
	public:
		virtual ~EventHandler(){}
		EventHandler():m_pReactor(NULL){}

	public:
		void setReactor(Reactor *pReactor){m_pReactor = pReactor;}
		Reactor* getReactor(){return m_pReactor;}
		virtual void close() = 0;
	protected:
		Reactor *m_pReactor;
	};
	//对空闲的处理
	class IdleEventHandler : virtual public EventHandler
	{
	public:
		IdleEventHandler(Reactor *pReactor){setReactor(pReactor);}
		virtual ~IdleEventHandler(){}

	public:
		virtual void onRun() = 0;
		int registerIdle();
		int unRegisterIdle();
		virtual void close();
	};
	//对事件的处理
	class TMEventHandler : virtual public EventHandler
	{
	public:
		TMEventHandler():m_id(0){}
		TMEventHandler(Reactor *pReactor):m_id(0) {setReactor(pReactor);}
		virtual ~TMEventHandler() {}

	public:
		//超时处理函数
		virtual void onTimeOut() = 0;
		//定义超时秒数
		int registerTimer(time_t to);
		int unRegisterTimer();
		virtual void close();
		void setTimerID(int id) {m_id = id;}
		int getTimerID() {return m_id;}
	private:
		int m_id;
	};

	//对 socket事件的处理,虚继承，子类继承相同类可以只保留一份父类
	class FDEventHandler : virtual public EventHandler
	{
	public:
		virtual ~FDEventHandler() {}
		FDEventHandler():m_fd(INVALID_SOCKET) {}
		FDEventHandler(Reactor *pReactor):m_fd(INVALID_SOCKET)  {setReactor(pReactor);}
		
		//fd 读的时候 可接受
		virtual void onFDRead() = 0;
		//fd 写的时候 可发送
		virtual void onFDWrite() = 0;

// 		virtual void OnFDReadTimeOut() = 0;
// 		virtual void OnFDWriteTimeOut() = 0;

		virtual void close();

		int registerRead();
		int registerWrite();
		int unRegisterRead();
		int unRegisterWrite();

		//设置非阻塞 返回0 成功
		int setNonBlocking();
		//设置地址复用 返回0 成功
		int setAddrReuse();

		inline void setFD(SOCKET fd) {m_fd = fd;}
		inline SOCKET getFD() const {return m_fd;}
		inline void setClientID(int id){m_id = id;}
		inline int getClientID(){return m_id;}

	protected:
		void closeSocket();

	protected:
		SOCKET m_fd;//网络描述符
		int m_id;//用户ID
	};
}

#endif//GLP_EVENTHANDLER_H_
