/*
 * debugInterface.cpp
 *
 *  Created on: 05.07.2016
 *      Author: urinator
 */

#include "debugInterface.hpp"

#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>

#include "flashCell.h"


DATA_TYPE DebugInterface::getValue(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress){
	return cell->planes[planeAddress].
			blocks[blockAddress].
			pages[pageAddress].
			bytes[wordAddress].word;
}

AccessValues DebugInterface::getAccessValues(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress){
	return cell->planes[planeAddress].
			blocks[blockAddress].
			pages[pageAddress].
			bytes[wordAddress].access;
}

unsigned int DebugInterface::getRadiationDose(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress){
	return cell->planes[planeAddress].
			blocks[blockAddress].
			pages[pageAddress].
			bytes[wordAddress].radiation_dose;
}

Failpoint DebugInterface::getFailpoint(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress){
	return cell->planes[planeAddress].
				blocks[blockAddress].
				pages[pageAddress].
				bytes[wordAddress].failpoint;
}

bool DebugInterface::wasBitFlipped(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress){
	return cell->planes[planeAddress].
				blocks[blockAddress].
				pages[pageAddress].
				bytes[wordAddress].wasFlipped;
}

DATA_TYPE DebugInterface::getLatchMask(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress){
	return cell->planes[planeAddress].
			blocks[blockAddress].
			pages[pageAddress].
			bytes[wordAddress].latch_mask;
}

void DebugInterface::setLatchMask(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress, DATA_TYPE latch_mask){
	cell->planes[planeAddress].
	blocks[blockAddress].
	pages[pageAddress].
	bytes[wordAddress].latch_mask |= latch_mask;
}

void DebugInterface::setFailureValues(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress, Failpoint failure){
	cell->planes[planeAddress].
	blocks[blockAddress].
	pages[pageAddress].
	bytes[wordAddress].failpoint = failure;
}

void DebugInterface::flipBit(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress){
	cell->planes[planeAddress].
	blocks[blockAddress].
	pages[pageAddress].
	bytes[wordAddress].flipBit();
}

void DebugInterface::flipBitWithRad(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress, unsigned int rad){
	cell->planes[planeAddress].
		blocks[blockAddress].
		pages[pageAddress].
		bytes[wordAddress].flipBit();
	cell->planes[planeAddress].
		blocks[blockAddress].
		pages[pageAddress].
		bytes[wordAddress].increaseTID(rad);
}

void DebugInterface::increaseTID(unsigned int rad){
	cell->increaseTID(rad);
}

void DebugInterface::setBadBlock(unsigned int planeAddress, unsigned int blockAddress){
	cell->planes[planeAddress].
					blocks[blockAddress].
					pages[0].
					bytes[PAGE_DATA + 5].word = 0x00;		//Siehe Dokumentation von Badblockmarkern
}

FlashCell* DebugInterface::getCell(){
	return cell;
}

int DebugInterface::getCellSize(){
	return cell->cellSize;
}
int DebugInterface::getPlaneSize(){
	return cell->planeSize;
}
int DebugInterface::getBlockSize(){
	return cell->blockSize;
}
int DebugInterface::getPageSize(){
	return cell->pageSize;
}
int DebugInterface::getPageDataSize(){
	return cell->pageDataSize;
}

#ifdef WITH_DEBUG_SERVER
void DebugInterface::registerOnChangeFunction(std::function<void()> f){
	notifyTarget = f;
	targetSet = true;
}
#endif

void DebugInterface::notifyChange(){
#ifdef WITH_DEBUG_SERVER
	if (targetSet){
		notifyTarget();
	}
#endif
}

