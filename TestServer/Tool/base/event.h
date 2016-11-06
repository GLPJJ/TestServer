#ifndef GLP_EVENT_H_
#define GLP_EVENT_H_

//#include "base.h"

namespace Tool{

enum EventType{
  kEventSignaled = 1,
  kEventError = 2,
  kEventTimeout = 3
};

class Event {
public:
  // Factory method. Constructor disabled.
	static Event* Create();
	static void destroy(Event* p);

	virtual ~Event() {}

	// Releases threads who are calling Wait() and has started waiting. Please
	// note that a thread calling Wait() will not start waiting immediately.
	// assumptions to the contrary is a very common source of issues in
	// multithreaded programming.
	// Set is sticky in the sense that it will release at least one thread
	// either immediately or some time in the future.
	virtual bool Set() = 0;

	// Prevents future Wait() calls from finishing without a new Set() call.
	virtual bool Reset() = 0;

	// Puts the calling thread into a wait state. The thread may be released
	// by a Set() call depending on if other threads are waiting and if so on
	// timing. The thread that was released will call Reset() before leaving
	// preventing more threads from being released. If multiple threads
	// are waiting for the same Set(), only one (random) thread is guaranteed to
	// be released. It is possible that multiple (random) threads are released
	// Depending on timing.
	virtual EventType Wait(unsigned long max_time) = 0;

	// Starts a timer that will call a non-sticky version of Set() either once
	// or periodically. If the timer is periodic it ensures that there is no
	// drift over time relative to the system clock.
	virtual bool StartTimer(bool periodic, unsigned long time) = 0;

	virtual bool StopTimer() = 0;

};

}

#endif  // GLP_EVENT_H_
