/*
 * debugInterfaceClient.cpp
 *
 *  Created on: 05.07.2016
 *      Author: urinator
 */


#include <arpa/inet.h>
#include <debugInterfaceClient.hpp>

bool debugInterfaceClient::initClient(int port){
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

void debugInterfaceClient::sendRequest(functionRequest function, unsigned int plane, unsigned int block, unsigned int page){

	sendBuf[0] = function;
	sendBuf[sizeof(functionRequest)] = plane;
	sendBuf[sizeof(functionRequest) + sizeof(unsigned int)] = block;
	sendBuf[sizeof(functionRequest) + sizeof(unsigned int) * 2] = page;

	if (sendto(clientSock, sendBuf, sendBufSize, 0 , (sockaddr*)&rcpt, rcptSize)<0){
		fprintf(stderr, "could not send request.\n");
	}
}

int debugInterfaceClient::recEverything(void* buf, unsigned int size){
	unsigned int receivedBytes = 0;
	while(receivedBytes < size){
		int tmpReceivedBytes = 0;
		if((tmpReceivedBytes = recvfrom(clientSock, buf, size, 0, (sockaddr*) &rcpt, &rcptSize)) < 0){
			//fprintf(stderr, "Error while receiving\n");
			return -1;
		}
		receivedBytes += tmpReceivedBytes;
		if(tmpReceivedBytes == 0){
			break;
		}
	}

	return receivedBytes;
}

bool debugInterfaceClient::isConnected(){
	sendRequest(F_PING,0,0,0);
	char b;
	return recEverything(&b, 1) > 0;
}

DATA_TYPE* debugInterfaceClient::getValue(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress){
	sendRequest(F_GETVALUE, planeAddress, blockAddress, pageAddress);
	if(recEverything(dataBuf, PAGE_SIZE* sizeof(DATA_TYPE)) > 0){
		return dataBuf;
	}
	return NULL;
}

AccessValues* debugInterfaceClient::getAccessValues(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress){
	sendRequest(F_GETACCESSVALUES, planeAddress, blockAddress, pageAddress);
	if(recEverything(avBuf, PAGE_SIZE* sizeof(AccessValues)) > 0){
		return avBuf;
	}
	return NULL;
}

unsigned int* debugInterfaceClient::getRadiationDose(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress){
	sendRequest(F_GETRADIATIONDOSE, planeAddress, blockAddress, pageAddress);
	if(recEverything(radiationBuf, PAGE_SIZE* sizeof(unsigned int)) > 0){
		return radiationBuf;
	}
	return NULL;
}

Failpoint* debugInterfaceClient::getFailpoint(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress){
	sendRequest(F_GETFAILPOINT, planeAddress, blockAddress, pageAddress);
	if(recEverything(fpBuf, PAGE_SIZE* sizeof(Failpoint)) > 0){
		return fpBuf;
	}
	return NULL;
}

bool* debugInterfaceClient::wasBitFlipped(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress){
	sendRequest(F_WASBITFLIPPED, planeAddress, blockAddress, pageAddress);
	if(recEverything(bitFlippedBuf, PAGE_SIZE* sizeof(bool)) > 0){
		return bitFlippedBuf;
	}
	return NULL;
}

DATA_TYPE* debugInterfaceClient::getLatchMask(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress){
	sendRequest(F_GETLATCHMASK, planeAddress, blockAddress, pageAddress);
	if(recEverything(latchBuf, PAGE_SIZE* sizeof(DATA_TYPE)) > 0){
		return latchBuf;
	}
	return NULL;
}
