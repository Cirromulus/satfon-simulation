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
#include <functional>

static constexpr unsigned int mramDebugServerStartPort = 3084;
static constexpr unsigned int mramDebugServerBlockwidth = 512;

struct MramConfiguration{
	unsigned int blockWidth;
	unsigned int size;
};

class Mram : private DebugServer{
	uint32_t mSize;
	uint8_t *mData;
	MramConfiguration mConfig;
	std::function<void()> notifyChange = nullptr;
	uint64_t readAccesses  = 0;
	uint64_t writeAccesses = 0;

	int handleRequest(char* answerBuf, functionRequest function, char *params) override;
public:
	Mram(uint32_t size);
	~Mram();
    void
    registerOnchangeFunction(std::function<void()> fun);

	uint8_t
	getByte(uint32_t address);
	void
	setByte(uint32_t address, uint8_t byte);

    void
    writeBlock(uint32_t address, const void* buf, uint16_t length);
    void
    readBlock(uint32_t address, void* buf, uint16_t length);


    uint64_t
	getNumberOfReadAccesses();
    uint64_t
	getNumberOfWriteAccesses();
    uint64_t
	getElapsedTimeUsec();
	std::ostream&
	serialize(std::ostream& stream);
	void
	deserialize(std::istream& stream);
};
