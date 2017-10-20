/*
 * DebugClient.hpp
 *
 *  Created on: 10.09.2017
 *      Author: urinator
 */

#pragma once

#include "debugServer.hpp"

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

class DebugClient{
	int clientSock = 0;
	sockaddr_in rcpt;
	unsigned int rcptSize;
	unsigned int mSendBufSize;
	char* sendBuf;
public:
	DebugClient(unsigned int port, unsigned int sendBufSize);
	virtual ~DebugClient();
	bool initClient(unsigned int port);
	void sendRequest(functionRequest function, char* params);
	int recEverything(void* buf, unsigned int size);
};
