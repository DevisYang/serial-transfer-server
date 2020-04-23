//
// Created by cgutech on 2020/4/16.
//

#include "SerialHandlerImpl.h"
#include <iostream>
#include <boost/bind.hpp>

SerialHandlerImpl::SerialHandlerImpl(boost::asio::io_service & io)
: io(io), serial(io){
	std::cout << "创建串口对象" << std::endl;
}

SerialHandlerImpl::~SerialHandlerImpl()
{
	std::cout << "销毁串口对象" << std::endl;
}

bool SerialHandlerImpl::Open(const char *port, int baud) {
	if (serial.is_open()) {
		return false;
	}
	try {
		serial.open(port);
		serial.set_option(boost::asio::serial_port::baud_rate(baud));
		serial.set_option(boost::asio::serial_port::flow_control());
		serial.set_option(boost::asio::serial_port::parity());
		serial.set_option(boost::asio::serial_port::stop_bits());
		serial.set_option(boost::asio::serial_port::character_size(8));
	}
	catch (boost::system::system_error& ec) {
		std::cout << "打开串口失败: " << ec.what() << std::endl;
		return false;
	}
	key = port;
	doRead();
	return true;
}

void SerialHandlerImpl::SetDataReceiver(IDataReceiver<ISerialHandler> *r, std::shared_ptr<ISerialHandler>& o) {
	this->receiver = r;
	this->obj = o;
}

bool SerialHandlerImpl::Write(uint8_t* data, int size) {
	if (!serial.is_open()) {
		return false;
	}
	boost::system::error_code ec;
	serial.write_some(boost::asio::buffer(data, size), ec);
    return !ec.failed();
}

void SerialHandlerImpl::doRead() {
    serial.async_read_some(boost::asio::buffer(buffer, sizeof(buffer)), boost::bind(&SerialHandlerImpl::onRead, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void SerialHandlerImpl::onRead(boost::system::error_code ec, std::size_t bytes_transferred) {
    if(ec) {
		char tmp[1024] = { 0 };
        std::cout<<"读取失败:"<<ec.message(tmp, 1024)<<std::endl;
		obj = nullptr;
        return;
    }

    receiver->onData(buffer, bytes_transferred, obj);

    doRead();
}

void SerialHandlerImpl::Close() {
    if(!serial.is_open()) {
        return;
    }
    serial.close();
}

const std::string & SerialHandlerImpl::GetKey()
{
	return key;
}
