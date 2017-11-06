/*
 * FlashDebugInterface.hpp
 *
 *  Created on: 05.07.2016
 *      Author: urinator
 */

#pragma once

#include "types.hpp"
#include "debugServer.hpp"

struct FlashConfiguration{
	unsigned int pageSize;
	unsigned int blockSize;
	unsigned int planeSize;
	unsigned int cellSize;
};

static constexpr unsigned int flashDebugServerStartPort = 2084;

class FlashCell;

class FlashDebugInterface : private DebugServer{
	FlashCell* cell;
	int handleRequest(char* answerBuf, functionRequest function, char *params) override;
public:
	FlashDebugInterface(FlashCell *mcell);

	~FlashDebugInterface();


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

	unsigned long getNumberOfReadAccesses();
	unsigned long getNumberOfWriteAccesses();
	unsigned long getNumberOfEraseAccesses();
	void resetGlobalUsage();

	unsigned long getElapsedTimeUsec();

	FlashCell* getCell();

	int getCellSize();
	int getPlaneSize();
	int getBlockSize();
	int getPageSize();
	int getPageDataSize();

	std::ostream& serialize(std::ostream& stream);

	void deserialize(std::istream& stream);
};
