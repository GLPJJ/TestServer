/*
	注释添加以及修改于 2014-4-3

	提供临界区的封装，以及对临界区友好使用的类CriticalSectionScoped 范围锁：只在生命周期内锁住。
	VoidGuard 防止内存泄露的友好类。是用free释放，所以只能保护malloc之类创建的内存

	提供字符集之间的转换的函数。
*/
#ifndef GLP_MUTEX_H_
#define GLP_MUTEX_H_

#include "object.h"

namespace Tool{

class Mutex {
public:
	// Factory method, constructor disabled
	static Mutex* CreateCriticalSection();

	virtual ~Mutex() {}

	// Tries to grab lock, beginning of a critical section. Will wait for the
	// lock to become available if the grab failed.
	virtual void enter() = 0;

	// Returns a grabbed lock, end of critical section.
	virtual void leave() = 0;
};

// RAII extension of the critical section. Prevents Enter/Leave mismatches and
// provides more compact critical section syntax.
class MutexScoped {
public:
	explicit MutexScoped(Mutex* critsec)
		: ptr_crit_sec_(critsec) {
				ptr_crit_sec_->enter();
	}

	~MutexScoped() {
		if (ptr_crit_sec_) {
			ptr_crit_sec_->leave();
			ptr_crit_sec_ = 0;
		}
	}

private:
	Mutex* ptr_crit_sec_;//release by users;
};

}


#endif//GLP_MUTEX_H_
