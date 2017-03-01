/*
 * example.cpp
 *
 *  Created on: 12.01.2017
 *      Author: urinator
 */
#include <stdio.h>
#include <stdlib.h>	//Random
#include <time.h>   //time

#include "../flashCell.h"

int main(int argc, char *argv[]){
	FAILPARAM fp = {100, 20, 0, 0};
	FlashCell flash = FlashCell(fp);
	bool ok = true;
	srand (time(NULL));
	unsigned char buf[flash.pageSize];
	unsigned int i = 0;

	//prepare buffer...
	for(; i < flash.pageSize; i++){
		buf[i] = i;
	}
	i = 1;
	while(ok && i++){
		NANDADRESS pos = {0};
		pos.plane = rand() % flash.cellSize;	//A bit confusing names...
		pos.block = rand() % flash.planeSize;
		pos.page  = rand() % flash.blockSize;
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
