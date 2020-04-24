//
// Created by cgutech on 2020/4/16.
//

#include <iostream>
#include <boost/format.hpp>
#include "WebSockServerImplWithBeast.h"
WebSockHandlerImpl::WebSockHandlerImpl(
	boost::asio::ip::tcp::socket&& socket, 
	std::map<std::string, std::shared_ptr<WebSockHandlerImpl>>& handlers, 
	std::string& ip,
	IWebSockServerHandlerListener* listener) : ws(std::move(socket)), receiver(nullptr), obj(nullptr), handlers(handlers), ip(ip), listener(listener) {
	std::cout << "创建客户端连接对象" << std::endl;
    boost::asio::dispatch(ws.get_executor(), boost::beast::bind_front_handler(&WebSockHandlerImpl::onRun, this));
}

WebSockHandlerImpl::~WebSockHandlerImpl()
{
	std::cout << "销毁客户端连接对象" << std::endl;
}

void WebSockHandlerImpl::SetDataReceiver(IDataReceiver<IWebSockHandler> *r, std::shared_ptr<IWebSockHandler>& obj) {
    this->receiver = r;
    this->obj = obj;
}

bool WebSockHandlerImpl::Write(uint8_t* data, int size) {
    ws.async_write(
            boost::asio::buffer(data, size),
            boost::beast::bind_front_handler(&WebSockHandlerImpl::onWrite, this));
    return true;
}

void WebSockHandlerImpl::Close() {
    boost::beast::websocket::close_reason cr("手动关闭");
    ws.close(cr);
}

const std::string & WebSockHandlerImpl::GetKey()
{
	return ip;
}


void WebSockHandlerImpl::onRun() {
    ws.set_option(boost::beast::websocket::stream_base::decorator([](boost::beast::websocket::response_type& res){
        res.set(boost::beast::http::field::server, std::string(BOOST_BEAST_VERSION_STRING) + "websocket-server-async");
    }));
    ws.async_accept(boost::beast::bind_front_handler(&WebSockHandlerImpl::onAccept, this));
}

void WebSockHandlerImpl::onAccept(boost::beast::error_code ec) {
    if(ec) {
		std::cout << "客户端onAccept失败:" << ec.message() << std::endl;
		handlers.erase(ip);
        return;
    }
	std::cout << "websocket客户端连接成功" << std::endl;
	if (listener != nullptr) { // 通知上层应用，客户端连接成功
		listener->onWebSockHandler(handlers[ip], ip);
	}
    doRead();
}

void WebSockHandlerImpl::doRead() {
	buffer.consume(buffer.size());
    ws.async_read(buffer, boost::beast::bind_front_handler(
            &WebSockHandlerImpl::onRead, this));
}

void WebSockHandlerImpl::onRead(boost::beast::error_code ec, std::size_t bytes_transferred) {
    if(ec) {
        std::cout<<"读取失败:"<<ec.message()<<std::endl;
		handlers.erase(ip);
		listener->onDisconnect(obj);
		obj = nullptr; // 如果没有行，智能指针会一直占用，该对象永远不会被释放
        return;
    }
	ws.text(ws.got_text());
	receiver->onData((uint8_t*)buffer.data().data(), bytes_transferred, obj);
    

    doRead();
}

void WebSockHandlerImpl::onWrite(boost::beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);
    if(ec) {
        std::cout<<"写入数据失败:"<<ec.message()<<std::endl;
		handlers.erase(ip);
    }
}


WebSockServerImplWithBeast::WebSockServerImplWithBeast(boost::asio::io_service &io_service)
: io(io_service), acceptor(io_service), listener(nullptr) {
	std::cout << "创建服务端" << std::endl;
}

WebSockServerImplWithBeast::~WebSockServerImplWithBeast()
{
	std::cout << "销毁服务端" << std::endl;
}


bool WebSockServerImplWithBeast::Open(int port) {
	boost::asio::ip::tcp::endpoint endpoint{ boost::asio::ip::address::from_string("0.0.0.0") , (uint16_t)port };
	boost::system::error_code ec;

	acceptor.open(endpoint.protocol(), ec);

	if (ec) {
		std::cout << "打开TCP监听失败:" << ec.message() << std::endl;
		return false;
	}
	acceptor.set_option(boost::asio::socket_base::reuse_address(true), ec);
	if (ec) {
		std::cout << "set_option失败:" << ec.message() << std::endl;
		return false;
	}
	acceptor.bind(endpoint, ec);
	if (ec) {
		std::cout << "bind失败:" << ec.message() << std::endl;
		return false;
	}
	acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
	if (ec) {
		std::cout << "listen失败:" << ec.message() << std::endl;
		return false;
	}

	doAccept();
    return true;
}

void WebSockServerImplWithBeast::RegisterHandlerListener(IWebSockServerHandlerListener* listener) {
	this->listener = listener;
}

bool WebSockServerImplWithBeast::Write(const std::string & key, uint8_t* data, int size)
{
	if (handlers.find(key) != handlers.end()) {
		return handlers[key]->Write(data, size);
	}
	return false;
}

void WebSockServerImplWithBeast::Close() {
	acceptor.close();
    for(const auto& v : handlers) {
		try {
			v.second->Close();
		}
		catch (boost::system::system_error& err) {

		}   
	}
}

void WebSockServerImplWithBeast::doAccept()
{
	acceptor.async_accept(boost::asio::make_strand(io),
		boost::beast::bind_front_handler(&WebSockServerImplWithBeast::onAccept, this));
}

void WebSockServerImplWithBeast::onAccept(boost::beast::error_code ec, boost::asio::ip::tcp::socket socket)
{
	if (ec) {
		socket.close();
		return;
	}
	std::string ip = (boost::format("%s:%d") % socket.remote_endpoint().address() % socket.remote_endpoint().port()).str();
	std::cout << "TCP客户端连接： " << ip << std::endl;
	handlers[ip] = std::make_shared<WebSockHandlerImpl>(std::move(socket), handlers, ip, listener);
	listener->onWebSockHandler(handlers[ip], ip);
	doAccept();
}
