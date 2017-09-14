/*
 * example.cpp
 *
 *  Created on: 12.01.2017
 *      Author: urinator
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
	cin.get();
	while(true){
		mram.setByte(rand() % mramSize, rand() % 0x100);
	}
}
