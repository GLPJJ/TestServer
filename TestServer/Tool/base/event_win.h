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
  virtual EventType wait(unsigned long max_time);
  virtual bool set();
  virtual bool reset();

  virtual bool startTimer(bool periodic, unsigned long time);
  virtual bool stopTimer();

 private:
  HANDLE  event_;
  unsigned int timerID_;
};

}

#endif  // GLP_EVENT_WIN_H_
