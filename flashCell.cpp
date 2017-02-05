#include "flashCell.h"


int FlashCell::readPage(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, DATA_TYPE *buf){
	if (planeAddress >= CELL_SIZE){
		fprintf(stderr, "READ to non-existent Plane-address (<%d, was: %d)\n", CELL_SIZE, planeAddress);
		return -1;
	}
	if (blockAddress >= PLANE_SIZE){
		fprintf(stderr, "READ to non-existent Block-address (<%d, was: %d)\n", CELL_SIZE, blockAddress);
		return -1;
	}
	if (pageAddress >= BLOCK_SIZE){
		fprintf(stderr, "READ to non-existent Page-address (<%d, was: %d)\n", CELL_SIZE, pageAddress);
		return -1;
	}
	return planes[planeAddress].getBlock(blockAddress)->getPage(pageAddress)->readPage(buf);
}

int FlashCell::writePage(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, DATA_TYPE data[PAGE_SIZE]){
	//mutex.lock();
	if (planeAddress >= CELL_SIZE){
		fprintf(stderr, "WRITE to non-existent Plane-address (<%d, was: %d)\n", CELL_SIZE, planeAddress);
		return -1;
	}
	if (blockAddress >= PLANE_SIZE){
		fprintf(stderr, "WRITE to non-existent Block-address (<%d, was: %d)\n", CELL_SIZE, blockAddress);
		return -1;
	}
	if (pageAddress >= BLOCK_SIZE){
		fprintf(stderr, "WRITE to non-existent Page-address (<%d, was: %d)\n", CELL_SIZE, pageAddress);
		return -1;
	}
	int ret = planes[planeAddress].getBlock(blockAddress)->getPage(pageAddress)->writePage(data);
	//mutex.unlock();
	dbgIf->notifyChange();
	return ret;
}

int FlashCell::eraseBlock(unsigned int planeAddress, unsigned int blockAddress){
	//mutex.lock();
	if (planeAddress >= CELL_SIZE){
		fprintf(stderr, "ERASE in non-existent Cell-address (<%d, was: %d)\n", CELL_SIZE, planeAddress);
		return -1;
	}
	if (blockAddress >= PLANE_SIZE){
		fprintf(stderr, "ERASE in non-existent Block-address (<%d, was: %d)\n", CELL_SIZE, blockAddress);
		return -1;
	}
	int ret = planes[planeAddress].getBlock(blockAddress)->eraseBlock();
	//mutex.unlock();
	dbgIf->notifyChange();
	return ret;
}

DebugInterface* FlashCell::getDebugInterface(){
	return dbgIf;
}
