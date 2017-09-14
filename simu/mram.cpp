/*
 * mram.cpp
 *
 *  Created on: 10.09.2017
 *      Author: urinator
 */

#include "mram.hpp"
#include <stdio.h>
#include <string.h>

Mram::Mram(unsigned int size) :
		DebugServer(mramDebugServerBlockwidth, mramDebugServerStartPort, sizeof(unsigned int))
		, mSize(size){
	mData = new unsigned char[size];
	config = {mramDebugServerBlockwidth, size};
	memset(mData, 0, size);
}
Mram::~Mram(){
	delete[] mData;
}

int Mram::handleRequest(char* answerBuf, functionRequest function, char *params){
	unsigned int address = params[0];
	if(address + mramDebugServerBlockwidth > mSize){
		memset(answerBuf, 0, mramDebugServerBlockwidth);
		return mramDebugServerBlockwidth;
	}
	switch(function){
	case F_GETCONFIG:
		memcpy(answerBuf, &config, sizeof(MramConfiguration));
		return sizeof(MramConfiguration);

	case F_GETVALUE:
		//printf("Got request from %u to %u\n",
		//		address, address + mramDebugServerBlockwidth);
		memcpy(answerBuf, &mData[address], mramDebugServerBlockwidth);
		return mramDebugServerBlockwidth;
	default:
		break;
	}
	memset(answerBuf, 0, mramDebugServerBlockwidth);
	return mramDebugServerBlockwidth;
}

unsigned char Mram::getByte(unsigned int address){
	if(address > mSize){
		printf("MRAM read access out of bounds (%u of %u)\n", address, mSize);
		return 0;
	}
	return mData[address];
}
void Mram::setByte(unsigned int address, unsigned char byte){
	if(address > mSize){
		printf("MRAM write access out of bounds (%u of %u)\n", address, mSize);
		return;
	}
	mData[address] = byte;
}
