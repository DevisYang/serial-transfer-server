//
// Created by cgutech on 2020/4/16.
//

#ifndef SERIAL_TRANSFER_SERVER_WEBSOCKSERVERIMPLWITHBEAST_H
#define SERIAL_TRANSFER_SERVER_WEBSOCKSERVERIMPLWITHBEAST_H
#include "web_sock.h"
#include <map>
#include <string>
#include <boost/asio.hpp>
#include <boost/beast.hpp>

class WebSockHandlerImpl : public IWebSockHandler{
public:
    explicit WebSockHandlerImpl(
		boost::asio::ip::tcp::socket && socket, 
		std::map<std::string, std::shared_ptr<WebSockHandlerImpl>>& handlers, 
		std::string& ip,
		IWebSockServerHandlerListener* listener
	);
	~WebSockHandlerImpl();
    virtual void SetDataReceiver(IDataReceiver<IWebSockHandler>* r, std::shared_ptr<IWebSockHandler>& obj);
    virtual bool Write(uint8_t* data, int size);
    virtual void Close();
	virtual const std::string& GetKey();
private:

    void onRun();
    void onAccept(boost::beast::error_code ec);
    void doRead();
    void onRead(boost::beast::error_code ec, std::size_t bytes_transferred);
    void onWrite(boost::beast::error_code ec, std::size_t bytes_transferred);
private:
    boost::beast::websocket::stream<boost::beast::tcp_stream> ws;
    boost::beast::flat_buffer buffer;
    IDataReceiver<IWebSockHandler>* receiver;
	std::shared_ptr<IWebSockHandler> obj;
    const std::string ip;
    std::map<std::string, std::shared_ptr<WebSockHandlerImpl>>& handlers;
	IWebSockServerHandlerListener* listener;
};

class WebSockServerImplWithBeast : public IWebSockServer {
public:
    explicit WebSockServerImplWithBeast(boost::asio::io_service& io);
    virtual bool Open(int port);
	virtual void RegisterHandlerListener(IWebSockServerHandlerListener* listener);
	virtual bool Write(const std::string& key, uint8_t* data, int size);
    virtual void Close();
private:
    boost::asio::io_service& io;
	boost::asio::ip::tcp::acceptor acceptor;
private:
	void doAccept();
	void onAccept(boost::beast::error_code ec, boost::asio::ip::tcp::socket socket);
	std::map<std::string, std::shared_ptr<WebSockHandlerImpl>> handlers;
	IWebSockServerHandlerListener* listener;
};

#endif //SERIAL_TRANSFER_SERVER_WEBSOCKSERVERIMPLWITHBEAST_H
