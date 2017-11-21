/*
 * Copyright (c) 2016, 2017, German Aerospace Center (DLR)
 *
 * This file is part of the development version of OUTPOST.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Authors:
 * - 2016 - 2017, Pascal Pieper (DLR RY-AVS)
 */
// ----------------------------------------------------------------------------


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
	MramConfiguration mConfig;

	int handleRequest(char* answerBuf, functionRequest function, char *params) override;
public:
	Mram(unsigned int size);
	~Mram();
	unsigned char getByte(unsigned int address);
	void setByte(unsigned int address, unsigned char byte);

	std::ostream& serialize(std::ostream& stream);
	void deserialize(std::istream& stream);
};
