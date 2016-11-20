#ifndef GLP_LOG_H_
#define GLP_LOG_H_

namespace Tool{

	void InitLogDir(const char* dir);

	void Log(char* pszLog,...);
	void LogCiphertext(const unsigned char* ciphertext,size_t len);
	void LogN();

	// This function sleeps for the specified number of milliseconds.
	// It may return early if the thread is woken by some other event,
	// such as the delivery of a signal on Unix.
	void SleepMs(int msecs);
}

#ifdef _WIN32
// VERBOSE
#define LOGV(...) Log(__VA_ARGS__)
// DEBUG
#define LOGD(...) Log(__VA_ARGS__)
// INFO
#define LOGI(...) Log(__VA_ARGS__)
//WARN
#define LOGW(...) Log(__VA_ARGS__)
// ERROR
#define LOGE(...) Log(__VA_ARGS__)
#else
#include <android/log.h>
// VERBOSE
#define LOGV(...)   __android_log_print(ANDROID_LOG_VERBOSE, "Tool", __VA_ARGS__)
// DEBUG
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG , "Tool ", __VA_ARGS__)
// INFO
#define LOGI(...)    __android_log_print(ANDROID_LOG_INFO  , "Tool ",__VA_ARGS__)
//WARN
#define LOGW(...)  __android_log_print(ANDROID_LOG_WARN  , "Tool ", __VA_ARGS__)
// ERROR
#define LOGE(...)   __android_log_print(ANDROID_LOG_ERROR  , "Tool ",__VA_ARGS__)

#define LOGGLP(...)  __android_log_print(ANDROID_LOG_INFO  , " GLP ",__VA_ARGS__)
#endif

#endif//GLP_LOG_H_
