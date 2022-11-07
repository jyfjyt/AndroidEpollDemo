
#ifndef MYMUSIC_ANDROIDLOG_H
#define MYMUSIC_ANDROIDLOG_H

#endif //MYMUSIC_ANDROIDLOG_H

#include "android/log.h"


#define INFOD(FORMAT,...)			__android_log_print(ANDROID_LOG_ERROR,"123123",FORMAT,##__VA_ARGS__);
#define INFOV(FORMAT,...)			__android_log_print(ANDROID_LOG_ERROR,"123123",FORMAT,##__VA_ARGS__);
#define INFO(FORMAT,...)			__android_log_print(ANDROID_LOG_INFO,"123123",FORMAT,##__VA_ARGS__);
#define INFOW(FORMAT,...)			__android_log_print(ANDROID_LOG_WARN,"123123",FORMAT,##__VA_ARGS__);
#define INFOE(FORMAT,...)			__android_log_print(ANDROID_LOG_ERROR,"123123",FORMAT,##__VA_ARGS__);
#define INFOF(FORMAT,...)			__android_log_print(ANDROID_LOG_FATAL,"123123",FORMAT,##__VA_ARGS__);
