/*
 * mram.cpp
 *
 *  Created on: 10.09.2017
 *      Author: urinator
 */

#include "mram.hpp"

Mram::Mram(unsigned int size) : mSize(size){
	mData = new unsigned char[size];
}
Mram::~Mram(){
	delete[] mData;
}

unsigned char Mram::getByte(unsigned int address){
	if(address > mSize){
		printf("MRAM read access out of bounds (%u of %u)\n", address, mSize);
		return 0;
	}
	return mData[address];
}
void Mram::setByte(unsigned int address, unsigned char byte){
	if(address > mSize){
		printf("MRAM write access out of bounds (%u of %u)\n", address, mSize);
		return;
	}
	mData[address] = byte;
}
