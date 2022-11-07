//
// Created by jinyf on 2022/9/13.
//



#include <jni.h>
#include <stdio.h>
#include "AndroidLog.h"
#include "MyEpoll.h"
#include "MySelect.h"

extern "C" {

}

JavaVM *javaVM = NULL;

extern "C"
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    jint result = -1;
    javaVM = vm;
    JNIEnv *env;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        return result;
    }
    return JNI_VERSION_1_4;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_yfjin_epoll_MainActivity_startEpollServer(JNIEnv *env, jobject thiz, jstring _address,
                                              jint _port) {

    const char* address=env->GetStringUTFChars(_address,0);

    auto *myEpoll=new MyEpoll(javaVM, env, &thiz);
    myEpoll->startServer(address, _port);

    env->ReleaseStringUTFChars(_address,address);


}
extern "C"
JNIEXPORT void JNICALL
Java_com_yfjin_epoll_MainActivity_startSelectServer(JNIEnv *env, jobject thiz, jstring _address,
                                                    jint _port) {

    const char* address=env->GetStringUTFChars(_address,0);

    auto *mySelect=new MySelect(javaVM, env, &thiz);
    mySelect->startServer(address, _port);

    env->ReleaseStringUTFChars(_address,address);
}