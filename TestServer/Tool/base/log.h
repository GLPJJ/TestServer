#ifndef GLP_LOG_H_
#define GLP_LOG_H_

namespace Tool{

	void Log(char* pszLog,...);

	// This function sleeps for the specified number of milliseconds.
	// It may return early if the thread is woken by some other event,
	// such as the delivery of a signal on Unix.
	void SleepMs(int msecs);
}

#endif//GLP_LOG_H_
