#ifndef GLP_EVENT_POSIX_H_
#define GLP_EVENT_POSIX_H_

#include "event.h"
#include <pthread.h>
#include <time.h>

#include "thread_wrapper.h"

namespace Tool{

enum State {
  kUp = 1,
  kDown = 2
};

class EventPosix : public Event {
 public:
  static Event* Create();

  virtual ~EventPosix();

  virtual EventType wait(unsigned long max_time);
  virtual bool set();
  virtual bool reset();

  virtual bool startTimer(bool periodic, unsigned long time);
  virtual bool stopTimer();

 private:
  EventPosix();
  int Construct();

  static bool Run(ThreadObj obj);
  bool Process();
  EventType Wait(timespec& wake_at);

 private:
  pthread_cond_t  cond_;
  pthread_mutex_t mutex_;

  Thread* timer_thread_;
  EventPosix*    timer_event_;
  timespec       created_at_;

  bool          periodic_;
  unsigned long time_;  // In ms
  unsigned long count_;
  State         state_;
};

}

#endif  // GLP_EVENT_POSIX_H_
