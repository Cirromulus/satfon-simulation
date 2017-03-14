/*
 * debugInterface.hpp
 *
 *  Created on: 05.07.2016
 *      Author: urinator
 */

#pragma once

#include "types.hpp"

#include <functional>
#include <atomic>
#include <thread>

#define START_PORT 2084
#define MAX_INSTANCES_LISTENING 4

enum functionRequest{
	F_PING,
	F_GETVALUE,
	F_GETACCESSVALUES,
	F_GETRADIATIONDOSE,
	F_GETFAILPOINT,
	F_WASBITFLIPPED,
	F_GETLATCHMASK
};


class FlashCell;

class DebugInterface{
	FlashCell* cell;
	bool targetSet = false;
	int serverSock = 0;
	std::function<void()> notifyTarget;
	std::atomic<bool> stop;
	std::thread serverListenerThread;
	bool createServer();

	void serverListener();

	int handleRequest(char* answerBuf, functionRequest function, unsigned int plane, unsigned int block, unsigned int page);

public:
	DebugInterface(FlashCell *mcell);

	~DebugInterface();


	DATA_TYPE getValue(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress);

	AccessValues getAccessValues(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress);

	unsigned int getRadiationDose(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress);

	Failpoint getFailpoint(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress);

	bool wasBitFlipped(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress);

	DATA_TYPE getLatchMask(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress);

	void setLatchMask(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress, DATA_TYPE latch_mask);

	void setFailureValues(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress, Failpoint failure);

	void flipBit(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress);

	void flipBitWithRad(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress, unsigned int rad);

	void increaseTID(unsigned int rad);

	void setBadBlock(unsigned int planeAddress, unsigned int blockAddress);

	FlashCell* getCell();

	int getCellSize();
	int getPlaneSize();
	int getBlockSize();
	int getPageSize();
	int getPageDataSize();

	void registerOnChangeFunction(std::function<void()> f);

	void notifyChange();
};
