//
// Created by cgutech on 2020/4/16.
//

#include "WebSockServerImplWithBeast.h"
WebSockHandlerImplWithBeast::WebSockHandlerImplWithBeast(boost::asio::ip::tcp::socket&& socket)
: ws(std::move(socket)), receiver(nullptr), obj(nullptr){

}

void WebSockHandlerImplWithBeast::SetDataReceiver(IDataReceiver *r, void *pVoid) {
    this->receiver = r;
    this->obj = pVoid;
}

bool WebSockHandlerImplWithBeast::Write(uint8_t *data, int size) {
    ws.async_write(
            boost::asio::buffer(data, size),
            boost::beast::bind_front_handler(&WebSockHandlerImplWithBeast::onWrite, shared_from_this()));
    return true;
}

void WebSockHandlerImplWithBeast::Close() {

}

void WebSockHandlerImplWithBeast::Release() {

}

void WebSockHandlerImplWithBeast::run() {
    boost::asio::dispatch(ws.get_executor(), boost::beast::bind_front_handler(&WebSockHandlerImplWithBeast::onRun, shared_from_this()));
}
void WebSockHandlerImplWithBeast::onRun() {
    ws.set_option(boost::beast::websocket::stream_base::decorator([](boost::beast::websocket::response_type& res){
        res.set(boost::beast::http::field::server, std::string(BOOST_BEAST_VERSION_STRING) + "websocket-server-async");
    }));
    ws.async_accept(boost::beast::bind_front_handler(&WebSockHandlerImplWithBeast::onAccept, shared_from_this()));
}

void WebSockHandlerImplWithBeast::onAccept(boost::beast::error_code ec) {
    if(ec) {
        return;
    }
    doRead();
}
void WebSockHandlerImplWithBeast::doRead() {
    ws.async_read(buffer, boost::beast::bind_front_handler(
            &WebSockHandlerImplWithBeast::onRead, shared_from_this()));
}

void WebSockHandlerImplWithBeast::onRead(boost::beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);
    if(ec) {
        return;
    }
    ws.got_binary();

    doRead();
}

void WebSockHandlerImplWithBeast::onWrite(boost::beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);
}



WebSockServerImplWithBeast::WebSockServerImplWithBeast(const boost::asio::io_service &io)
: io(io){

}

bool WebSockServerImplWithBeast::Open(int port) {
    return false;
}

void WebSockServerImplWithBeast::Close() {

}

