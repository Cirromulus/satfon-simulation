/*
 * debugInterfaceClient.hpp
 *
 *  Created on: 05.07.2016
 *      Author: urinator
 */

#ifndef SIMU_MRAMDEBUGINTERFACECLIENT_HPP_
#define SIMU_MRAMDEBUGINTERFACECLIENT_HPP_

#include "debugClient.hpp"
#include "mram.hpp"

#include <vector>
#include <stdio.h>

class MramDebugInterfaceClient : private DebugClient{
	std::vector<unsigned char> dataBuf;
	MramConfiguration config;

public:
	MramDebugInterfaceClient(int number) :
			DebugClient(mramDebugServerStartPort + number, sizeof(unsigned int)){
		//default values
		config.blockWidth = 512;
		config.size = 4098;
		dataBuf.resize(config.blockWidth, 0);
	}

	MramDebugInterfaceClient() :
		MramDebugInterfaceClient(0) {
	}

	void sendFlashRequest(functionRequest function, unsigned int address);

	bool isConnected();

	unsigned int getBlockWidth();

	unsigned int getSize();

	unsigned char* getValue(unsigned int address);

};

#endif /* SIMU_FLASHDEBUGINTERFACECLIENT_HPP_ */
