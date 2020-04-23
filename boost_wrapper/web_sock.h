#ifndef WRAPPER_DLL_H
#define WRAPPER_DLL_H


#ifdef MSVC
#ifdef DLL_WRAPPER_API
#define DLL_WRAPPER_EXPORT_API __declspec(dllexport)
#else
#define DLL_WRAPPER_EXPORT_API __declspec(dllimport)
#endif
#else
#define DLL_WRAPPER_EXPORT_API
#endif

#include <cstdint>
#include <memory>
#include <string>
template<class T>
class IDataReceiver {
public:
	virtual void onData(uint8_t* data, int size, std::shared_ptr<T>& obj) = 0;
};


class IWebSockHandler {
public:
	// 这个用法耦合比较深，因为限制了obj的类型，只适用于串口转发项目
	virtual void SetDataReceiver(IDataReceiver<IWebSockHandler>* receiver, std::shared_ptr<IWebSockHandler>& obj) = 0;
	virtual bool Write(uint8_t* data, int size) = 0;
	virtual void Close() = 0;
	virtual const std::string& GetKey() = 0;
};

class IWebSockServerHandlerListener {
public:
	virtual void onWebSockHandler(std::shared_ptr<IWebSockHandler> handler, const std::string& key) = 0;
};

class IWebSockServer
{
public:
	virtual bool Open(int port) = 0;
	virtual void RegisterHandlerListener(IWebSockServerHandlerListener* listener) = 0;
	virtual bool Write(const std::string& key, uint8_t* data, int size) = 0;
	virtual void Close() = 0;
};

class ISerialHandler {
public:
	virtual bool Open(const char* port, int baud) = 0;
	// 这个用法耦合比较深，因为限制了obj的类型，只适用于串口转发项目
	virtual void SetDataReceiver(IDataReceiver<ISerialHandler>* receiver, std::shared_ptr<ISerialHandler>& obj) = 0;
	virtual bool Write(uint8_t* data, int size) = 0;
	virtual void Close() = 0;
	virtual const std::string& GetKey() = 0;
};

#include<memory>
class IWrapperFactory {
public:
	virtual std::unique_ptr<IWebSockServer> CreateWebSockServer() = 0;
    virtual std::shared_ptr<ISerialHandler> CreateSerialHandler() = 0;
    virtual void Run() = 0;
};

extern "C" DLL_WRAPPER_EXPORT_API IWrapperFactory* CreateWebSockServerFactory();

#endif