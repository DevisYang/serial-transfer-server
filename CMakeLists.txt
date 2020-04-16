cmake_minimum_required(VERSION 3.0.0)
project(serial-transfer-server)

# boost从1.66开始支持websocket，这里使用最新版本1.72
include_directories(G:/source/boost_1_72_0)
link_libraries(G:/source/boost_1_72_0/stage/lib)

set(WEB_SOCK_SOURCE web_sock.h web_sock.cpp TransferServer.cpp TransferServer.h WebSockServerImplWithBeast.cpp WebSockServerImplWithBeast.h SerialHandlerImplWithBoost.cpp SerialHandlerImplWithBoost.h factory.cpp factory.h)

add_executable(serial-transfer-server main.cpp ${WEB_SOCK_SOURCE})
