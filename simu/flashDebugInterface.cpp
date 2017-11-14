/*
 * FlashDebugInterface.cpp
 *
 *  Created on: 05.07.2016
 *      Author: Pascal Pieper
 */

#include "flashDebugInterface.hpp"
#include "string.h"
#include "flashCell.hpp"

FlashDebugInterface::FlashDebugInterface(FlashCell *mcell) :
	DebugServer(simu::pageTotalSize, flashDebugServerStartPort), cell(mcell){};

FlashDebugInterface::~FlashDebugInterface(){};

FlashByte FlashDebugInterface::getValue(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress){
	return cell->planes[planeAddress].
			blocks[blockAddress].
			pages[pageAddress].
			bytes[wordAddress].word;
}

AccessValues FlashDebugInterface::getAccessValues(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress){
	return cell->planes[planeAddress].
			blocks[blockAddress].
			pages[pageAddress].
			bytes[wordAddress].access;
}

unsigned int FlashDebugInterface::getRadiationDose(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress){
	return cell->planes[planeAddress].
			blocks[blockAddress].
			pages[pageAddress].
			bytes[wordAddress].radiation_dose;
}

Failpoint FlashDebugInterface::getFailpoint(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress){
	return cell->planes[planeAddress].
				blocks[blockAddress].
				pages[pageAddress].
				bytes[wordAddress].failpoint;
}

bool FlashDebugInterface::wasBitFlipped(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress){
	return cell->planes[planeAddress].
				blocks[blockAddress].
				pages[pageAddress].
				bytes[wordAddress].wasFlipped;
}

FlashByte FlashDebugInterface::getLatchMask(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress){
	return cell->planes[planeAddress].
			blocks[blockAddress].
			pages[pageAddress].
			bytes[wordAddress].latch_mask;
}

void FlashDebugInterface::setLatchMask(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress, FlashByte latch_mask){
	cell->planes[planeAddress].
	blocks[blockAddress].
	pages[pageAddress].
	bytes[wordAddress].latch_mask |= latch_mask;
}

void FlashDebugInterface::setFailureValues(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress, Failpoint failure){
	cell->planes[planeAddress].
	blocks[blockAddress].
	pages[pageAddress].
	bytes[wordAddress].failpoint = failure;
}

void FlashDebugInterface::flipBit(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress){
	cell->planes[planeAddress].
	blocks[blockAddress].
	pages[pageAddress].
	bytes[wordAddress].flipBit();
}

void FlashDebugInterface::flipBitWithRad(unsigned int planeAddress, unsigned int blockAddress, unsigned int pageAddress, unsigned int wordAddress, unsigned int rad){
	cell->planes[planeAddress].
		blocks[blockAddress].
		pages[pageAddress].
		bytes[wordAddress].flipBit();
	cell->planes[planeAddress].
		blocks[blockAddress].
		pages[pageAddress].
		bytes[wordAddress].increaseTID(rad);
}

void FlashDebugInterface::increaseTID(unsigned int rad){
	cell->increaseTID(rad);
}

void FlashDebugInterface::setBadBlock(unsigned int planeAddress, unsigned int blockAddress){
	cell->planes[planeAddress].
					blocks[blockAddress].
					pages[0].
					bytes[pageDataSize + 5].word = 0x00;		//Siehe Dokumentation von Badblockmarkern
}

unsigned long FlashDebugInterface::getNumberOfReadAccesses()
{
	return cell->readAccesses;
}

unsigned long FlashDebugInterface::getNumberOfWriteAccesses()
{
	return cell->writeAccesses;
}
unsigned long FlashDebugInterface::getNumberOfEraseAccesses()
{
	return cell->eraseAccesses;
}

void FlashDebugInterface::resetGlobalUsage()
{
	cell->readAccesses = 0;
	cell->writeAccesses = 0;
	cell->eraseAccesses = 0;
}

unsigned long FlashDebugInterface::getElapsedTimeUsec()
{
	return cell->readAccesses * usecRead + cell->writeAccesses * usecWrite + cell->eraseAccesses * usecErase;
}

FlashCell* FlashDebugInterface::getCell(){
	return cell;
}

int FlashDebugInterface::getCellSize(){
	return simu::planesPerCell;
}
int FlashDebugInterface::getPlaneSize(){
	return simu::blocksPerPlane;
}
int FlashDebugInterface::getBlockSize(){
	return simu::pagesPerBlock;
}
int FlashDebugInterface::getPageSize(){
	return simu::pageTotalSize;
}
int FlashDebugInterface::getPageDataSize(){
	return simu::pageDataSize;
}

