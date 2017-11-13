/*
 * example.cpp
 *
 *  Created on: 12.01.2017
 *      Author: Pascal Pieper
 */
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
