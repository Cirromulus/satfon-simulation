/*
 * types.hpp
 *
 *  Created on: 05.07.2016
 *      Author: urinator
 */

#ifndef SIMU_TYPES_HPP_
#define SIMU_TYPES_HPP_

#include <simu/config.hpp>

struct AccessValues{
	unsigned int times_read;
	unsigned int times_written;
	unsigned int times_reset;
};

struct Failpoint{
	unsigned int erases;
	unsigned int rad;
};

struct Failparam{
	unsigned int mean_erases;
	unsigned int erase_deviation;
	unsigned int mean_idose;
	unsigned int idose_deviation;
};

struct Nandaddress{
	unsigned int plane;
	unsigned int block;
	unsigned int page;
};



#endif /* SIMU_TYPES_HPP_ */
