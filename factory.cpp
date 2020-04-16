//
// Created by cgutech on 2020/4/16.
//

#include "factory.h"
#include "WebSockServerImplWithBeast.h"
#include "SerialHandlerImplWithBoost.h"
IWebSockServer *WebSockServerFactoryImplWithBoost::CreateWebSockServer() {
    return new WebSockServerImplWithBeast(io);
}

ISerialHandler *WebSockServerFactoryImplWithBoost::CreateSerialHandler() {
    return new SerialHandlerImplWithBoost(io);
}

void WebSockServerFactoryImplWithBoost::Run() {
    io.run();
}

void WebSockServerFactoryImplWithBoost::Release() {
    delete this;
}
