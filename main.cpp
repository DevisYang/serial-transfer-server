#include <iostream>
#include "TransferServer.h"

int main(int, char**) {
#ifdef WIN32
    system("chcp 65001");
#endif
	TransferServer server;
	server.Start(9527);
	server.Stop();
}
