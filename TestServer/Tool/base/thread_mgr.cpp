#include "../Tool.h"

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
	Thread::destroy(m_pObjectThread);
	Event::destroy(m_pObjectEvent);
	Mutex::destroy(m_pObjectCS);
}

bool ThreadMgr::CreateThread( ThreadPriority iPriority )
{
	if(m_pObjectThread)
		return true;

	m_pObjectThread = Thread::CreateThread(ThreadProc,this,kNormalPriority,"Net Thread");
	if(!m_pObjectThread)
		return false;
	unsigned int thread_id;
	if(!m_pObjectThread->Start(thread_id))
	{
		return false;
	}
	return true;
}

bool ThreadMgr::ReleaseThread()
{
	m_bStop = true;
	if(m_pObjectThread)
		m_pObjectThread->Stop();
	return true;
}

void ThreadMgr::PostMessageOS( int id, void* pData )
{
	_tMsg msg = { false, id, pData };
	
	{
		MutexScoped lock(m_pObjectCS);
		m_lstMsg.push_back( msg );
	}
}

bool ThreadMgr::SendMessageOS( int id, void* pData )
{
	_tMsg msg = { true, id, pData };

	{
		MutexScoped lock(m_pObjectCS);
		m_lstMsg.push_front( msg );
	}
	

	if(m_pObjectEvent->Wait(TOOL_EVENT_INFINITE) == kEventSignaled)
	{
		return true;
	}

	return false;
}

void ThreadMgr::ProcessIdle()
{
	SleepMs(50);
}

bool ThreadMgr::ThreadProc(ThreadObj pData)
{
	ThreadMgr * pThis = static_cast<ThreadMgr *>(pData);
	return pThis->Run();
}

bool ThreadMgr::Run()
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
			ProcessMessage(msg.id,msg.pData);
			if ( msg.isSend )
				m_pObjectEvent->Set();
		}
		else//空闲处理
		{
			ProcessIdle();
		}
	}

	return false;//结束线程
}

}
