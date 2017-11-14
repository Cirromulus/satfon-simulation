#ifndef __FLASHCELL__
#define __FLASHCELL__

#include <simu/config.hpp>
#include "flashDebugInterface.hpp"

#include <chrono>
#include <random>
#include <iostream>
#include <vector>


using namespace simu;

class f_byte{
	friend class FlashDebugInterface;
	friend class FlashCell;
	FlashByte word = 0xFF, latch_mask = 0x00;
	AccessValues access = {0,0,0};
	Failpoint failpoint = {0,0};
	unsigned int radiation_dose = 0;
	bool wasFlipped = false;

	void flipBit(){
		word ^= 1 << (rand() % (sizeof(FlashByte) * 8));
		wasFlipped = true;
	}
	void latch(){
		latch_mask = 1 << (rand() % (sizeof(FlashByte) * 8));
	}
public:
	f_byte(Failpoint f) : failpoint(f){
	}
	int getWord(FlashByte *wrd){
		access.times_read++;
		*wrd = word & (~latch_mask);
		return failpoint.rad != 0 && radiation_dose > failpoint.rad ? -1 : 0;
	}
	int setWord(FlashByte c){
		access.times_written++;
		word &= c;
		return failpoint.rad != 0 && radiation_dose > failpoint.rad ? -1 : 0;
	}
	int resetWord(){
		access.times_reset++;
		word = 0xFF;
		if (failpoint.erases != 0 && access.times_reset > failpoint.erases){
			latch();
			return -1;
		}
		return 0;
	}
	void increaseTID(unsigned int rad){
		radiation_dose += rad;
		if(failpoint.rad == 0){
			return;
		}
		if(radiation_dose > failpoint.rad){
			latch();
		}
		if(radiation_dose > failpoint.rad * tidFlipStartInPercent){
			if(rand() % static_cast<int>(100* failpoint.rad * (1 - tidFlipStartInPercent)) < radiation_dose - failpoint.rad * tidFlipStartInPercent){
				//Bei erreichen der TID -> geringe Wahrscheinlichkeit eines FLIPS
				flipBit();
			}
		}
	}
};

class Page{
	friend class FlashDebugInterface;
	friend class FlashCell;
	std::vector<f_byte> bytes;
	Failparam fpa;
public:
	Page(Failparam mfpa) : fpa(mfpa){
		bytes.reserve(pageTotalSize);
		
		auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
		std::mt19937 generator(seed + rand());

		std::normal_distribution<> erase_distribution(fpa.mean_erases,fpa.erase_deviation);
		std::normal_distribution<> tid_distribution(fpa.mean_idose,fpa.idose_deviation);


		for(int i = 0; i < pageTotalSize; i++){
			Failpoint fpo = {fpa.mean_erases, fpa.mean_idose};
			if(fpa.erase_deviation != 0){
				fpo.erases = std::round(erase_distribution(generator));
			}
			if(fpa.idose_deviation != 0){
				fpo.rad = std::round(tid_distribution(generator));
			}
			bytes.push_back(f_byte(fpo));
		}
	}


	int readPage(FlashByte* buf){
		bool allfailed = true;
		for (unsigned int i = 0; i < pageTotalSize; i++){
			allfailed &= bytes[i].getWord(&buf[i]) != 0;
		}
		return allfailed ? -1 : 0;
	}
	int writePage(FlashByte data[pageTotalSize]){
		bool allfailed = true;
		for (unsigned int i = 0; i < pageTotalSize; i++){
			allfailed &= bytes[i].setWord(data[i]);
		}
		return allfailed ? -1 : 0;
	}
	int resetPage(){
		bool allfailed = true;
		for (unsigned int i = 0; i < pageTotalSize; i++){
			allfailed &= bytes[i].resetWord() != 0;
		}
		return allfailed ? -1 : 0;
	}
	void increaseTID(unsigned int rad){
		for(unsigned int i = 0; i < bytes.size(); i++){
			bytes[i].increaseTID(rad);
		}
	}
};

class Block{
	friend class FlashDebugInterface;
	friend class FlashCell;
	std::vector<Page> pages;
public:
	Block(Failparam f){
		pages.reserve(pagesPerBlock);
		for(int i = 0; i < pagesPerBlock; i++){
			pages.push_back(Page(f));
		}
	}

	Page *getPage(unsigned int address){
		if (address >= pagesPerBlock){
			fprintf(stderr, "ACCESS (READ/WRITE) from non-existent Block-address (<%d, was: %d)\n", pagesPerBlock, address);
			return NULL;
		}
		return &pages[address];
	}

	int eraseBlock(){
		bool allfailed = false;
		for (unsigned int i = 0; i < pagesPerBlock; i++){
			allfailed |= pages[i].resetPage() < 0;
		}
		return allfailed ? -1 : 0;
	}

	void increaseTID(unsigned int rad){
		for(unsigned int i = 0; i < pages.size(); i++){
			pages[i].increaseTID(rad);
		}
	}
};

class Plane{
	friend class FlashDebugInterface;
	friend class FlashCell;
	std::vector<Block> blocks;
public:
	Plane(Failparam f){
		blocks.reserve(blocksPerPlane);
		for(int i = 0; i < blocksPerPlane; i++){
			blocks.push_back(Block(f));
		}
	}
	//~plane(){} //Notwendig?

	Block *getBlock(unsigned int address){
		if (address >= blocksPerPlane){
			fprintf(stderr, "ACCESS (READ/WRITE) to non-existent Plane-address (<%d, was: %d)\n", blocksPerPlane, address);
			return NULL;
		}
		return &blocks[address];
	}

	void increaseTID(unsigned int rad){
		for(unsigned int i = 0; i < blocks.size(); i++){
			blocks[i].increaseTID(rad);
		}
	}
};

class FlashCell{
	friend class FlashDebugInterface;
	std::vector<Plane> planes;
	FlashDebugInterface* dbgIf;
	unsigned long readAccesses  = 0;
	unsigned long writeAccesses = 0;
	unsigned long eraseAccesses = 0;

public:
	FlashCell(Failparam f = {100000, 0, 0, 0}){
		srand(time(0)+rand());
		planes.reserve(planesPerCell);
		for(int i = 0; i < planesPerCell; i++){
			planes.push_back(Plane(f));
		}
		dbgIf = new FlashDebugInterface(this);
	}

	~FlashCell(){
		delete dbgIf;
	}

	void increaseTID(unsigned int rad){
		for(unsigned int i = 0; i < planes.size(); i++){
			planes[i].increaseTID(rad);
		}
	}

	int readPage  (unsigned int planeAddress, unsigned int blockAddress,
	               unsigned int pageAddress, FlashByte *buf);

	int writePage (unsigned int planeAddress, unsigned int blockAddress,
	               unsigned int pageAddress, FlashByte data[pageTotalSize]);

	int eraseBlock(unsigned int planeAddress, unsigned int blockAddress);

	FlashDebugInterface* getDebugInterface();
};
#endif
