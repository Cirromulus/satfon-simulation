#ifndef __FLASHCELL__
#define __FLASHCELL__

#include <stdio.h>
#include <random>
#include <chrono>
#include <functional>
#include <mutex>
#include <iostream>
#include <vector>

#define DATA_TYPE unsigned char
#define PAGE_DATA 512
#define PAGE_AUX (PAGE_DATA / 32)
#define PAGE_SIZE (PAGE_DATA + PAGE_AUX)
#define BLOCK_SIZE 64	//128
#define PLANE_SIZE 4	//8
#define CELL_SIZE 4		//4

#define TID_FLIP_START_PERCENT 0.85

#define BLOCKING_ERROR true

class flashCell;

struct ACCESS_VALUES{
	unsigned int times_read;
	unsigned int times_written;
	unsigned int times_reset;
};

struct FAILPOINT{
	unsigned int erases;
	unsigned int rad;
};

struct FAILPARAM{
	unsigned int mean_erases;
	unsigned int erase_deviation;
	unsigned int mean_idose;
	unsigned int idose_deviation;
};

struct NANDADRESS{
	unsigned int plane;
	unsigned int block;
	unsigned int page;
};

class debugInterface{
	flashCell* cell;
	std::function<void()> notifyTarget;
	bool targetSet = false;
public:
	debugInterface(flashCell *cell) : debugInterface(){
		this->cell = cell;
	}
	debugInterface(){};


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

class f_byte{
	friend class debugInterface;
	friend class flashCell;
	DATA_TYPE word = 0xFF, latch_mask = 0x00;
	ACCESS_VALUES access = {};
	FAILPOINT failpoint = {};
	unsigned int radiation_dose = 0;
	bool wasFlipped = false;

	void flipBit(){
		word ^= 1 << (rand() % (sizeof(DATA_TYPE) * 8));
		wasFlipped = true;
	}
	void latch(){
		latch_mask = 1 << (rand() % (sizeof(DATA_TYPE) * 8));
	}
public:
	f_byte(FAILPOINT f){
		failpoint = f;
	}
	int getWord(DATA_TYPE *wrd){
		access.times_read++;
		*wrd = word & (~latch_mask);
		return failpoint.rad != 0 && radiation_dose > failpoint.rad ? -1 : 0;
	}
	int setWord(DATA_TYPE c){
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
		if(radiation_dose > failpoint.rad * TID_FLIP_START_PERCENT){
			if(rand() % (int)(100* failpoint.rad * (1 - TID_FLIP_START_PERCENT)) < radiation_dose - failpoint.rad * TID_FLIP_START_PERCENT){
				//Bei erreichen der TID -> geringe Wahrscheinlichkeit eines FLIPS
				flipBit();
			}
		}
	}
};

class page{
	friend class debugInterface;
	friend class flashCell;
	std::vector<f_byte> bytes;
	FAILPARAM fpa;
public:
	page(FAILPARAM fpa){
		this->fpa = fpa;
		bytes.reserve(PAGE_SIZE);
		
		auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
		std::mt19937 generator(seed + rand());

		std::normal_distribution<> erase_distribution(fpa.mean_erases,fpa.erase_deviation);
		std::normal_distribution<> tid_distribution(fpa.mean_idose,fpa.idose_deviation);


		for(int i = 0; i < PAGE_SIZE; i++){
			FAILPOINT fpo = {fpa.mean_erases, fpa.mean_idose};
			if(fpa.erase_deviation != 0){
				fpo.erases = std::round(erase_distribution(generator));
			}
			if(fpa.idose_deviation != 0){
				fpo.rad = std::round(tid_distribution(generator));
			}
			bytes.push_back(f_byte(fpo));
		}
	}


	int readPage(DATA_TYPE* buf){
		bool allfailed = true;
		for (unsigned int i = 0; i < PAGE_SIZE; i++){
			allfailed &= bytes[i].getWord(&buf[i]) != 0;
		}
		return allfailed ? -1 : 0;
	}
	int writePage(DATA_TYPE data[PAGE_SIZE]){
		bool allfailed = true;
		for (unsigned int i = 0; i < PAGE_SIZE; i++){
			allfailed &= bytes[i].setWord(data[i]);
		}
		return allfailed ? -1 : 0;
	}
	int resetPage(){
		bool allfailed = true;
		for (unsigned int i = 0; i < PAGE_SIZE; i++){
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

class block{
	friend class debugInterface;
	friend class flashCell;
	std::vector<page> pages;
	//FAILPARAM fpa;
public:
	block(FAILPARAM f){
		pages.reserve(BLOCK_SIZE);
		for(int i = 0; i < BLOCK_SIZE; i++){
			pages.push_back(page(f));
		}
		//fpa = f;
	}
	//~block(){} //Notwendig?

	page *getPage(unsigned int address){
		if (address > BLOCK_SIZE){
			fprintf(stderr, "ACCESS (READ/WRITE) from non-existent Block-address (<%d, was: %d)\n", BLOCK_SIZE, address);
			while(!BLOCKING_ERROR){};
			return NULL;
		}
		return &pages[address];
	}

	int eraseBlock(){
		bool allfailed = false;
		for (unsigned int i = 0; i < BLOCK_SIZE; i++){
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

class plane{
	friend class debugInterface;
	friend class flashCell;
	std::vector<block> blocks;
public:
	plane(FAILPARAM f){
		blocks.reserve(PLANE_SIZE);
		for(int i = 0; i < PLANE_SIZE; i++){
			blocks.push_back(block(f));
		}
	}
	//~plane(){} //Notwendig?

	block *getBlock(unsigned int address){
		if (address > PLANE_SIZE){
			fprintf(stderr, "ACCESS (READ/WRITE) to non-existent Plane-address (<%d, was: %d)\n", PLANE_SIZE, address);
			while(!BLOCKING_ERROR){};
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

class flashCell{
	friend class debugInterface;
	std::vector<plane> planes;
	debugInterface* dbgIf;
	//std::mutex write_mutex;
public:
	flashCell(FAILPARAM f = {100000, 0, 0, 0}){
		srand(time(0)+rand());
		planes.reserve(CELL_SIZE);
		for(int i = 0; i < CELL_SIZE; i++){
			planes.push_back(plane(f));
		}
		dbgIf = new debugInterface(this);
	}

	~flashCell(){
		delete dbgIf;
	}

	void increaseTID(unsigned int rad){
		for(unsigned int i = 0; i < planes.size(); i++){
			planes[i].increaseTID(rad);
		}
	}

	static const unsigned int cellSize = CELL_SIZE;
	static const unsigned int planeSize = PLANE_SIZE;
	static const unsigned int blockSize = BLOCK_SIZE;
	static const unsigned int pageSize = PAGE_SIZE;
	static const unsigned int pageDataSize = PAGE_DATA;

	int readPage(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, DATA_TYPE *buf);

	int writePage(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, DATA_TYPE data[PAGE_SIZE]);

	int eraseBlock(unsigned int planeAddress, unsigned int blockAddress);

	debugInterface* getDebugInterface();
};
#endif
