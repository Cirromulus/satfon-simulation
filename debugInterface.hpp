/*
 * debugInterface.hpp
 *
 *  Created on: 05.07.2016
 *      Author: urinator
 */

#ifndef SIMU_DEBUGINTERFACE_HPP_
#define SIMU_DEBUGINTERFACE_HPP_
#include "types.hpp"
#ifdef WITH_DEBUG_SERVER
#include <functional>
#include <atomic>
#include <thread>
#include <sys/socket.h>
#endif
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
#ifdef WITH_DEBUG_SERVER
	std::function<void()> notifyTarget;
	std::atomic<bool> stop;
	std::thread serverListenerThread;
#endif

	bool createServer();

	void serverListener();

	int handleRequest(char* answerBuf, functionRequest function, unsigned int plane, unsigned int block, unsigned int page);

public:
	DebugInterface(FlashCell *mcell) : cell(mcell){
#ifdef WITH_DEBUG_SERVER
		stop = {false};
		if(!createServer()){
			fprintf(stderr, "Debuginterface %s: Could not create Server!", typeid(this).name());
		}else{
			serverListenerThread = std::thread(&DebugInterface::serverListener, this);
		}
#endif
	}

	~DebugInterface(){
#ifdef WITH_DEBUG_SERVER
		stop = true;
		shutdown(serverSock, SHUT_RDWR);
		//while(!serverListenerThread.joinable()){}

		serverListenerThread.join();
#endif
	}


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
#ifdef WITH_DEBUG_SERVER
	void registerOnChangeFunction(std::function<void()> f);
#endif

	void notifyChange();
};



#endif /* SIMU_DEBUGINTERFACE_HPP_ */
