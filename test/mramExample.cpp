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

#include <stdio.h>
#include <stdlib.h>	//Random
#include <time.h>   //time
#include <iostream>
#include <unistd.h>

#include "../simu/mram.hpp"

using namespace std;

int main(int argc, char *argv[]){
	unsigned int mramSize = 4096 * 512;
	Mram mram(mramSize);
	cout << "Inited MRAM." << endl;
	mram.setByte(0, 0xAA);
	mram.setByte(mramSize - 1, 0xAA);
	cin.get();
	cout << "Inserting random bits" << endl;
	while(true){
		unsigned int address = rand() % mramSize;
		unsigned char value = rand() % 0x100;
		printf("%02X at %u\n", value & 0xFF, address);
		mram.setByte(address, value);
	}
}
