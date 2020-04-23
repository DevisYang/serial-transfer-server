#include "factory.h"

IWrapperFactory* CreateWebSockServerFactory() {
	return new BoostWrapperFactoryImpl(4);
}
