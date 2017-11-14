#include "flashCell.hpp"


int FlashCell::readPage(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, FlashByte *buf){
	if (planeAddress >= planesPerCell){
		fprintf(stderr, "READ to non-existent Plane-address (<%d, was: %d)\n", planesPerCell, planeAddress);
		return -1;
	}
	if (blockAddress >= blocksPerPlane){
		fprintf(stderr, "READ to non-existent Block-address (<%d, was: %d)\n", planesPerCell, blockAddress);
		return -1;
	}
	if (pageAddress >= pagesPerBlock){
		fprintf(stderr, "READ to non-existent Page-address (<%d, was: %d)\n", planesPerCell, pageAddress);
		return -1;
	}
	readAccesses++;
	return planes[planeAddress].getBlock(blockAddress)->getPage(pageAddress)->readPage(buf);
}

int FlashCell::writePage(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, FlashByte data[pageTotalSize]){
	//mutex.lock();
	if (planeAddress >= planesPerCell){
		fprintf(stderr, "WRITE to non-existent Plane-address (<%d, was: %d)\n", planesPerCell, planeAddress);
		return -1;
	}
	if (blockAddress >= blocksPerPlane){
		fprintf(stderr, "WRITE to non-existent Block-address (<%d, was: %d)\n", planesPerCell, blockAddress);
		return -1;
	}
	if (pageAddress >= pagesPerBlock){
		fprintf(stderr, "WRITE to non-existent Page-address (<%d, was: %d)\n", planesPerCell, pageAddress);
		return -1;
	}
	writeAccesses++;
	return planes[planeAddress].getBlock(blockAddress)->getPage(pageAddress)->writePage(data);
}

int FlashCell::eraseBlock(unsigned int planeAddress, unsigned int blockAddress){
	//mutex.lock();
	if (planeAddress >= planesPerCell){
		fprintf(stderr, "ERASE in non-existent Cell-address (<%d, was: %d)\n", planesPerCell, planeAddress);
		return -1;
	}
	if (blockAddress >= blocksPerPlane){
		fprintf(stderr, "ERASE in non-existent Block-address (<%d, was: %d)\n", planesPerCell, blockAddress);
		return -1;
	}
	eraseAccesses++;
	return planes[planeAddress].getBlock(blockAddress)->eraseBlock();
}

FlashDebugInterface* FlashCell::getDebugInterface(){
	return dbgIf;
}
