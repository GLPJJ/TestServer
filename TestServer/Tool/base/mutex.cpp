#include "mutex.h"
#include "object.h"

namespace Tool{

#ifdef _WIN32
#include <windows.h>
class CriticalSectionWindows : public Mutex {
public:
	CriticalSectionWindows(){InitializeCriticalSection(&crit);}

	virtual ~CriticalSectionWindows(){DeleteCriticalSection(&crit);}

	virtual void enter(){EnterCriticalSection(&crit);}
	virtual void leave(){LeaveCriticalSection(&crit);}

private:
	CRITICAL_SECTION crit;
};

#else 

#include <pthread.h>
class CriticalSectionPosix : public Mutex {
public:
	CriticalSectionPosix()
	{
		pthread_mutexattr_t attr;
		(void) pthread_mutexattr_init(&attr);
		(void) pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
		(void) pthread_mutex_init(&mutex_, &attr);
	}

	virtual ~CriticalSectionPosix(){(void) pthread_mutex_destroy(&mutex_);}

	virtual void enter(){(void) pthread_mutex_lock(&mutex_);}
	virtual void leave(){(void) pthread_mutex_unlock(&mutex_);}

private:
	pthread_mutex_t mutex_;
};
#endif//_WIN32

Mutex* Mutex::CreateCriticalSection() {
#ifdef _WIN32
	new_(CriticalSectionWindows,p);
	return (Mutex*)p;
#else
	new_(CriticalSectionPosix,p);
	return (Mutex*)p;
#endif//_WIN32
}

void Mutex::Destroy(Mutex* p){
	delete_(Mutex,p);
}

}
