project(epollJni)

cmake_minimum_required(VERSION 3.10)


#include_directories(include)
aux_source_directory(. SOURCE)
#aux_source_directory(./jni JNI_API)

include_directories(jni)

#add_library(src_webrtc SHARED ${cpp_srcs})

add_library(
        epollJni

        SHARED
        ${SOURCE}
        #        ${JNI_API}
)


find_library(
        log-lib
        log)

target_link_libraries(
        epollJni

        ${log-lib}
)
