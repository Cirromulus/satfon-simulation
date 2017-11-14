/*
 * debugInterfaceClient.cpp
 *
 *  Created on: 05.07.2016
 *      Author: Pascal Pieper
 */


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
	return recEverything(&config, sizeof(FlashConfiguration)) == sizeof(FlashConfiguration);
}

unsigned int FlashDebugInterfaceClient::getPageSize(){
	return config.pageSize;
}

unsigned int FlashDebugInterfaceClient::getBlockSize(){
	return config.blockSize;
}

unsigned int FlashDebugInterfaceClient::getPlaneSize(){
	return config.planeSize;
}

unsigned int FlashDebugInterfaceClient::getCellSize(){
	return config.cellSize;
}

FlashByte* FlashDebugInterfaceClient::getValue(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress){
	sendFlashRequest(F_GETVALUE, planeAddress, blockAddress, pageAddress);
	if(recEverything(dataBuf, pageTotalSize* sizeof(FlashByte)) > 0){
		return dataBuf;
	}
	return NULL;
}

AccessValues* FlashDebugInterfaceClient::getAccessValues(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress){
	sendFlashRequest(F_GETACCESSVALUES, planeAddress, blockAddress, pageAddress);
	if(recEverything(avBuf, pageTotalSize* sizeof(AccessValues)) > 0){
		return avBuf;
	}
	return NULL;
}

unsigned int* FlashDebugInterfaceClient::getRadiationDose(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress){
	sendFlashRequest(F_GETRADIATIONDOSE, planeAddress, blockAddress, pageAddress);
	if(recEverything(radiationBuf, pageTotalSize* sizeof(unsigned int)) > 0){
		return radiationBuf;
	}
	return NULL;
}

Failpoint* FlashDebugInterfaceClient::getFailpoint(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress){
	sendFlashRequest(F_GETFAILPOINT, planeAddress, blockAddress, pageAddress);
	if(recEverything(fpBuf, pageTotalSize* sizeof(Failpoint)) > 0){
		return fpBuf;
	}
	return NULL;
}

bool* FlashDebugInterfaceClient::wasBitFlipped(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress){
	sendFlashRequest(F_WASBITFLIPPED, planeAddress, blockAddress, pageAddress);
	if(recEverything(bitFlippedBuf, pageTotalSize* sizeof(bool)) > 0){
		return bitFlippedBuf;
	}
	return NULL;
}

FlashByte* FlashDebugInterfaceClient::getLatchMask(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress){
	sendFlashRequest(F_GETLATCHMASK, planeAddress, blockAddress, pageAddress);
	if(recEverything(latchBuf, pageTotalSize* sizeof(FlashByte)) > 0){
		return latchBuf;
	}
	return NULL;
}
