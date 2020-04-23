#include <iostream>
#include<memory>
#include <windows.h>
#include "TransferServer.h"

int main(int, char**) {
    system("chcp 65001");
	TransferServer server;
	server.Start(9527);
	server.Stop();
}
