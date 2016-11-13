#ifndef GLP_TIMER_H_
#define GLP_TIMER_H_

namespace Tool{

class Timer
{
public:
	Timer();                                    // default constructor
	~Timer();                                   // default destructor

	void	start();                             // start timer
	void	start2();							// cur_startCount
	void	stop();                              // stop the timer
	void	stop2();							// cur_endCount
	double getElapsedTime();                    // get elapsed time in second
	double getElapsedTimeInSec();               // get elapsed time in second (same as getElapsedTime)
	double getElapsedTimeInMilliSec();          // get elapsed time in milli-second
	double getElapsedTimeInMicroSec();          // get elapsed time in micro-second

	double getLastElapsedTimerInMilliSec();//获取上次的执行时间：毫秒
	double getLastElapsedTimeInMicroSec();//获取上次的执行时间：微秒

private:
	double startTimeInMicroSec;                 // starting time in micro-second
	double endTimeInMicroSec;                   // ending time in micro-second
	int    stopped;                             // stop flag 
};

};

#endif // GLP_TIMER_H_
