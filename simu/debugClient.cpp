/*
 * DebugClient.cpp
 *
 *  Created on: 10.09.2017
 *      Author: urinator
 */

#include "debugClient.hpp"
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>

DebugClient::DebugClient(unsigned int port, unsigned int sendBufSize) :
	rcptSize(sizeof(rcpt)), mSendBufSize(sendBufSize){
	if(!initClient(port)){
		printf("DebugClient: Could not bind to port %u\n", port);
	}
	sendBuf = new char[sendBufSize];
}

DebugClient::~DebugClient(){
	delete[] sendBuf;
}

bool DebugClient::initClient(unsigned int port){
	if((clientSock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		fprintf(stderr, "could not create Socket\n");
		return false;
	}

	rcpt.sin_family = AF_INET;
	rcpt.sin_port = htons(port);

	if (inet_aton("127.0.0.1" , &rcpt.sin_addr) == 0){
		fprintf(stderr, "inet_aton() failed\n");
		return false;
	}

	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 10000;
	if (setsockopt(clientSock, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
		fprintf(stderr, "setSockopt failed\n");
		return false;
	}
	printf("Bind succeeded with Port %d\n", port);
	return true;

}

void DebugClient::sendRequest(functionRequest function, char* params){
	sendBuf[0] = function;
	memcpy(&sendBuf[sizeof(functionRequest)], params, mSendBufSize);
	if (sendto(clientSock, sendBuf, mSendBufSize + sizeof(functionRequest), 0 ,
			reinterpret_cast<struct sockaddr*> (&rcpt), rcptSize)<0){
		fprintf(stderr, "could not send request.\n");
	}
}

int DebugClient::recEverything(void* buf, unsigned int size){
	unsigned int receivedBytes = 0;
	while(receivedBytes < size){
		int tmpReceivedBytes = 0;
		if((tmpReceivedBytes = recvfrom(clientSock,
				&reinterpret_cast<char*>(buf)[receivedBytes],
				size - receivedBytes, 0,
				reinterpret_cast<struct sockaddr*> (&rcpt), &rcptSize)) < 0)
		{
			fprintf(stderr, "Error while receiving (Rec: %d, %s)\n", tmpReceivedBytes, strerror(errno));
			return -1;
		}
		if(tmpReceivedBytes == 0){
			break;
		}
		receivedBytes += tmpReceivedBytes;
	}

	return receivedBytes;
}
