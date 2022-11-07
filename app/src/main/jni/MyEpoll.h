//
// Created by jinyf on 2022/10/26.
//

#ifndef BITMAPJNI_PLAYER_H
#define BITMAPJNI_PLAYER_H

#include <jni.h>
#include <pthread.h>
#include "AndroidLog.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>

class MyEpoll {

public:

    pthread_t *t_id;

    JavaVM *jVm;
    JNIEnv *jEnv;
    jobject jobj;

    const char* address;
    int prot;

    //jobj要用指针，否则无法值传递
    MyEpoll(JavaVM *jVm, JNIEnv *jEnv, jobject *jobj);
    MyEpoll();

    ~MyEpoll();

    void startServer(const char *address,int port);
};


#endif //BITMAPJNI_PLAYER_H
