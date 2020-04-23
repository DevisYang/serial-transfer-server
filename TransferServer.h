//
// Created by cgutech on 2020/4/16.
//

#ifndef SERIAL_TRANSFER_SERVER_TRANSFERSERVER_H
#define SERIAL_TRANSFER_SERVER_TRANSFERSERVER_H
#include "boost_wrapper/web_sock.h"
#include <map>
#include <rapidjson/document.h>
struct ConnData {
public:
	enum ConnType { SERIAL_DATA, WEB_SOCK_DATA };
	ConnType type;
	std::shared_ptr<IWebSockHandler> websock;
	std::shared_ptr<ISerialHandler> serial;
};

class TransferServer : public IDataReceiver<IWebSockHandler>, public IDataReceiver<ISerialHandler>, public IWebSockServerHandlerListener
{
public:
	TransferServer();
	bool Start(int net_port);
	void Stop();
private:
	virtual void onData(uint8_t* data, int size, std::shared_ptr<IWebSockHandler>& handler);
	virtual void onData(uint8_t* data, int size, std::shared_ptr<ISerialHandler>& handler);
	virtual void onWebSockHandler(std::shared_ptr<IWebSockHandler> handler, const std::string& key);
private:
	IWrapperFactory* pFactroy;
	std::unique_ptr<IWebSockServer> server;
private:
	std::map<std::string, std::shared_ptr<IWebSockHandler>> websocks; // 串口与websocket会话的绑定关系
	std::map<std::string, std::shared_ptr<ISerialHandler>> serials; // websocket会话与串口的绑定关系

private:
	void onOpen(rapidjson::Document&doc, std::shared_ptr<IWebSockHandler>& handler, rapidjson::Value& res, rapidjson::Document::AllocatorType& allocator);
	void onSend(std::string data, std::shared_ptr<IWebSockHandler>& handler, rapidjson::Value& res, rapidjson::Document::AllocatorType& allocator);
	void onClose(std::shared_ptr<IWebSockHandler>& handler, rapidjson::Value& res, rapidjson::Document::AllocatorType& allocator);
	void addMember(const char* key, std::string value, rapidjson::Value& res,  rapidjson::Document::AllocatorType& allocator);
};

#endif //SERIAL_TRANSFER_SERVER_TRANSFERSERVER_H
