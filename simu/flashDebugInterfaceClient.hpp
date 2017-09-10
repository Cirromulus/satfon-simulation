/*
 * debugInterfaceClient.hpp
 *
 *  Created on: 05.07.2016
 *      Author: urinator
 */

#ifndef SIMU_FLASHDEBUGINTERFACECLIENT_HPP_
#define SIMU_FLASHDEBUGINTERFACECLIENT_HPP_

#include <stdio.h>

#include "debugClient.hpp"
#include "flashDebugInterface.hpp"

class FlashDebugInterfaceClient : private DebugClient{
	DATA_TYPE dataBuf[PAGE_SIZE];
	AccessValues avBuf[PAGE_SIZE];
	unsigned int radiationBuf[PAGE_SIZE];
	Failpoint fpBuf[PAGE_SIZE];
	bool bitFlippedBuf[PAGE_SIZE];
	DATA_TYPE latchBuf[PAGE_SIZE];
	FlashConfiguration config;

public:
	FlashDebugInterfaceClient(int number) :
			DebugClient(flashDebugServerStartPort + number, 3 * sizeof(unsigned int)){
		//default values
		config.pageSize = 528;
		config.blockSize = 64;
		config.planeSize = 4;
		config.cellSize = 4;
	}

	FlashDebugInterfaceClient() :
			FlashDebugInterfaceClient(0) {
	}

	void sendFlashRequest(functionRequest function, unsigned int planeAddress,
			unsigned int blockAddress, unsigned int pageAddress);

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

#endif /* SIMU_FLASHDEBUGINTERFACECLIENT_HPP_ */
