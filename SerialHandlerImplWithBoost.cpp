//
// Created by cgutech on 2020/4/16.
//

#include "SerialHandlerImplWithBoost.h"

SerialHandlerImplWithBoost::SerialHandlerImplWithBoost(const boost::asio::io_service &) {

}

void SerialHandlerImplWithBoost::Open(const char *port, int baud) {

}

void SerialHandlerImplWithBoost::SetDataReceiver(IDataReceiver *receiver, void *obj) {

}

bool SerialHandlerImplWithBoost::Write(uint8_t *data, int size) {
    return false;
}

void SerialHandlerImplWithBoost::Close() {

}

void SerialHandlerImplWithBoost::Release() {

}
