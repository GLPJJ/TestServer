#include "log.h"
#include <stdio.h>

#ifdef WIN32
#include <Windows.h>
extern HANDLE g_hStdOut;
#endif


namespace Tool{

#ifdef WIN32

void Log(char* pszLog,...)
{
	if (pszLog == NULL)
		return;
	char log[65525] = { 0 };

	va_list args;
	va_start(args,pszLog);
	vsprintf(log, pszLog, args);
	va_end(pszLog);
	WriteConsoleA(g_hStdOut, log, strlen(log), NULL, NULL);
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

#endif

}

