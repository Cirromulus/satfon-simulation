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

#include <atomic>
#include <thread>


enum functionRequest{
	F_PING,
	F_GETCONFIG,
	F_GETVALUE,
	F_GETACCESSVALUES,
	F_GETRADIATIONDOSE,
	F_GETFAILPOINT,
	F_WASBITFLIPPED,
	F_GETLATCHMASK
};

static constexpr unsigned int maxInstancesListening = 5;

class DebugServer{
	int serverSock = 0;
	unsigned int mPageWidth;
	unsigned int mStartPort;
	unsigned int mRequestSize;
	std::thread serverListenerThread;
	std::atomic<bool> stop;

	bool createServer();
	void serverListener();
	virtual int handleRequest(char* answerBuf, functionRequest function, char *params) = 0;
protected:
	void stopServer();
public:
	DebugServer(unsigned int pageWidth = 528,
				unsigned int startPort = 2084,
				unsigned int requestSize = sizeof(unsigned int) * 3);
	virtual ~DebugServer();
};
