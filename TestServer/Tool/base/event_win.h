#ifndef GLP_EVENT_WIN_H_
#define GLP_EVENT_WIN_H_

#include <windows.h>
#include "event.h"

namespace Tool{

class EventWindows : public Event {
 public:
  EventWindows();
  virtual ~EventWindows();

  //miro second
  virtual EventType Wait(unsigned long max_time);
  virtual bool Set();
  virtual bool Reset();

  virtual bool StartTimer(bool periodic, unsigned long time);
  virtual bool StopTimer();

 private:
  HANDLE  event_;
  unsigned int timerID_;
};

}

#endif  // GLP_EVENT_WIN_H_