int FlashDebugInterface::handleRequest(char* answerBuf, functionRequest function, char *params) {
	unsigned int plane = params[0];
	unsigned int block = params[sizeof(unsigned int)];
	unsigned int page  = params[sizeof(unsigned int) * 2];

	if(plane >= planesPerCell || block >= blocksPerPlane || page >= pagesPerBlock){
		memset(answerBuf, 0, pageTotalSize*sizeof(FlashByte));
		return pageTotalSize*sizeof(FlashByte);
	}
	FlashConfiguration config = {simu::pageTotalSize, simu::pagesPerBlock, simu::blocksPerPlane, simu::planesPerCell};
	switch(function){
	case F_GETCONFIG:
		memcpy(answerBuf, &config, sizeof(FlashConfiguration));
		return sizeof(FlashConfiguration);
	case F_GETVALUE:
		for(int i = 0; i < pageTotalSize; i++){
			answerBuf[i*sizeof(FlashByte)] = getValue(plane, block, page, i);
		}
		return pageTotalSize*sizeof(FlashByte);
	case F_GETACCESSVALUES:
		for(int i = 0; i < pageTotalSize; i++){
			AccessValues av = getAccessValues(plane, block, page, i);
			for(unsigned int j = 0; j < sizeof(AccessValues); j++){
				answerBuf[i*sizeof(AccessValues) + j] = reinterpret_cast<char*> (&av)[j];
			}
		}
		return pageTotalSize*sizeof(AccessValues);
	case F_GETRADIATIONDOSE:
		for(int i = 0; i < pageTotalSize; i++){
			answerBuf[i*sizeof(unsigned int)] = getRadiationDose(plane, block, page, i);
		}
		return pageTotalSize*sizeof(unsigned int);
	case F_GETFAILPOINT:
		for(int i = 0; i < pageTotalSize; i++){
			Failpoint fp = getFailpoint(plane, block, page, i);
			for(unsigned int j = 0; j < sizeof(Failpoint); j++){
				answerBuf[i*sizeof(Failpoint) + j] = reinterpret_cast<char*> (&fp)[j];
			}
		}
		return pageTotalSize*sizeof(Failpoint);
	case F_WASBITFLIPPED:
		for(int i = 0; i < pageTotalSize; i++){
			answerBuf[i*sizeof(bool)] = wasBitFlipped(plane, block, page, i);
		}
		return pageTotalSize*sizeof(bool);
	case F_GETLATCHMASK:
		for(int i = 0; i < pageTotalSize; i++){
			answerBuf[i*sizeof(FlashByte)] = getLatchMask(plane, block, page, i);
		}
		return pageTotalSize*sizeof(FlashByte);
	default:
		answerBuf[0] = 0xFF;
		return 1;
	}
}

std::ostream& FlashDebugInterface::serialize(std::ostream& stream)
{
	//TODO: Add failpoints
	FlashConfiguration config = {simu::pageTotalSize, simu::pagesPerBlock, simu::blocksPerPlane, simu::planesPerCell};
	stream.write(reinterpret_cast<char*>(&config), sizeof(FlashConfiguration));
	for(unsigned plane = 0; plane < simu::planesPerCell; plane++)
	{
		for(unsigned block = 0; block < simu::blocksPerPlane; block++)
		{
			for(unsigned page = 0; page < simu::pagesPerBlock; page++)
			{
				for(unsigned byte = 0; byte < simu::pageTotalSize; byte++)
				{
					stream.write(reinterpret_cast<char*>(
							&cell->planes[plane].
										blocks[block].
										pages[page].
										bytes[byte].word)
					             , 1);
				}
			}
		}
	}
	return stream;
}

void FlashDebugInterface::deserialize(std::istream& stream)
{
	//TODO: Add failpoints
	FlashConfiguration ownconfig = {simu::pageTotalSize, simu::pagesPerBlock, simu::blocksPerPlane, simu::planesPerCell};;
	FlashConfiguration config;
	stream.read(reinterpret_cast<char*>(&config), sizeof(FlashConfiguration));
	if(memcmp(&ownconfig, &config, sizeof(FlashConfiguration)) != 0)
	{
		printf("Serializing config differs to own!\n");
		return;
	}
	for(unsigned plane = 0; plane < simu::planesPerCell; plane++)
	{
		for(unsigned block = 0; block < simu::blocksPerPlane; block++)
		{
			for(unsigned page = 0; page < simu::pagesPerBlock; page++)
			{
				for(unsigned byte = 0; byte < simu::pageTotalSize; byte++)
				{
					stream.read(reinterpret_cast<char*>(
							&cell->planes[plane].
										blocks[block].
										pages[page].
										bytes[byte].word)
					             , 1);
				}
			}
		}
	}
}
