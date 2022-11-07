//
// Created by jinyf on 2022/11/7.
//

#include "MySelect.h"
#include <sys/select.h>
#include "unistd.h"

MySelect::MySelect(JavaVM *jVm, JNIEnv *jEnv, jobject *jobj) {
    this->jVm=jVm;
    this->jEnv=jEnv;
    this->jobj=jEnv->NewGlobalRef(*jobj);
}

MySelect::~MySelect() {

}



void callbackToJava(MySelect *player, char *s) {
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


void *playServer(void *arg){



    MySelect *myFun= static_cast<MySelect *>(arg);


    int listenfd= socket(AF_INET,SOCK_STREAM,0);
    if (listenfd < 0) {
        INFO("listenfd error!")
    }
    sockaddr_in server_addr;
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    inet_aton(myFun->address, &(server_addr.sin_addr));
    server_addr.sin_port = htons(myFun->prot);
    bind(listenfd, reinterpret_cast<const sockaddr *>(&server_addr), sizeof(server_addr));
    listen(listenfd,20);


    //select
    fd_set read_fds;
    fd_set work_fds;

    //select函数的超时时间
    struct timeval time_out;
    time_out.tv_sec=2;
    time_out.tv_usec=0;

    FD_ZERO(&read_fds);
    FD_SET(listenfd,&read_fds);
//    FD_SET(STDIN_FILENO,&read_fds);

    int max_fd=listenfd;

    char recv_buf[1024];


    INFO("run while");
    while (1) {

        //maxfdp：被监听的文件描述符的总数，它比所有文件描述符集合中的文件描述符的最大值大1，因为文件描述符是从0开始计数的；
        //readfds、writefds、exceptset：分别指向可读、可写和异常等事件对应的描述符集合。
        //timeout:用于设置select函数的超时时间，即告诉内核select等待多长时间之后就放弃等待。timeout == NULL 表示等待无限长的时间
        //返回值：超时返回0;失败返回-1；成功返回大于0的整数，这个整数表示就绪描述符的数目。
        work_fds=read_fds;
        int ret = select (max_fd + 1, &work_fds, NULL, NULL, &time_out);
        if (ret<=0){
            continue;
        }
        INFO("select ready")

        for (int i = 0; i < max_fd + 1; i++) {
//            INFO("select fd:%d",i);

            //FD_ISSET 是否有动作
            if (!FD_ISSET(i, &work_fds)) {
                continue;
            }
            int fd = i;
            if (fd == listenfd) {
                //有新客户端接入
                sockaddr_in client_addr;
                int client_addrlen = sizeof(client_addr);
                int cfd = accept(listenfd, (struct sockaddr *)&client_addr,
                                 (socklen_t *)&client_addrlen);
                if (cfd < 0) {
                    INFO("accept cfd < 0!");
                    continue;
                }
                char *ip_address = inet_ntoa(client_addr.sin_addr);
                INFO("accept fd:%d---%s ", cfd,ip_address);

                FD_SET(cfd, &read_fds);
                if (cfd > max_fd) {
                    max_fd = cfd;
                }
            }else{
                ssize_t num_read = recv(fd, recv_buf, sizeof(recv_buf), 0);
                if (num_read>0){
                    recv_buf[num_read]='\0';
                    INFO("fd:%d :%s",fd,recv_buf);
                    callbackToJava(myFun,recv_buf);
                    //回复客户端
                    send(fd, recv_buf, (size_t)num_read, 0);
                }else{
                    INFO("fd:%d :no message",fd);
                    //移除fd
                    FD_CLR(fd,&read_fds);
                    //关闭fd
                    close(fd);
                    continue;
                }
            }

        }

    }


    pthread_exit(myFun->t_id);
}


void MySelect::startServer(const char *address, int port) {

    this->address=address;
    this->prot=port;

    pthread_t _tid;
    this->t_id=&_tid;
    pthread_create(t_id,NULL,playServer,this);
}
