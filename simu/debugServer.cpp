/*
 * debugServer.cpp
 *
 *  Created on: 10.09.2017
 *      Author: urinator
 */


#include "debugServer.hpp"
#include "types.hpp"
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>


DebugServer::DebugServer(unsigned int pageWidth, unsigned int startPort, unsigned int requestSize) :
	mPageWidth(pageWidth), mStartPort(startPort), mRequestSize(requestSize){
	stop = false;
	if(!createServer()){
		fprintf(stderr, "DebugServer %s: Could not create Server!", typeid(this).name());
	}else{
		serverListenerThread = std::thread(&DebugServer::serverListener, this);
	}
}

DebugServer::~DebugServer(){
	stop = true;
	shutdown(serverSock, SHUT_RDWR);
	//while(!serverListenerThread.joinable()){}
	serverListenerThread.join();
}

bool DebugServer::createServer(){

	if ((serverSock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		fprintf(stderr, "cannot create socket");
		return false;
	}

	struct sockaddr_in addr;
	bool any = false;

	for(unsigned int i = 0; i < maxInstancesListening; i++){
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		addr.sin_port = htons(mStartPort+i);

		if (bind(serverSock, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
			fprintf(stderr, "Bind failed with Port %d\n", mStartPort+i);
		}else{
			//printf("Bind succeeded with Port %d\n", START_PORT+i);
			any = true;
			break;
		}
	}

	return any;
}

void DebugServer::serverListener(){

	int bufsize = sizeof(enum functionRequest) + mRequestSize;
	char* buf = new char[bufsize];

	char* answerBuf = new char[mPageWidth * sizeof(AccessValues)];	//Has to fit the biggest type

	sockaddr remAddr;
	unsigned int remAddrSize = sizeof(remAddr);
	while (!stop) {
		int recvlen = recvfrom(serverSock, buf, bufsize, 0, &remAddr, &remAddrSize);
		if (recvlen == bufsize) {
			functionRequest function = static_cast<functionRequest> (buf[0]);

			int answerSize = handleRequest(answerBuf, function, &buf[sizeof(enum functionRequest)]);
			if(sendto(serverSock, answerBuf, answerSize, 0, &remAddr, remAddrSize) < 0)
				fprintf(stderr, "Failed to send data.\n");

		}else{
			if(recvlen > 0)
				fprintf(stderr, "DebugServer: Misaligned read from socket.\n(Was %d, should %d)\n", recvlen, bufsize);
		}
	}
	delete[] buf;
	delete[] answerBuf;
	close(serverSock);
}

