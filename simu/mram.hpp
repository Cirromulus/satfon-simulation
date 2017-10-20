/*
 * mram.hpp
 *
 *  Created on: 10.09.2017
 *      Author: urinator
 */

#pragma once

#include "debugServer.hpp"
#include <simu/config.hpp>

static constexpr unsigned int mramDebugServerStartPort = 3084;
static constexpr unsigned int mramDebugServerBlockwidth = 512;

struct MramConfiguration{
	unsigned int blockWidth;
	unsigned int size;
};

class Mram : private DebugServer{
	unsigned int mSize;
	unsigned char *mData;
	MramConfiguration config;

	int handleRequest(char* answerBuf, functionRequest function, char *params) override;
public:
	Mram(unsigned int size);
	~Mram();
	unsigned char getByte(unsigned int address);
	void setByte(unsigned int address, unsigned char byte);

	std::ostream& serialize(std::ostream& stream);
	void deserialize(std::istream& stream);
};
