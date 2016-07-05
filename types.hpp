/*
 * types.hpp
 *
 *  Created on: 05.07.2016
 *      Author: urinator
 */

#ifndef SIMU_TYPES_HPP_
#define SIMU_TYPES_HPP_


#define DATA_TYPE unsigned char
#define PAGE_DATA 512
#define PAGE_AUX (PAGE_DATA / 32)
#define PAGE_SIZE (PAGE_DATA + PAGE_AUX)
#define BLOCK_SIZE 64	//128
#define PLANE_SIZE 4	//8
#define CELL_SIZE 4		//4

#define TID_FLIP_START_PERCENT 0.85

struct ACCESS_VALUES{
	unsigned int times_read;
	unsigned int times_written;
	unsigned int times_reset;
};

struct FAILPOINT{
	unsigned int erases;
	unsigned int rad;
};

struct FAILPARAM{
	unsigned int mean_erases;
	unsigned int erase_deviation;
	unsigned int mean_idose;
	unsigned int idose_deviation;
};

struct NANDADRESS{
	unsigned int plane;
	unsigned int block;
	unsigned int page;
};



#endif /* SIMU_TYPES_HPP_ */
