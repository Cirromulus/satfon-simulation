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

#include "flashDebugInterfaceClient.hpp"

#include <arpa/inet.h>
using namespace simu;

void FlashDebugInterfaceClient::sendFlashRequest(functionRequest function, unsigned int planeAddress,
		unsigned int blockAddress, unsigned int pageAddress){
	unsigned int address[3];
	address[0] = planeAddress;
	address[1] = blockAddress;
	address[2] = pageAddress;
	sendRequest(function, reinterpret_cast<char*>(address));
}

bool FlashDebugInterfaceClient::isConnected(){
	sendFlashRequest(F_GETCONFIG, 0, 0, 0);
	FlashConfiguration conf;
	bool connected = recEverything(&conf, sizeof(FlashConfiguration)) == sizeof(FlashConfiguration);
	if(!connected)
	{
	    return false;
	}
	if(conf != mConfig)
	{
        mConfig = conf;
	    resizeBuffers();
	}
	return true;
}

unsigned int FlashDebugInterfaceClient::getPageSize(){
	return mConfig.pageSize;
}

unsigned int FlashDebugInterfaceClient::getBlockSize(){
	return mConfig.blockSize;
}

unsigned int FlashDebugInterfaceClient::getPlaneSize(){
	return mConfig.planeSize;
}

unsigned int FlashDebugInterfaceClient::getCellSize(){
	return mConfig.cellSize;
}

FlashByte* FlashDebugInterfaceClient::getValue(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress){
	sendFlashRequest(F_GETVALUE, planeAddress, blockAddress, pageAddress);
	if(recEverything(dataBuf, mConfig.pageSize* sizeof(FlashByte)) > 0){
		return dataBuf;
	}
	return NULL;
}

AccessValues* FlashDebugInterfaceClient::getAccessValues(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress){
	sendFlashRequest(F_GETACCESSVALUES, planeAddress, blockAddress, pageAddress);
	if(recEverything(avBuf, mConfig.pageSize* sizeof(AccessValues)) > 0){
		return avBuf;
	}
	return NULL;
}

unsigned int* FlashDebugInterfaceClient::getRadiationDose(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress){
	sendFlashRequest(F_GETRADIATIONDOSE, planeAddress, blockAddress, pageAddress);
	if(recEverything(radiationBuf, mConfig.pageSize* sizeof(unsigned int)) > 0){
		return radiationBuf;
	}
	return NULL;
}

Failpoint* FlashDebugInterfaceClient::getFailpoint(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress){
	sendFlashRequest(F_GETFAILPOINT, planeAddress, blockAddress, pageAddress);
	if(recEverything(fpBuf, mConfig.pageSize* sizeof(Failpoint)) > 0){
		return fpBuf;
	}
	return NULL;
}

bool* FlashDebugInterfaceClient::wasBitFlipped(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress){
	sendFlashRequest(F_WASBITFLIPPED, planeAddress, blockAddress, pageAddress);
	if(recEverything(bitFlippedBuf, mConfig.pageSize* sizeof(bool)) > 0){
		return bitFlippedBuf;
	}
	return NULL;
}

FlashByte* FlashDebugInterfaceClient::getLatchMask(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress){
	sendFlashRequest(F_GETLATCHMASK, planeAddress, blockAddress, pageAddress);
	if(recEverything(latchBuf, mConfig.pageSize* sizeof(FlashByte)) > 0){
		return latchBuf;
	}
	return NULL;
}

void
FlashDebugInterfaceClient::resizeBuffers()
{
    printf("Reallocating to %u pagewidth\n", mConfig.pageSize);
    dataBuf       = static_cast<FlashByte*>(realloc(dataBuf, sizeof(FlashByte)            * mConfig.pageSize));
    avBuf         = static_cast<AccessValues*>(realloc(avBuf, sizeof(AccessValues)        * mConfig.pageSize));
    radiationBuf  = static_cast<unsigned int*>(realloc(radiationBuf, sizeof(unsigned int) * mConfig.pageSize));
    fpBuf         = static_cast<Failpoint*>(realloc(fpBuf, sizeof(Failpoint)              * mConfig.pageSize));
    bitFlippedBuf = static_cast<bool*>(realloc(bitFlippedBuf, sizeof(bool)                * mConfig.pageSize));
    latchBuf      = static_cast<FlashByte*>(realloc(latchBuf, sizeof(FlashByte)           * mConfig.pageSize));

}
