/*
 * mram.hpp
 *
 *  Created on: 10.09.2017
 *      Author: urinator
 */

#pragma once

class Mram{
	unsigned int mSize;
	unsigned char *mData;
public:
	Mram(unsigned int size);
	~Mram();
	unsigned char getByte(unsigned int address);
	void setByte(unsigned int address, unsigned char byte);
};
