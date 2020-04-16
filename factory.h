//
// Created by cgutech on 2020/4/16.
//

#ifndef SERIAL_TRANSFER_SERVER_FACTORY_H
#define SERIAL_TRANSFER_SERVER_FACTORY_H
#include "web_sock.h"
#include <boost/asio.hpp>

class WebSockServerFactoryImplWithBoost : public IWebSockServerSerialRunner{
public:
    virtual IWebSockServer* CreateWebSockServer();
    virtual ISerialHandler* CreateSerialHandler();
    virtual void Run();
    virtual void Release();
private:
    boost::asio::io_service io;
};

#endif //SERIAL_TRANSFER_SERVER_FACTORY_H
