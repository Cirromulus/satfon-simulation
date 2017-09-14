/*
 * debugServer.hpp
 *
 *  Created on: 10.09.2017
 *      Author: urinator
 */

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
	std::atomic<bool> stop;
	std::thread serverListenerThread;

	bool createServer();
	void serverListener();
	virtual int handleRequest(char* answerBuf, functionRequest function, char *params) = 0;
public:
	DebugServer(unsigned int pageWidth = 528,
				unsigned int startPort = 2084,
				unsigned int requestSize = sizeof(unsigned int) * 3);
	virtual ~DebugServer();
};
