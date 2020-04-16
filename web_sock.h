#pragma once
#include <cstdint>

// 数据接收器
class IDataReceiver {
public:
	virtual void onData(uint8_t* data, int size, void* obj) = 0;
};

// WebSocket客户端连接管理
class IWebSockHandler {
public:
	virtual void SetDataReceiver(IDataReceiver* receiver, void* obj) = 0;
	virtual bool Write(uint8_t* data, int size) = 0;
	virtual void Close() = 0;
	virtual void Release() = 0;
};

// websocket客户端连接监听器
class IWebSockServerHandlerListener {
public:
	virtual void onWebSockHandler(IWebSockHandler* handler) = 0;
};

class IWebSockServer
{
public:
	virtual bool Open(int port) = 0;
	virtual void Close() = 0;
};

// 串口客户端
class ISerialHandler : public IWebSockHandler {
public:
	virtual void Open(const char* port, int baud) = 0;
};

class IWebSockServerSerialRunner {
public:
	virtual IWebSockServer* CreateWebSockServer() = 0;
    virtual ISerialHandler* CreateSerialHandler() = 0;
    virtual void Run() = 0;
    virtual void Release() = 0;
};

IWebSockServerSerialRunner* CreateWebSockServerFactory();

