#include "../Tool.h"
#include <stdio.h>
#include <string.h>

#ifdef WIN32

#include <Windows.h>
extern HANDLE g_hStdOut;

#else

// For nanosleep()
#include <time.h>

#endif


namespace Tool{

	static char sLogDir[260] = {0};

	void WriteFile(char *buf)//写入文件
	{
		if(!buf)
			return;

		struct tm *t=NULL;
		char filename[200]={0};
		time_t tt;
		char filecontent[65535]={0};
		tt=time(0);//时间自1970-7-1，秒
		if( (time_t)-1==tt)
		{
			//printf("sockclient time 1 error\n");
			return ;
		}
		t=localtime(&tt);
		if( NULL==t )
		{
			//printf("sockclient time 2 error\n");
			return ;
		}

		sprintf(filename,"%s/%04d-%02d-%02d.log",sLogDir,t->tm_year+1900
			,t->tm_mon+1,t->tm_mday);//文件名
		FILE* pFile=fopen(filename,"a");
		if(NULL==pFile)
		{
			//printf("Create or open file error\n");
			return ;
		}
		fprintf(pFile,"[%02d:%02d:%02d] %s\n",t->tm_hour,t->tm_min,t->tm_sec,buf);//文件内容
		fflush(pFile);
		fclose(pFile);
	}

	void InitLogDir(const char* dir)
	{
		if(dir)
			strncpy(sLogDir,dir,strlen(dir));
	}

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
		WriteFile(log);
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

