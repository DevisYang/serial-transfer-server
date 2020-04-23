//
// Created by cgutech on 2020/4/16.
//

#ifndef SERIAL_TRANSFER_SERVER_FACTORY_H
#define SERIAL_TRANSFER_SERVER_FACTORY_H
#include "web_sock.h"
#include <boost/asio.hpp>

class BoostWrapperFactoryImpl : public IWrapperFactory {
public:
	BoostWrapperFactoryImpl(int threads);
    virtual std::unique_ptr<IWebSockServer> CreateWebSockServer();
    virtual std::shared_ptr<ISerialHandler> CreateSerialHandler();
    virtual void Run();
    virtual void Release();
private:
    boost::asio::io_service io;
	int threads;
	static void workThread(void* io);
};

#endif //SERIAL_TRANSFER_SERVER_FACTORY_H
