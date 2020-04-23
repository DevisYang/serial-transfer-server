//
// Created by cgutech on 2020/4/16.
//

#include "TransferServer.h"
#include <iostream>
#include <rapidjson/document.h>
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "hex.h"

TransferServer::TransferServer()
{
    pFactroy = CreateWebSockServerFactory();
    server = nullptr;
}

TransferServer::~TransferServer() {
    pFactroy->Release();
}

bool TransferServer::Start(int net_port) {
	if (server != nullptr) {
		return false;
	}

	server = pFactroy->CreateWebSockServer();
	server->RegisterHandlerListener(this);
	server->Open(net_port);
	pFactroy->Run();

    return true;
}

void TransferServer::Stop() {
	server->Close();
}

void TransferServer::onData(uint8_t* data, int size, std::shared_ptr<IWebSockHandler>& handler) {
	char* str = new char[size + 1];
	memcpy(str, data, size);
	str[size] = '\0';
	// websocket协议本身有封装，不需要处理TCP中的粘包问题
	// 使用rapidjson解析json文档
	rapidjson::Document doc;
    rapidjson::Document document;
    rapidjson::Document::AllocatorType& allocator=document.GetAllocator();

	rapidjson::Value res(rapidjson::kObjectType);
	doc.Parse(str);
	std::cout << "收到websocket数据：" << str << std::endl;
	delete[] str;
	
	if (doc.HasParseError() || !doc.HasMember("type")) { // 如果不符合JSON格式或是没有type字段，则不处理
		res.AddMember("success", false, allocator);
		res.AddMember("message", "协议解析错误或是不符合协议规范", allocator);
	}
	else {
		std::string type = doc["type"].GetString();
		if (type == "open") { // 处理打开串口操作
			res.AddMember("type", "open", allocator);
			onOpen(doc, handler, res, allocator);
		} else if (serials.find(handler->GetKey()) == serials.end()) {
			// 如果串口对象不存在，不处理
			res.AddMember("success", false, allocator);
			res.AddMember("message", "串口已关闭", allocator);
		}
		else if (type == "close") { // 处理关闭事件 
			res.AddMember("type", "close", allocator);
			onClose(handler, res, allocator);
		} 
		else if (type == "send" && doc.HasMember("data")) { // 处理写入数据事件
			res.AddMember("type", "data", allocator);
			onSend(doc["data"].GetString(), handler, res, allocator);
		}
		else if (type == "enum"){ // 处理枚举串口事件
			res.AddMember("type", "enum", allocator);
		}
		else {
			this->addMember("type", type, res, allocator);
			res.AddMember("success", false, allocator);
			res.AddMember("message", "未识别的操作", allocator);
		}
	}
	
	rapidjson::StringBuffer  buff;
	rapidjson::Writer<rapidjson::StringBuffer>  writer(buff);
	res.Accept(writer);
	auto s = buff.GetString();
	handler->Write((uint8_t*)s, buff.GetSize());
}

void TransferServer::onData(uint8_t* data, int size, std::shared_ptr<ISerialHandler>& handler) {
	if (websocks.find(handler->GetKey()) == websocks.end()) {
		// 串口未绑定，关闭串口
		handler->Close();
		return;
	}
	auto websock = websocks[handler->GetKey()];
	auto hex = BytesToHexString(data, size);
	rapidjson::Document document;
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
	rapidjson::Value res(rapidjson::kObjectType);
	res.AddMember("type", "data", allocator);
	addMember("data", hex, res, allocator);
	rapidjson::StringBuffer  buff;
	rapidjson::Writer<rapidjson::StringBuffer>  writer(buff);
	res.Accept(writer);
	auto s = buff.GetString();
	websock->Write((uint8_t*)s, buff.GetSize());
}

void TransferServer::onWebSockHandler(std::shared_ptr<IWebSockHandler> handler, const std::string& key) {
	handler->SetDataReceiver(this, handler);
}

void TransferServer::onOpen(rapidjson::Document&doc, std::shared_ptr<IWebSockHandler>& handler, rapidjson::Value& res, rapidjson::Document::AllocatorType& allocator)
{
	if (!doc.HasMember("serial_name") || !doc.HasMember("serial_baud")) { // 如果没有串口号和波特率，返回错误
		res.AddMember("success", rapidjson::Value(false), allocator);
		res.AddMember("message", rapidjson::Value("没有串名或没有波特率"), allocator);
	}
	else {
		auto serial = pFactroy->CreateSerialHandler();
		serial->SetDataReceiver(this, serial);
		const char* name = doc["serial_name"].GetString();
		if (!serial->Open(name, doc["serial_baud"].GetInt())) {
			res.AddMember("success", rapidjson::Value(false), allocator);
			res.AddMember("message", rapidjson::Value("打开串口失败"), allocator);
		}
		else {
			// 绑定串口Handker和websocket Handler
			websocks[name] = handler;
			serials[handler->GetKey()] = serial;
			res.AddMember("success", rapidjson::Value(), allocator);
			res.AddMember("message", rapidjson::Value("打开串口成功"), allocator);
		}
	}
}

void TransferServer::onSend(std::string data, std::shared_ptr<IWebSockHandler>& handler, rapidjson::Value& res, rapidjson::Document::AllocatorType& allocator) {
	auto buffer = HexStringToBytes(data);
	auto serial = serials[handler->GetKey()];
	if (serial->Write(buffer.data, buffer.size)) {
		res.AddMember("success", rapidjson::Value(true), allocator);
		res.AddMember("message", rapidjson::Value("写入串口数据成功"), allocator);
	}
	else {
		res.AddMember("success", rapidjson::Value(false), allocator);
		res.AddMember("message", rapidjson::Value("写入串口数据失败"), allocator);
	}
}

void TransferServer::onClose(std::shared_ptr<IWebSockHandler>& handler, rapidjson::Value& res, rapidjson::Document::AllocatorType& allocator)
{
	if (serials.find(handler->GetKey()) == serials.end()) {
		res.AddMember("success", rapidjson::Value(false), allocator);
		res.AddMember("message", rapidjson::Value("串口不存在"), allocator);
		return;
	}

	auto serial = serials[handler->GetKey()];
	serial->Close();
	std::cout << "count=" << serial.use_count() << std::endl;
	websocks.erase(serial->GetKey());
	serials.erase(handler->GetKey());
	std::cout << "count=" << serial.use_count() << std::endl;
}

void TransferServer::addMember(const char* key, std::string value, rapidjson::Value& res, rapidjson::Document::AllocatorType& allocator) {
    rapidjson::Value key_(rapidjson::kStringType);
    rapidjson::Value value_(rapidjson::kStringType);
    key_.SetString(key, allocator);
    value_.SetString(value.c_str(), value.size(), allocator);
    res.AddMember(key_, value_, allocator);
}
