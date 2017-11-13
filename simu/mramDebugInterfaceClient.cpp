/*
 * debugInterfaceClient.cpp
 *
 *  Created on: 05.07.2016
 *      Author: Pascal Pieper
 */


#include "mramDebugInterfaceClient.hpp"

#include <arpa/inet.h>
#include <string.h>


void MramDebugInterfaceClient::sendFlashRequest(functionRequest function, unsigned int address){
	char buf[sizeof(unsigned int)];
	memcpy(buf, &address, sizeof(unsigned int));
	sendRequest(function, buf);
}

bool MramDebugInterfaceClient::isConnected(){
	sendFlashRequest(F_GETCONFIG, 0);
	MramConfiguration localConf;
	if(recEverything(&localConf, sizeof(MramConfiguration)) != sizeof(MramConfiguration)){
		return false;
	}
	if(localConf.size > 0x1000000 ||
			localConf.blockWidth > localConf.size ||
			localConf.blockWidth > 0x10000){
		return false;
	}
	if(localConf.blockWidth != config.blockWidth){
		dataBuf.resize(localConf.blockWidth, 0);
	}
	config = localConf;
	return true;
}


unsigned int MramDebugInterfaceClient::getBlockWidth(){
	return config.blockWidth;
}

unsigned int MramDebugInterfaceClient::getSize(){
	return config.size;
}

unsigned char* MramDebugInterfaceClient::getValue(unsigned int address){
	//printf("Sending Value Request for %u to %u\n", address, address + config.blockWidth);
	sendFlashRequest(F_GETVALUE, address);
	if(recEverything(&dataBuf[0], config.blockWidth) > 0){
		return &dataBuf[0];
	}
	return NULL;
}
