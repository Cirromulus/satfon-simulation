/*
 * debugInterface.hpp
 *
 *  Created on: 05.07.2016
 *      Author: urinator
 */

#ifndef SIMU_DEBUGINTERFACE_HPP_
#define SIMU_DEBUGINTERFACE_HPP_
#include <functional>
#include <atomic>
#include <thread>
#include "types.hpp"

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


class flashCell;

class debugInterface{
	flashCell* cell;
	std::function<void()> notifyTarget;
	bool targetSet = false;
	int serverSock = 0;
	std::atomic<bool> stop;
	std::thread serverListenerThread;

	debugInterface(){
		cell = NULL;
	};

	bool createServer();

	void serverListener();

	int handleRequest(char* answerBuf, functionRequest function, unsigned int plane, unsigned int block, unsigned int page);

public:
	debugInterface(flashCell *cell) : debugInterface(){
		this->cell = cell;
		stop = {false};
		if(!createServer()){
			fprintf(stderr, "Debuginterface %s: Could not create Server!", typeid(this).name());
		}else{
			serverListenerThread = std::thread(&debugInterface::serverListener, this);
		}
	}

	~debugInterface(){
		stop = true;
		//while(!serverListenerThread.joinable()){}

		serverListenerThread.join();
	}


	DATA_TYPE getValue(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress);

	ACCESS_VALUES getAccessValues(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress);

	unsigned int getRadiationDose(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress);

	FAILPOINT getFailpoint(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress);

	bool wasBitFlipped(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress);

	DATA_TYPE getLatchMask(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress);

	void setLatchMask(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress, DATA_TYPE latch_mask);

	void setFailureValues(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress, FAILPOINT failure);

	void flipBit(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress);

	void flipBitWithRad(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress, unsigned int rad);

	void increaseTID(unsigned int rad);

	void setBadBlock(unsigned int planeAddress, unsigned int blockAddress);

	flashCell* getCell();

	int getCellSize();
	int getPlaneSize();
	int getBlockSize();
	int getPageSize();
	int getPageDataSize();

	void registerOnChangeFunction(std::function<void()> f);

	void notifyChange();
};



#endif /* SIMU_DEBUGINTERFACE_HPP_ */
