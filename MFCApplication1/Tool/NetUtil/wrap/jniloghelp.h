/*
 *
 * 		2014-4-16
 * 		This file is used for debugging in eclipse to print some thing in the LogCat
 * 		Just like printf in th c.
 *
 */

#ifndef JNILOGHELP__H__
#define JNILOGHELP__H__
#ifdef _WIN32
// VERBOSE
#define LOGV(...) printf(__VA_ARGS__)
// DEBUG
#define LOGD(...) printf(__VA_ARGS__)
// INFO
#define LOGI(...) printf(__VA_ARGS__)
//WARN
#define LOGW(...) printf(__VA_ARGS__)
// ERROR
#define LOGE(...) printf(__VA_ARGS__)
#else
#include <android/log.h>
// VERBOSE
#define LOGV(...)   __android_log_print(ANDROID_LOG_VERBOSE, "JniNetUtil", __VA_ARGS__)
// DEBUG
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG , "JniNetUtil ", __VA_ARGS__)
// INFO
#define LOGI(...)    __android_log_print(ANDROID_LOG_INFO  , "JniNetUtil ",__VA_ARGS__)
//WARN
#define LOGW(...)  __android_log_print(ANDROID_LOG_WARN  , "JniNetUtil ", __VA_ARGS__)
// ERROR
#define LOGE(...)   __android_log_print(ANDROID_LOG_ERROR  , "JniNetUtil ",__VA_ARGS__)

#define LOGGLP(...)  __android_log_print(ANDROID_LOG_INFO  , " GLP ",__VA_ARGS__)
#endif

#endif//JNILOGHELP__H__
