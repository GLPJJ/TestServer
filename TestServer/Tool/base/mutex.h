#ifndef GLP_MUTEX_H_
#define GLP_MUTEX_H_

namespace Tool{

class Mutex {
public:
	// Factory method, constructor disabled
	static Mutex* CreateCriticalSection();
	static void Destroy(Mutex* p);

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
