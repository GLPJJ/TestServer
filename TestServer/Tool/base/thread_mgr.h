#ifndef THREADMGR__H__
#define THREADMGR__H__

#include "thread.h"
#include <list>

namespace Tool{

class Event;
class Thread;
class Mutex;

enum
{
	MSG_INVALID = 0,
};

struct _tMsg//定义消息结构
{
	bool		isSend;//是否立即发送
	int		id;
	void*	pData;
};

class ThreadMgr
{
public:
	ThreadMgr(void);
	virtual ~ThreadMgr(void);

public:
//创建线程，指定优先级
	bool CreateThread(ThreadPriority iPriority = kNormalPriority);
//资源回收
	bool ReleaseThread();
//投递消息
	void PostMessageOS( int id, void* pData );
//发送消息
	bool SendMessageOS( int id, void* pData );
//消息处理
	virtual void ProcessMessage(int id,void* pData) = 0;
//空闲处理
	virtual void ProcessIdle();
//线程run函数
	bool Run();
protected:
//线程函数
	static bool ThreadProc(ThreadObj pData);

protected:
	volatile bool			m_bStop;
private:
	Thread*				m_pObjectThread;
	Event*					m_pObjectEvent;
	Mutex*					m_pObjectCS;
	std::list<_tMsg>	m_lstMsg;
};

}
#endif//THREADMGR__H__
