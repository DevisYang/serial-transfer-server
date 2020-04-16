#include "web_sock.h"

#include "factory.h"

IWebSockServerSerialRunner* CreateWebSockServerFactory() {
	return new WebSockServerFactoryImplWithBoost();
}
