#ifndef GLP_THREAD_WIN_H_
#define GLP_THREAD_WIN_H_

#include "thread.h"
#include <windows.h>

namespace Tool{

class Mutex;

class ThreadWindows : public Thread
{
public:
  ThreadWindows(ThreadRunFunction func, ThreadObj obj, ThreadPriority prio,
                const char* thread_name);
  virtual ~ThreadWindows();

  virtual bool start(unsigned int& id);
  bool setAffinity(const int* processor_numbers,const unsigned int amount_of_processors);
  virtual bool stop();
  virtual void setNotAlive();
  virtual bool waitFor(unsigned int ms=TOOL_EVENT_INFINITE);
  virtual bool terminate(unsigned long ecode);

  static unsigned int WINAPI StartThread(LPVOID lp_parameter);

 protected:
  virtual void Run();

private:
  ThreadRunFunction    run_function_;
  ThreadObj           obj_;

  bool                    alive_;
  bool                    dead_;

  // TODO(hellner)
  // do_not_close_handle_ member seem pretty redundant. Should be able to remove
  // it. Basically it should be fine to reclaim the handle when calling stop
  // and in the destructor.
  bool                    do_not_close_handle_;
  ThreadPriority     prio_;
  Mutex*				critsect_stop_;

  HANDLE              thread_;
  unsigned int        id_;
  char                    name_[kThreadMaxNameLength];
  bool                    set_thread_name_;

};

}

#endif  // GLP_THREAD_WIN_H_
