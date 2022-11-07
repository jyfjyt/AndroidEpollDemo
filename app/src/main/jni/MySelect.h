//
// Created by jinyf on 2022/11/7.
//

#ifndef EPOLL_MYSELECT_H
#define EPOLL_MYSELECT_H

#include <jni.h>
#include <pthread.h>
#include "AndroidLog.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>


class MySelect {

public:
    pthread_t *t_id;

    JavaVM *jVm;
    JNIEnv *jEnv;
    jobject jobj;

    const char* address;
    int prot;

    //jobj要用指针，否则无法值传递
    MySelect(JavaVM *jVm, JNIEnv *jEnv, jobject *jobj);

    ~MySelect();

    void startServer(const char *address,int port);
};


#endif //EPOLL_MYSELECT_H
