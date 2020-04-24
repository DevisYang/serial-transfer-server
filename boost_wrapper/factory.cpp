//
// Created by cgutech on 2020/4/16.
//

#include "factory.h"
#include <iostream>
#include "WebSockServerImplWithBeast.h"
#include "SerialHandlerImpl.h"
BoostWrapperFactoryImpl::BoostWrapperFactoryImpl(int ths)
	:threads(ths), io(ths)  {

}

std::shared_ptr<IWebSockServer> BoostWrapperFactoryImpl::CreateWebSockServer() {
	return std::shared_ptr<WebSockServerImplWithBeast>(new WebSockServerImplWithBeast(io));
}

std::shared_ptr<ISerialHandler> BoostWrapperFactoryImpl::CreateSerialHandler() {
	return std::shared_ptr<SerialHandlerImpl>(new SerialHandlerImpl(io));
}

void BoostWrapperFactoryImpl::Run() {
	for (auto i = threads; i > 0; i--) {
		vec.push_back(std::thread(workThread, this));
	}
}

void BoostWrapperFactoryImpl::Release() {
	std::cout << "销毁工厂对象" << std::endl;
	for (auto i = threads; i > 0; i--) {
		io.stop();
	}
	vec.clear();
}

void BoostWrapperFactoryImpl::workThread(void* obj) {
    static_cast<BoostWrapperFactoryImpl*>(obj)->io.run();
}
