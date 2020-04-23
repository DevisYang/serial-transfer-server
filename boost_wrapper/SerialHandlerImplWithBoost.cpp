//
// Created by cgutech on 2020/4/16.
//

#include "SerialHandlerImplWithBoost.h"

SerialHandlerImplWithBoost::SerialHandlerImplWithBoost(const boost::asio::io_service &) {

}

bool SerialHandlerImplWithBoost::Open(const char *port, int baud) {
	return false;
}

void SerialHandlerImplWithBoost::SetDataReceiver(IDataReceiver<ISerialHandler> *receiver, std::shared_ptr<ISerialHandler>& obj) {

}

bool SerialHandlerImplWithBoost::Write(uint8_t* data, int size) {
    return false;
}

void SerialHandlerImplWithBoost::Close() {

}

const std::string & SerialHandlerImplWithBoost::GetKey()
{
	return key;
}
