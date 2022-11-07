//
// Created by jinyf on 2022/10/26.
//

#include "MyEpoll.h"


//#include <stdio.h>
//#include <fcntl.h>
//#include <netinet/in.h>
#include "unistd.h"
#include <sys/epoll.h>



void callbackToJava(MyEpoll *player, char *s) {
//    sleep(3);
    char *result_data = s;

    jobject jobj = player->jobj;

    JNIEnv *localEnvInThread = NULL;
    int ret = player->jVm->AttachCurrentThread(&localEnvInThread, 0);
    if (ret == JNI_OK) {
        //其实env->GetObjectClass和env->GetMethodID以及env->DeleteGlobalRef
        //都可以用player->jEnv
        //除了CallVoidMethod必须用localEnvInThread，因为他回调了jvm的方法？
        jclass jclz = localEnvInThread->GetObjectClass(player->jobj);
        jmethodID jmid = localEnvInThread->GetMethodID(jclz, "onResultShow", "(Ljava/lang/String;)V");

        jstring j_result_data=localEnvInThread->NewStringUTF(result_data);
        localEnvInThread->CallVoidMethod(jobj, jmid, j_result_data);

        player->jVm->DetachCurrentThread();
//        player->jEnv->DeleteGlobalRef(jobj);
    } else {
        INFO("AttachCurrentThread ERROR!")
    }
//    pthread_exit((player->t_id));
}


#define LISTENQ 20
#define MAXLINE 512

void *play_socket(void *arg) {
    MyEpoll *myFun = static_cast<MyEpoll *>(arg);

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        INFO("listenfd error!")
    }

    sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    inet_aton(myFun->address, &(server_addr.sin_addr));
    server_addr.sin_port = htons(myFun->prot);
    bind(listenfd, reinterpret_cast<const sockaddr *>(&server_addr), sizeof(server_addr));
    listen(listenfd,LISTENQ);


    int epollfd = epoll_create(58);
    epoll_event ev;

    ev.data.fd=listenfd;
    ev.events= EPOLLIN | EPOLLOUT;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,listenfd,&ev);

    const int ep_event_len=20;
    epoll_event ep_events[ep_event_len];

    int socket_fd;
    char line[MAXLINE];
    sockaddr_in client_addr;
    socklen_t clilen= sizeof(client_addr);


    while (1){
        INFO("epoll_wait 1");
        int nfds = epoll_wait(epollfd, ep_events,ep_event_len, -1);
        INFO("epoll_wait 2");
        for (int i = 0; i < nfds; ++i) {
            if (ep_events[i].data.fd == listenfd){
                INFO("new socket connected");
                //如果新监测到一个SOCKET用户连接到了绑定的SOCKET端口，建立新的连接。
                int connfd= accept(listenfd, reinterpret_cast<sockaddr *>(&client_addr), &clilen);
                if (connfd<0){
                    INFO("connfd<0: %d",connfd);
                    continue;
                }
                char *str = inet_ntoa(client_addr.sin_addr);
                INFO("accapt a connection from %s",str);

//                epoll_event ev;
                //设置用于读操作的文件描述符
                ev.data.fd = connfd;
                //设置用于注测的读操作事件
                ev.events = EPOLLIN | EPOLLET ;
                //注册ev
                epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &ev);
            }else if (ep_events[i].events & EPOLLOUT) {
                // 如果有数据发送
                INFO("EPOLLOUT");
//                socket_fd = ep_events[i].data.fd;
//                write(socket_fd, line, MAXLINE);
////                //设置用于读操作的文件描述符
//                ev.data.fd = socket_fd;
////                //设置用于注测的读操作事件
//                ev.events = EPOLLIN | EPOLLET;
////                //修改sockfd上要处理的事件为EPOLIN
//                epoll_ctl(epollfd, EPOLL_CTL_MOD, socket_fd, &ev);
            }else if (ep_events[i].events & EPOLLIN){
                //如果是已经连接的用户，并且收到数据，那么进行读入。
                INFO("EPOLLIN 1");
                if ((socket_fd = ep_events[i].data.fd) < 0){
                    continue;
                }
                INFO("EPOLLIN 2");
                ssize_t n = read(socket_fd, line, MAXLINE);
                INFO("EPOLLIN :%d",n);
                if (n <= 0) {
                    close(socket_fd);
                    ep_events[i].data.fd = -1;
                    continue;
                }
                line[n] = '\0';
                INFO("read======>  %s",line);
                callbackToJava(myFun,line);
                //设置用于写操作的文件描述符
                ev.data.fd = socket_fd;
                //设置用于注测的写操作事件
                ev.events = EPOLLIN | EPOLLET;
                //修改sockfd上要处理的事件为EPOLLOUT
                epoll_ctl(epollfd, EPOLL_CTL_MOD, socket_fd, &ev);

            }
        }

    }

    pthread_exit((myFun->t_id));
}


void MyEpoll::startServer(const char *_address, int _port) {
    this->address = _address;
    this->prot = _port;
    pthread_t _tid;
    t_id = &_tid;
    pthread_create(t_id, NULL, play_socket, this);
}


MyEpoll::MyEpoll(JavaVM *_jVm, JNIEnv *_jEnv, jobject *_jobj) {
    this->jVm = _jVm;
    this->jEnv = _jEnv;
    this->jobj = _jEnv->NewGlobalRef(*_jobj);
}

MyEpoll::~MyEpoll() {

}
