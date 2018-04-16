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

Mram::Mram(uint32_t size) :
            DebugServer(mramDebugServerBlockwidth, mramDebugServerStartPort,
            sizeof(uint32_t)), mSize(size)
{
    mData = new uint8_t[size];
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
    uint32_t address;
    memcpy(&address, params, sizeof(uint32_t));
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

uint8_t
Mram::getByte(uint32_t address)
{
    uint8_t ret;
    readBlock(address, &ret, 1);
    return ret;
}
void
Mram::setByte(uint32_t address, uint8_t byte)
{
    writeBlock(address, &byte, 1);
}

void
Mram::writeBlock(uint32_t address, const void* buf, uint16_t length)
{
    writeAccesses++;
    if(address+length > mSize){
        printf("MRAM write access out of bounds (%u of %u)\n", address, mSize);
        return;
    }
    memcpy(&mData[address], buf, length);
    if(notifyChange != nullptr)
    {
        notifyChange();
    }
}

void
Mram::readBlock(uint32_t address, void* buf, uint16_t length)
{
    readAccesses++;
    if(address+length > mSize){
        printf("MRAM read access out of bounds (%u of %u)\n", address, mSize);
        return;
    }
    memcpy(buf, &mData[address], length);
}

uint64_t
Mram::getNumberOfReadAccesses()
{
    return readAccesses;
}
uint64_t
Mram::getNumberOfWriteAccesses()
{
    return writeAccesses;
}
uint64_t
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
