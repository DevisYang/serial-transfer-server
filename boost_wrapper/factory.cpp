//
// Created by cgutech on 2020/4/16.
//

#include "factory.h"
#include "WebSockServerImplWithBeast.h"
#include "SerialHandlerImpl.h"
BoostWrapperFactoryImpl::BoostWrapperFactoryImpl(int ths)
	:threads(ths), io(ths)  {

}

std::unique_ptr<IWebSockServer> BoostWrapperFactoryImpl::CreateWebSockServer() {
	return std::unique_ptr<IWebSockServer>(new WebSockServerImplWithBeast(io));
}

std::shared_ptr<ISerialHandler> BoostWrapperFactoryImpl::CreateSerialHandler() {
	return std::shared_ptr<ISerialHandler>(new SerialHandlerImpl(io));
}

void BoostWrapperFactoryImpl::Run() {
	std::vector<std::thread> v;
	v.reserve(threads - 1);

	for (auto i = threads - 1; i > 0; --i) {
		std::thread th(workThread, this);
		v.emplace_back(std::move(th));
	}
	io.run();
}

void BoostWrapperFactoryImpl::Release() {
	delete this;
}

void BoostWrapperFactoryImpl::workThread(void* obj) {
    static_cast<BoostWrapperFactoryImpl*>(obj)->io.run();
}
