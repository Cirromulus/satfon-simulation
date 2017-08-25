/*
 * debugInterfaceClient.hpp
 *
 *  Created on: 05.07.2016
 *      Author: urinator
 */

#ifndef SIMU_DEBUGINTERFACECLIENT_HPP_
#define SIMU_DEBUGINTERFACECLIENT_HPP_

#include "debugInterface.hpp"

#include <functional>
#include <atomic>
#include <thread>
#include <netinet/in.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>

class debugInterfaceClient {
	DATA_TYPE dataBuf[PAGE_SIZE];
	AccessValues avBuf[PAGE_SIZE];
	unsigned int radiationBuf[PAGE_SIZE];
	Failpoint fpBuf[PAGE_SIZE];
	bool bitFlippedBuf[PAGE_SIZE];
	DATA_TYPE latchBuf[PAGE_SIZE];
	FlashConfiguration config;

	int clientSock = 0;
	sockaddr_in rcpt;
	unsigned int rcptSize = sizeof(rcpt);
	static const unsigned int sendBufSize = sizeof(functionRequest)
			+ sizeof(unsigned int) * 3;
	char sendBuf[sendBufSize];bool initClient(int port);

	void sendRequest(functionRequest function, unsigned int plane,
			unsigned int block, unsigned int page);

	int recEverything(void* buf, unsigned int size);

public:
	debugInterfaceClient(int number) {
		if (!initClient(START_PORT + number))
			fprintf(stderr, "Could not init Clientsocket!\n");
		//default values
		config.pageSize = 528;
		config.blockSize = 64;
		config.planeSize = 4;
		config.cellSize = 4;
	}

	debugInterfaceClient() :
			debugInterfaceClient(0) {
	}

	bool isConnected();

	unsigned int getPageSize();

	unsigned int getBlockSize();

	unsigned int getPlaneSize();

	unsigned int getCellSize();

	DATA_TYPE* getValue(unsigned int planeAddress, unsigned int blockAddress,
			unsigned int pageAddress);

	AccessValues* getAccessValues(unsigned int planeAddress,
			unsigned int blockAddress, unsigned int pageAddress);

	unsigned int* getRadiationDose(unsigned int planeAddress,
			unsigned int blockAddress, unsigned int pageAddress);

	Failpoint* getFailpoint(unsigned int planeAddress,
			unsigned int blockAddress, unsigned int pageAddress);

	bool* wasBitFlipped(unsigned int planeAddress, unsigned int blockAddress,
			unsigned int pageAddress);

	DATA_TYPE* getLatchMask(unsigned int planeAddress,
			unsigned int blockAddress, unsigned int pageAddress);

};

#endif /* SIMU_DEBUGINTERFACECLIENT_HPP_ */
