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


#ifndef SIMU_MRAMDEBUGINTERFACECLIENT_HPP_
#define SIMU_MRAMDEBUGINTERFACECLIENT_HPP_

#include "debugClient.hpp"
#include "mram.hpp"

#include <vector>
#include <ostream>

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
