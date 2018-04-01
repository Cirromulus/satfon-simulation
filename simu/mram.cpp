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


#include "mram.hpp"
#include <stdio.h>
#include <string.h>
#include <fstream>

Mram::Mram(unsigned int size) :
            DebugServer(mramDebugServerBlockwidth, mramDebugServerStartPort,
            sizeof(unsigned int)), mSize(size)
{
    mData = new unsigned char[size];
    mConfig = {mramDebugServerBlockwidth, size};
    memset(mData, 0, size);
}
Mram::~Mram()
{
    stopServer();
    delete[] mData;
    mData = nullptr;
}

int Mram::handleRequest(char* answerBuf, functionRequest function, char *params)
{
    unsigned int address;
    memcpy(&address, params, sizeof(unsigned int));
    if(mData == nullptr)
    {
        return mramDebugServerBlockwidth;
    }
    if(address + mramDebugServerBlockwidth > mSize){
        printf("Got invalid request address at %u (< %u)\n", address, mSize);
        memset(answerBuf, 0, mramDebugServerBlockwidth);
        return mramDebugServerBlockwidth;
    }
    switch(function){
    case F_GETCONFIG:
        memcpy(answerBuf, &mConfig, sizeof(MramConfiguration));
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

void
Mram::registerOnchangeFunction(std::function<void()> fun)
{   //Puttin' the fun in function
    notifyChange = fun;
}

unsigned char
Mram::getByte(unsigned int address)
{
    readAccesses++;
	if(address > mSize){
		printf("MRAM read access out of bounds (%u of %u)\n", address, mSize);
		return 0;
	}
	return mData[address];
}
void
Mram::setByte(unsigned int address, unsigned char byte)
{
    writeAccesses++;
	if(address > mSize){
		printf("MRAM write access out of bounds (%u of %u)\n", address, mSize);
		return;
	}
	mData[address] = byte;
	if(notifyChange != nullptr)
	{
	    notifyChange();
	}
}

unsigned long
Mram::getNumberOfReadAccesses()
{
    return readAccesses;
}
unsigned long
Mram::getNumberOfWriteAccesses()
{
    return writeAccesses;
}
unsigned long
Mram::getElapsedTimeUsec()
{
    return (readAccesses + writeAccesses) / 1000;
}

std::ostream&
Mram::serialize(std::ostream& stream)
{
	stream.write(reinterpret_cast<char*>(&mConfig), sizeof(MramConfiguration));
	stream.write(reinterpret_cast<char*>(mData), mConfig.size);
	return stream;
}
void
Mram::deserialize(std::istream& stream){
	MramConfiguration fileconf;
	stream.read(reinterpret_cast<char*>(&fileconf), sizeof(MramConfiguration));
	if(memcmp(&fileconf, &mConfig, sizeof(MramConfiguration)) != 0)
	{
		printf("Serializing config differs to own!\n");
		return;
	}
	stream.read(reinterpret_cast<char*>(mData), mConfig.size);
}
