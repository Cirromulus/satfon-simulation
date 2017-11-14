/*
 * Copyright (c) 2016, 2017, German Aerospace Center (DLR)
 *
 * This file is part of the development version of OUTPOST.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Authors:
 * - 2016 - 2017, Pascal Pieper (DLR RY-AVS)
 */
// ----------------------------------------------------------------------------



#include "debugServer.hpp"
#include "types.hpp"
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>


DebugServer::DebugServer(unsigned int pageWidth, unsigned int startPort, unsigned int requestSize) :
	mPageWidth(pageWidth), mStartPort(startPort), mRequestSize(sizeof(functionRequest) + requestSize){
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
			//fprintf(stderr, "Bind failed with Port %d\n", mStartPort+i);
		}else{
			printf("Bind succeeded with Port %d\n", mStartPort+i);
			any = true;
			break;
		}
	}

	return any;
}

void DebugServer::serverListener(){
	char* buf = new char[mRequestSize];

	char* answerBuf = new char[mPageWidth * sizeof(AccessValues)];	//Has to fit the biggest type

	sockaddr remAddr;
	unsigned int remAddrSize = sizeof(remAddr);
	while (!stop) {
		int recvlen = recvfrom(serverSock, buf, mRequestSize, 0, &remAddr, &remAddrSize);
		if (recvlen > 0 && static_cast<unsigned>(recvlen) == mRequestSize) {
			functionRequest function;
			memcpy(&function, buf, sizeof(functionRequest));

			int answerSize = handleRequest(answerBuf, function, &buf[sizeof(enum functionRequest)]);
			if(sendto(serverSock, answerBuf, answerSize, 0, &remAddr, remAddrSize) < 0)
				fprintf(stderr, "Failed to send data.\n");

		}else{
			if(recvlen > 0)
				fprintf(stderr, "DebugServer: Misaligned read from socket.\n(Was %d, should %d)\n", recvlen, mRequestSize);
		}
	}
	delete[] buf;
	delete[] answerBuf;
	close(serverSock);
}

