//
// Created by cgutech on 2020/4/16.
//

#include "TransferServer.h"
#include <iostream>
#include <thread>
#include <rapidjson/document.h>
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "hex.h"

TransferServer::TransferServer()
{
    pFactroy = nullptr;
    server = nullptr;
	std::cout << "创建TransferServer" << std::endl;
}

TransferServer::~TransferServer() {
	std::cout << "销毁TransferServer" << std::endl;
}

bool TransferServer::Start(int net_port) {
	if (server != nullptr) {
		return false;
	}
	pFactroy = CreateWebSockServerFactory();
	server = pFactroy->CreateWebSockServer();
	server->RegisterHandlerListener(this);
	server->Open(net_port);
	pFactroy->Run();

    return true;
}

void TransferServer::Stop() {
	std::cout << "server cout=" << server.use_count() << std::endl;
	server->Close();
	server = nullptr;
	pFactroy->Release();
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
		}
		else if (type == "enum") { // 处理枚举串口事件
			res.AddMember("type", "enum", allocator);
			onEnum(res, allocator);
		}
		else if (serials.find(handler->GetKey()) == serials.end()) {
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

// websocket连接断开 
void TransferServer::onDisconnect(std::shared_ptr<IWebSockHandler> handler)
{
	if (serials.find(handler->GetKey()) != serials.end()) {
		std::cout << "websocket客户端连接断开";
		auto serial = serials[handler->GetKey()];
		serial->Close();
		websocks.erase(serial->GetKey());
		serials.erase(handler->GetKey());
	}
}

void TransferServer::onOpen(rapidjson::Document&doc, std::shared_ptr<IWebSockHandler>& handler, rapidjson::Value& res, rapidjson::Document::AllocatorType& allocator)
{
	if (serials.find(handler->GetKey()) != serials.end()) {
		res.AddMember("success", rapidjson::Value(false), allocator);
		res.AddMember("message", rapidjson::Value("一个连接只能打开一个串口"), allocator);
		return;
	}

	if (!doc.HasMember("serial_name") || !doc.HasMember("serial_baud")) { // 如果没有串口号和波特率，返回错误
		res.AddMember("success", rapidjson::Value(false), allocator);
		res.AddMember("message", rapidjson::Value("参数不足"), allocator);
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
	websocks.erase(serial->GetKey());
	serials.erase(handler->GetKey());

	res.AddMember("success", rapidjson::Value(true), allocator);
	res.AddMember("message", rapidjson::Value("关闭串口成功"), allocator);
}

void TransferServer::onEnum(rapidjson::Value & res, rapidjson::Document::AllocatorType & allocator)
{
#ifdef WIN32
	rapidjson::Value arr(rapidjson::Type::kArrayType);
	auto serials = EnumSerial();
	for (auto serial : serials) {
		rapidjson::Value value_(rapidjson::kStringType);
		value_.SetString(serial.c_str(), serial.size(), allocator);
		arr.PushBack(value_, allocator);
	}
	res.AddMember("serials", arr, allocator);
	res.AddMember("success", true, allocator);
	res.AddMember("message", "查询串口成功", allocator);
#else
	res.AddMember("success", rapidjson::Value(false), allocator);
	res.AddMember("message", rapidjson::Value("不支持WINDOWS之外的系统的串口枚举"), allocator);
#endif
}

void TransferServer::addMember(const char* key, std::string value, rapidjson::Value& res, rapidjson::Document::AllocatorType& allocator) {
    rapidjson::Value key_(rapidjson::kStringType);
    rapidjson::Value value_(rapidjson::kStringType);
    key_.SetString(key, allocator);
    value_.SetString(value.c_str(), value.size(), allocator);
    res.AddMember(key_, value_, allocator);
}

#ifdef WIN32

#include <windows.h>
#include <setupapi.h>
#include <devguid.h>
#include <tchar.h>
const int port_name_max_length = 128;
const int friendly_name_max_length = 256;
const int hardware_id_max_length = 256;

// Convert a wide Unicode string to an UTF8 string
std::string utf8_encode(const std::wstring &wstr)
{
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo( size_needed, 0 );
    WideCharToMultiByte                  (CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

std::vector<std::string> TransferServer::EnumSerial() {
    std::vector<std::string> devices_found;

    HDEVINFO device_info_set = SetupDiGetClassDevs(
            (const GUID *) &GUID_DEVCLASS_PORTS,
            NULL,
            NULL,
            DIGCF_PRESENT);

    unsigned int device_info_set_index = 0;
    SP_DEVINFO_DATA device_info_data;

    device_info_data.cbSize = sizeof(SP_DEVINFO_DATA);

    while(SetupDiEnumDeviceInfo(device_info_set, device_info_set_index, &device_info_data))
    {
        device_info_set_index++;
        // Get port name
        HKEY hkey = SetupDiOpenDevRegKey(
                device_info_set,
                &device_info_data,
                DICS_FLAG_GLOBAL,
                0,
                DIREG_DEV,
                KEY_READ);

        wchar_t port_name[port_name_max_length];
        DWORD port_name_length = port_name_max_length;

        LONG return_code = RegQueryValueExW(
                hkey,
                L"PortName",
                NULL,
                NULL,
                (LPBYTE)port_name,
                &port_name_length);

        RegCloseKey(hkey);

        if(return_code != EXIT_SUCCESS)
            continue;

        if(port_name_length > 0 && port_name_length <= port_name_max_length)
            port_name[port_name_length-1] = '\0';
        else
            port_name[0] = '\0';

        // Ignore parallel ports

        if(wcsstr(port_name, L"LPT") != NULL)
            continue;

        // Get port friendly name

        TCHAR friendly_name[friendly_name_max_length];
        DWORD friendly_name_actual_length = 0;

        BOOL got_friendly_name = SetupDiGetDeviceRegistryProperty(
                device_info_set,
                &device_info_data,
                SPDRP_FRIENDLYNAME,
                NULL,
                (PBYTE)friendly_name,
                friendly_name_max_length,
                &friendly_name_actual_length);

        if(got_friendly_name == TRUE && friendly_name_actual_length > 0)
            friendly_name[friendly_name_actual_length-1] = '\0';
        else
            friendly_name[0] = '\0';

        // Get hardware ID

        TCHAR hardware_id[hardware_id_max_length];
        DWORD hardware_id_actual_length = 0;

        BOOL got_hardware_id = SetupDiGetDeviceRegistryProperty(
                device_info_set,
                &device_info_data,
                SPDRP_HARDWAREID,
                NULL,
                (PBYTE)hardware_id,
                hardware_id_max_length,
                &hardware_id_actual_length);

        if(got_hardware_id == TRUE && hardware_id_actual_length > 0)
            hardware_id[hardware_id_actual_length-1] = '\0';
        else
            hardware_id[0] = '\0';

        std::string portName = utf8_encode(port_name);
        devices_found.push_back(portName);
    }

    SetupDiDestroyDeviceInfoList(device_info_set);
	return devices_found;
}
#else
std::vector<std::string> TransferServer::EnumSerial() {
	return std::vector<std::string>();
}
#endif

