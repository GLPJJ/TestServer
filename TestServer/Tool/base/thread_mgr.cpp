#include "thread_mgr.h"
#include "mutex.h"
#include "event.h"
#include "log.h"

namespace Tool{

ThreadMgr::ThreadMgr(void)
: m_bStop( false )
,m_pObjectCS(NULL)
,m_pObjectEvent(NULL)
,m_pObjectThread(NULL)
{
	m_pObjectCS = Mutex::CreateCriticalSection();
	m_pObjectEvent = Event::Create();
}

ThreadMgr::~ThreadMgr(void)
{
	Thread::Destroy(m_pObjectThread);
	Event::Destroy(m_pObjectEvent);
	Mutex::Destroy(m_pObjectCS);
}

bool ThreadMgr::createThread( ThreadPriority iPriority )
{
	if(m_pObjectThread)
		return true;

	m_pObjectThread = Thread::CreateThread(ThreadProc,this,kNormalPriority,"Net Thread");
	if(!m_pObjectThread)
		return false;
	unsigned int thread_id;
	if(!m_pObjectThread->start(thread_id))
	{
		return false;
	}
	return true;
}

bool ThreadMgr::releaseThread()
{
	m_bStop = true;
	if(m_pObjectThread)
		m_pObjectThread->stop();
	return true;
}

void ThreadMgr::postMessageOS( int id, void* pData )
{
	_tMsg msg = { false, id, pData };
	
	{
		MutexScoped lock(m_pObjectCS);
		m_lstMsg.push_back( msg );
	}
}

bool ThreadMgr::sendMessageOS( int id, void* pData )
{
	_tMsg msg = { true, id, pData };

	{
		MutexScoped lock(m_pObjectCS);
		m_lstMsg.push_front( msg );
	}
	

	if(m_pObjectEvent->wait(TOOL_EVENT_INFINITE) == kEventSignaled)
	{
		return true;
	}

	return false;
}

void ThreadMgr::processIdle()
{
	SleepMs(50);
}

bool ThreadMgr::ThreadProc(ThreadObj pData)
{
	ThreadMgr * pThis = static_cast<ThreadMgr *>(pData);
	return pThis->run();
}

bool ThreadMgr::run()
{
	while ( !m_bStop )
	{
		bool bIsEmpty = false;
		_tMsg msg = { false, MSG_INVALID, 0 };
		
		{
			MutexScoped lock(m_pObjectCS);
			bIsEmpty = m_lstMsg.empty();
			if ( !bIsEmpty )
			{
				msg = m_lstMsg.front();
				m_lstMsg.pop_front();
			}
		}

		
		if ( !bIsEmpty && msg.id != MSG_INVALID )//如果不为空，并且消息不为MSG_INVALID,
		{
			processMessage(msg.id,msg.pData);
			if ( msg.isSend )
				m_pObjectEvent->set();
		}
		else//空闲处理
		{
			processIdle();
		}
	}

	return false;//结束线程
}

}
