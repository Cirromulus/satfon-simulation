/*
 * debugInterface.cpp
 *
 *  Created on: 05.07.2016
 *      Author: urinator
 */

#include "debugInterface.hpp"
#include "flashCell.h"
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>


DATA_TYPE debugInterface::getValue(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress){
	return cell->planes[planeAddress].
			blocks[blockAddress].
			pages[pageAddress].
			bytes[wordAddress].word;
}

ACCESS_VALUES debugInterface::getAccessValues(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress){
	return cell->planes[planeAddress].
			blocks[blockAddress].
			pages[pageAddress].
			bytes[wordAddress].access;
}

unsigned int debugInterface::getRadiationDose(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress){
	return cell->planes[planeAddress].
			blocks[blockAddress].
			pages[pageAddress].
			bytes[wordAddress].radiation_dose;
}

FAILPOINT debugInterface::getFailpoint(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress){
	return cell->planes[planeAddress].
				blocks[blockAddress].
				pages[pageAddress].
				bytes[wordAddress].failpoint;
}

bool debugInterface::wasBitFlipped(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress){
	return cell->planes[planeAddress].
				blocks[blockAddress].
				pages[pageAddress].
				bytes[wordAddress].wasFlipped;
}

DATA_TYPE debugInterface::getLatchMask(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress){
	return cell->planes[planeAddress].
			blocks[blockAddress].
			pages[pageAddress].
			bytes[wordAddress].latch_mask;
}

void debugInterface::setLatchMask(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress, DATA_TYPE latch_mask){
	cell->planes[planeAddress].
	blocks[blockAddress].
	pages[pageAddress].
	bytes[wordAddress].latch_mask |= latch_mask;
}

void debugInterface::setFailureValues(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress, FAILPOINT failure){
	cell->planes[planeAddress].
	blocks[blockAddress].
	pages[pageAddress].
	bytes[wordAddress].failpoint = failure;
}

void debugInterface::flipBit(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress){
	cell->planes[planeAddress].
	blocks[blockAddress].
	pages[pageAddress].
	bytes[wordAddress].flipBit();
}
#include <thread>
void debugInterface::flipBitWithRad(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress, unsigned int rad){
	cell->planes[planeAddress].
		blocks[blockAddress].
		pages[pageAddress].
		bytes[wordAddress].flipBit();
	cell->planes[planeAddress].
		blocks[blockAddress].
		pages[pageAddress].
		bytes[wordAddress].increaseTID(rad);
}

void debugInterface::increaseTID(unsigned int rad){
	cell->increaseTID(rad);
}

void debugInterface::setBadBlock(unsigned int planeAddress, unsigned int blockAddress){
	cell->planes[planeAddress].
					blocks[blockAddress].
					pages[0].
					bytes[PAGE_DATA + 5].word = 0x00;		//Siehe Dokumentation von Badblockmarkern
}

flashCell* debugInterface::getCell(){
	return cell;
}

int debugInterface::getCellSize(){
	return cell->cellSize;
}
int debugInterface::getPlaneSize(){
	return cell->planeSize;
}
int debugInterface::getBlockSize(){
	return cell->blockSize;
}
int debugInterface::getPageSize(){
	return cell->pageSize;
}
int debugInterface::getPageDataSize(){
	return cell->pageDataSize;
}

void debugInterface::registerOnChangeFunction(std::function<void()> f){
	notifyTarget = f;
	targetSet = true;
}

void debugInterface::notifyChange(){
	if (targetSet){
		notifyTarget();
	}
}

bool debugInterface::createServer(){

	if ((serverSock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		fprintf(stderr, "cannot create socket");
		return false;
	}

	struct sockaddr_in addr;

	for(int i = 0; i < MAX_INSTANCES_LISTENING; i++){
		memset((char *)&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		addr.sin_port = htons(START_PORT+i);

		if (bind(serverSock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
			printf("Bind failed with Port %d\n", START_PORT+i);
		}else{
			printf("Bind succeeded with Port %d\n", START_PORT+i);
			break;
		}
	}

	return true;
}

void debugInterface::serverListener(){
	int bufsize = sizeof(enum functionRequest) + sizeof(unsigned int) * 3;
	char* buf = (char*)malloc(bufsize);	//plane, block, page

	char* answerBuf = (char*)malloc(PAGE_SIZE * sizeof(ACCESS_VALUES));	//Has to fit the biggest type

	sockaddr remAddr = {0};
	unsigned int remAddrSize = sizeof(remAddr);
	while (!stop) {
		int recvlen = recvfrom(serverSock, buf, bufsize, 0, &remAddr, &remAddrSize);
		if (recvlen == bufsize) {
			functionRequest function = (functionRequest) buf[0];
			unsigned int plane = buf[sizeof(enum functionRequest)];
			unsigned int block = buf[sizeof(enum functionRequest) + sizeof(unsigned int)];
			unsigned int page  = buf[sizeof(enum functionRequest) + sizeof(unsigned int) * 2];

			//printf("received request: function %d, p: %u, b: %u, p: %u\n",
			//		function, plane, block, page);
			int answerSize = handleRequest(answerBuf, function, plane, block, page);
			if(sendto(serverSock, answerBuf, answerSize, 0, &remAddr, remAddrSize) < 0)
				fprintf(stderr, "Failed to send data.\n");

		}else{
			fprintf(stderr, "Debuginterface %s: Misaligned read from socket.\n(Was %d, should %d)\n",  typeid(this).name(), recvlen, bufsize);
		}
	}
	close(serverSock);
}

int debugInterface::handleRequest(char* answerBuf, functionRequest function, unsigned int plane, unsigned int block, unsigned int page){
	switch(function){
	case F_GETVALUE:
		for(int i = 0; i < PAGE_SIZE; i++){
			answerBuf[i*sizeof(DATA_TYPE)] = getValue(plane, block, page, i);
		}
		return PAGE_SIZE*sizeof(DATA_TYPE);
	case F_GETACCESSVALUES:
		for(int i = 0; i < PAGE_SIZE; i++){
			ACCESS_VALUES av = getAccessValues(plane, block, page, i);
			for(unsigned int j = 0; j < sizeof(ACCESS_VALUES); j++){
				answerBuf[i*sizeof(ACCESS_VALUES) + j] = ((char*) &av)[j];
			}
		}
		return PAGE_SIZE*sizeof(ACCESS_VALUES);
	case F_GETRADIATIONDOSE:
		for(int i = 0; i < PAGE_SIZE; i++){
			answerBuf[i*sizeof(unsigned int)] = getRadiationDose(plane, block, page, i);
		}
		return PAGE_SIZE*sizeof(unsigned int);
	case F_GETFAILPOINT:
		for(int i = 0; i < PAGE_SIZE; i++){
			FAILPOINT fp = getFailpoint(plane, block, page, i);
			for(unsigned int j = 0; j < sizeof(FAILPOINT); j++){
				answerBuf[i*sizeof(FAILPOINT) + j] = ((char*) &fp)[j];
			}
		}
		return PAGE_SIZE*sizeof(FAILPOINT);
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

