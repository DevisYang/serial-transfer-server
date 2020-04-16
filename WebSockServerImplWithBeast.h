//
// Created by cgutech on 2020/4/16.
//

#ifndef SERIAL_TRANSFER_SERVER_WEBSOCKSERVERIMPLWITHBEAST_H
#define SERIAL_TRANSFER_SERVER_WEBSOCKSERVERIMPLWITHBEAST_H
#include "web_sock.h"
#include <boost/asio.hpp>
#include <boost/beast.hpp>

class WebSockHandlerImplWithBeast : public IWebSockHandler, public std::enable_shared_from_this<WebSockHandlerImplWithBeast> {
public:
    explicit WebSockHandlerImplWithBeast(boost::asio::ip::tcp::socket && socket);
    virtual void SetDataReceiver(IDataReceiver* r, void* obj);
    virtual bool Write(uint8_t* data, int size);
    virtual void Close();
    virtual void Release();
private:
    void run();
    void onRun();
    void onAccept(boost::beast::error_code ec);
    void doRead();
    void onRead(boost::beast::error_code ec, std::size_t bytes_transferred);
    void onWrite(boost::beast::error_code ec, std::size_t bytes_transferred);
private:
    boost::beast::websocket::stream<boost::beast::tcp_stream> ws;
    boost::beast::flat_buffer buffer;
    IDataReceiver* receiver;
    void* obj;
};

class WebSockServerImplWithBeast : public IWebSockServer {
public:
    explicit WebSockServerImplWithBeast(const boost::asio::io_service& io);
    virtual bool Open(int port);
    virtual void Close() ;
private:
    const boost::asio::io_service& io;
};



#endif //SERIAL_TRANSFER_SERVER_WEBSOCKSERVERIMPLWITHBEAST_H