bool DebugInterface::createServer(){
#ifdef WITH_DEBUG_SERVER
	if ((serverSock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		fprintf(stderr, "cannot create socket");
		return false;
	}

	struct sockaddr_in addr;

	for(int i = 0; i < MAX_INSTANCES_LISTENING; i++){
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		addr.sin_port = htons(START_PORT+i);

		if (bind(serverSock, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
			fprintf(stderr, "Bind failed with Port %d\n", START_PORT+i);
		}else{
			//printf("Bind succeeded with Port %d\n", START_PORT+i);
			break;
		}
	}
#endif
	return true;
}

void DebugInterface::serverListener(){
#ifdef WITH_DEBUG_SERVER
	int bufsize = sizeof(enum functionRequest) + sizeof(unsigned int) * 3;
	char* buf = new char[bufsize];	//plane, block, page

	char* answerBuf = new char[PAGE_SIZE * sizeof(AccessValues)];	//Has to fit the biggest type

	sockaddr remAddr;
	unsigned int remAddrSize = sizeof(remAddr);
	while (!stop) {
		int recvlen = recvfrom(serverSock, buf, bufsize, 0, &remAddr, &remAddrSize);
		if (recvlen == bufsize) {
			functionRequest function = static_cast<functionRequest> (buf[0]);
			unsigned int plane = buf[sizeof(enum functionRequest)];
			unsigned int block = buf[sizeof(enum functionRequest) + sizeof(unsigned int)];
			unsigned int page  = buf[sizeof(enum functionRequest) + sizeof(unsigned int) * 2];

			//printf("received request: function %d, p: %u, b: %u, p: %u\n",
			//		function, plane, block, page);
			int answerSize = handleRequest(answerBuf, function, plane, block, page);
			if(sendto(serverSock, answerBuf, answerSize, 0, &remAddr, remAddrSize) < 0)
				fprintf(stderr, "Failed to send data.\n");

		}else{
			if(recvlen > 0)
				fprintf(stderr, "Debuginterface %s: Misaligned read from socket.\n(Was %d, should %d)\n",  typeid(this).name(), recvlen, bufsize);
		}
	}
	delete[] buf;
	delete[] answerBuf;
	close(serverSock);
#endif
}

int DebugInterface::handleRequest(char* answerBuf, functionRequest function, unsigned int plane, unsigned int block, unsigned int page){
	switch(function){
	case F_GETVALUE:
		for(int i = 0; i < PAGE_SIZE; i++){
			answerBuf[i*sizeof(DATA_TYPE)] = getValue(plane, block, page, i);
		}
		return PAGE_SIZE*sizeof(DATA_TYPE);
	case F_GETACCESSVALUES:
		for(int i = 0; i < PAGE_SIZE; i++){
			AccessValues av = getAccessValues(plane, block, page, i);
			for(unsigned int j = 0; j < sizeof(AccessValues); j++){
				answerBuf[i*sizeof(AccessValues) + j] = reinterpret_cast<char*> (&av)[j];
			}
		}
		return PAGE_SIZE*sizeof(AccessValues);
	case F_GETRADIATIONDOSE:
		for(int i = 0; i < PAGE_SIZE; i++){
			answerBuf[i*sizeof(unsigned int)] = getRadiationDose(plane, block, page, i);
		}
		return PAGE_SIZE*sizeof(unsigned int);
	case F_GETFAILPOINT:
		for(int i = 0; i < PAGE_SIZE; i++){
			Failpoint fp = getFailpoint(plane, block, page, i);
			for(unsigned int j = 0; j < sizeof(Failpoint); j++){
				answerBuf[i*sizeof(Failpoint) + j] = reinterpret_cast<char*> (&fp)[j];
			}
		}
		return PAGE_SIZE*sizeof(Failpoint);
	case F_WASBITFLIPPED:
		for(int i = 0; i < PAGE_SIZE; i++){
			answerBuf[i*sizeof(bool)] = wasBitFlipped(plane, block, page, i);
		}
		return PAGE_SIZE*sizeof(bool);
	case F_GETLATCHMASK:
		for(int i = 0; i < PAGE_SIZE; i++){
			answerBuf[i*sizeof(DATA_TYPE)] = getLatchMask(plane, block, page, i);
		}
		return PAGE_SIZE*sizeof(DATA_TYPE);
	default:
		answerBuf[0] = 0xFF;
		return 1;
	}
}

