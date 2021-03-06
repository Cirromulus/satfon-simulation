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

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

class DebugClient{
	int clientSock = 0;
	sockaddr_in rcpt;
	unsigned int rcptSize;
	unsigned int mSendBufSize;
	char* sendBuf;
public:
	DebugClient(unsigned int port, unsigned int sendBufSize);
	virtual ~DebugClient();
	bool initClient(unsigned int port);
	void sendRequest(functionRequest function, char* params);
	int recEverything(void* buf, unsigned int size);
};
