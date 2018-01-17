/*
 * Copyright (c) 2017, German Aerospace Center (DLR)
 *
 * This file is part of the development version of OUTPOST.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Authors:
 * - 2017, Pascal Pieper (DLR RY-AVS)
 */
// ----------------------------------------------------------------------------

#ifndef SIMU_FLASHDEBUGINTERFACECLIENT_HPP_
#define SIMU_FLASHDEBUGINTERFACECLIENT_HPP_

#include <stdio.h>

#include "debugClient.hpp"
#include "flashDebugInterface.hpp"

using namespace simu;

class FlashDebugInterfaceClient : private DebugClient{
	FlashByte *dataBuf;
	AccessValues *avBuf;
	unsigned int *radiationBuf;
	Failpoint *fpBuf;
	bool *bitFlippedBuf;
	FlashByte *latchBuf;
	FlashConfiguration mConfig;

public:
	FlashDebugInterfaceClient(int number) :
			DebugClient(flashDebugServerStartPort + number, 3 * sizeof(unsigned int)),
			dataBuf(nullptr), avBuf(nullptr), radiationBuf(nullptr), fpBuf(nullptr),
			bitFlippedBuf(nullptr), latchBuf(nullptr){
		//default values
		mConfig.pageSize = 528;
		mConfig.blockSize = 64;
		mConfig.planeSize = 4;
		mConfig.cellSize = 4;
		resizeBuffers();
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

	FlashByte* getValue(unsigned int planeAddress, unsigned int blockAddress,
			unsigned int pageAddress);

	AccessValues* getAccessValues(unsigned int planeAddress,
			unsigned int blockAddress, unsigned int pageAddress);

	unsigned int* getRadiationDose(unsigned int planeAddress,
			unsigned int blockAddress, unsigned int pageAddress);

	Failpoint* getFailpoint(unsigned int planeAddress,
			unsigned int blockAddress, unsigned int pageAddress);

	bool* wasBitFlipped(unsigned int planeAddress, unsigned int blockAddress,
			unsigned int pageAddress);

	FlashByte* getLatchMask(unsigned int planeAddress,
			unsigned int blockAddress, unsigned int pageAddress);
private:

	void
	resizeBuffers();
};

#endif /* SIMU_FLASHDEBUGINTERFACECLIENT_HPP_ */
