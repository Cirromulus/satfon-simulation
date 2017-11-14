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

#include "../simu/flashCell.hpp"

int main(int argc, char *argv[]){
	(void) argc;
	(void) argv;
	Failparam fp = {100, 20, 0, 0};
	FlashCell flash = FlashCell(fp);
	bool ok = true;
	srand (time(NULL));
	unsigned char buf[simu::pageDataSize];
	unsigned int i = 0;

	//prepare buffer...
	for(; i < simu::pageDataSize; i++){
		buf[i] = i;
	}
	i = 1;
	while(ok && i++){
		Nandaddress pos = {0, 0, 0};
		pos.plane = rand() % simu::planesPerCell;
		pos.block = rand() % simu::blocksPerPlane;
		pos.page  = rand() % simu::pagesPerBlock;
		//This really expects buffer to be of pageSize (not pageDataSize)
		if(i % 100 == 0){
			ok = flash.eraseBlock(pos.plane, pos.block) == 0;
			std::cout << "Erased block at " << pos.plane << "." << pos.block;
		}else{
			//it is allowed to overwrite Pages in Simulation...
			ok = flash.writePage(pos.plane, pos.block, pos.page, buf) == 0;
			std::cout << "Wrote page at " << pos.plane << "." << pos.block << "." << pos.page;
		}
		if(ok)
			std::cout << " successfully.\n";
		else
			std::cout << " with an error.\n";
	}
	std::cin.get();
}
