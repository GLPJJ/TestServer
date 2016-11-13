#include "counter.h"
#include "mutex.h"
#include "object.h"

namespace Tool{

Counter::Counter(int min,int max)
: m_count(min),m_min(min),m_max(max){
	mutex = Mutex::CreateCriticalSection();
}

Counter::~Counter(){
	delete_(Mutex,mutex);
}

inline int Counter::get(){
	MutexScoped lock(mutex);

	if(m_count >= m_max) m_count = m_min;
	return ++m_count;
}

}
