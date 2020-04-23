//
// Created by cgutech on 2020/4/16.
//

#ifndef SERIAL_TRANSFER_SERVER_SERIAL_HANDLER_IMPL_WITH_BOOST_H
#define SERIAL_TRANSFER_SERVER_SERIAL_HANDLER_IMPL_WITH_BOOST_H
#include "web_sock.h"
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>

class SerialHandlerImpl : public ISerialHandler, public boost::enable_shared_from_this<SerialHandlerImpl> {
public:
    SerialHandlerImpl(boost::asio::io_service& );
	~SerialHandlerImpl();
    virtual bool Open(const char* port, int baud);
    virtual void SetDataReceiver(IDataReceiver<ISerialHandler>* receiver, std::shared_ptr<ISerialHandler>& obj);
    virtual bool Write(uint8_t* data, int size);
    virtual void Close();
	virtual const std::string& GetKey();
private:
    void doRead();
    void onRead(boost::system::error_code ec, std::size_t bytes_transferred);
private:

	std::string key;
	boost::asio::io_service& io;
	boost::asio::serial_port serial;
	IDataReceiver<ISerialHandler> *receiver;
	std::shared_ptr<ISerialHandler> obj;
    enum { max_length = 2048 };
    uint8_t buffer[max_length];
};


#endif //SERIAL_TRANSFER_SERVER_SERIAL_HANDLER_IMPL_WITH_BOOST_H
