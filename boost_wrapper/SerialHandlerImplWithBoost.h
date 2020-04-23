//
// Created by cgutech on 2020/4/16.
//

#ifndef SERIAL_TRANSFER_SERVER_SERIAL_HANDLER_IMPL_WITH_BOOST_H
#define SERIAL_TRANSFER_SERVER_SERIAL_HANDLER_IMPL_WITH_BOOST_H
#include "web_sock.h"
#include <boost/asio.hpp>

class SerialHandlerImplWithBoost : public ISerialHandler {
public:
    SerialHandlerImplWithBoost(const boost::asio::io_service& );
    virtual bool Open(const char* port, int baud);
    virtual void SetDataReceiver(IDataReceiver<ISerialHandler>* receiver, std::shared_ptr<ISerialHandler>& obj);
    virtual bool Write(uint8_t* data, int size);
    virtual void Close();
	virtual const std::string& GetKey();
private:
	std::string key;
};


#endif //SERIAL_TRANSFER_SERVER_SERIAL_HANDLER_IMPL_WITH_BOOST_H
