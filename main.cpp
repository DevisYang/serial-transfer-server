#include <iostream>
#include <thread>
#include <tuple> 
#include "TransferServer.h"


int main(int argc, char** argv) {
#ifdef WIN32
    system("chcp 65001");
#endif
	if (argc > 1) {
			TransferServer* server = new TransferServer();
			server->Start(atoi(argv[1]));
			while (true) {
				std::string cmd;
				std::cin >> cmd;
				if (cmd == "quit") {
					
					break;
				}
			}
			server->Stop();
			std::this_thread::sleep_for(std::chrono::seconds(1));
			delete server;
	}
}
