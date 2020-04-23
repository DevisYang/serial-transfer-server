cmake_minimum_required(VERSION 3.0.0)
project(boost_wrapper)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
message("库输出目录：${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

SET(CMAKE_BUILD_TYPE "Release")
# boost从1.66开始支持websocket，这里使用最新版本1.72
if(MSVC) # VS编译时，release版本使用MT
    set(CMAKE_CXX_FLAGS_RELEASE "/MT")
    # 如果使用MSVC编译器，boost可以直接下载已经编译好的版本，不需要自己编译
    link_directories(G:/library/boost_1_72_0/lib32-msvc-14.1)
    ADD_DEFINITIONS(-D MSVC)
	add_compile_options("/utf-8")
ELSE()
    link_directories(G:/source/boost_1_72_0/stage/lib)
endif()
ADD_DEFINITIONS(-D DLL_WRAPPER_API)
set(WEB_SOCK_SOURCE web_sock.h web_sock.cpp WebSockServerImplWithBeast.cpp WebSockServerImplWithBeast.h SerialHandlerImpl.cpp SerialHandlerImpl.h factory.cpp factory.h)

# 以动态链接库方式提供boost库的封装
add_library(web_sock SHARED ${WEB_SOCK_SOURCE})
target_include_directories(web_sock PRIVATE G:/source/boost_1_72_0)
# windows下，需要添加ws2_32.lib库，不然在链接的时候会出现符号不存在的问题
if(WIN32)
    target_link_libraries(web_sock ws2_32 wsock32)
ENDIF(WIN32)

