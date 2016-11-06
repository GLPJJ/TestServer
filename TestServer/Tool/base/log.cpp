#include "../Tool.h"
#include <stdio.h>

#ifdef WIN32

#include <Windows.h>
extern HANDLE g_hStdOut;

#else

// For nanosleep()
#include <time.h>

#endif


namespace Tool{

#ifdef _WIN32

void Log(char* pszLog,...)
{
	if (pszLog == NULL)
		return;
	char log[65525] = { 0 };

	va_list args;
	va_start(args,pszLog);
	vsprintf(log, pszLog, args);
	if(g_hStdOut != NULL)
		WriteConsoleA(g_hStdOut, log, strlen(log), NULL, NULL);
	OutputDebugStringA(log);
	va_end(pszLog);
}

#else

void Log(char* pszLog,...)
{
	if (pszLog == NULL)
		return;
	char log[65525] = { 0 };

	va_list args;
	va_start(args,pszLog);
	vsprintf(log, pszLog, args);
	va_end(pszLog);
	printf(log);
}

#endif//_WIN32

void SleepMs(int msecs) {
#ifdef _WIN32
	Sleep(msecs);
#else
	struct timespec short_wait;
	struct timespec remainder;
	short_wait.tv_sec = msecs / 1000;
	short_wait.tv_nsec = (msecs % 1000) * 1000 * 1000;
	nanosleep(&short_wait, &remainder);
#endif
}

}

