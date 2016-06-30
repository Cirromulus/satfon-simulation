#include "flashCell.h"

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



int flashCell::readPage(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, DATA_TYPE *buf){
	if (planeAddress > CELL_SIZE){
		fprintf(stderr, "READ from non-existent Cell-address (<%d, was: %d)\n", CELL_SIZE, planeAddress);
		return -1;
	}
	//TODO: Fail-Safe getters (If address out of range -> Absturz)
	return planes[planeAddress].getBlock(blockAddress)->getPage(pageAddress)->readPage(buf);
}

int flashCell::writePage(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, DATA_TYPE data[PAGE_SIZE]){
	//mutex.lock();
	if (planeAddress > CELL_SIZE){
		fprintf(stderr, "WRITE to non-existent Cell-address (<%d, was: %d)\n", CELL_SIZE, planeAddress);
		return -1;
	}
	//TODO: Fail-Safe getters (If address out of range -> Absturz)
	int ret = planes[planeAddress].getBlock(blockAddress)->getPage(pageAddress)->writePage(data);
	//mutex.unlock();
	dbgIf->notifyChange();
	return ret;
}

int flashCell::eraseBlock(unsigned int planeAddress, unsigned int blockAddress){
	//mutex.lock();
	if (planeAddress > CELL_SIZE){
		fprintf(stderr, "ERASE in non-existent Cell-address (<%d, was: %d)\n", CELL_SIZE, planeAddress);
		return -1;
	}
	//TODO: Fail-Safe getters (If address out of range -> Absturz)
	int ret = planes[planeAddress].getBlock(blockAddress)->eraseBlock();
	//mutex.unlock();
	dbgIf->notifyChange();
	return ret;
}

debugInterface* flashCell::getDebugInterface(){
	return dbgIf;
}
