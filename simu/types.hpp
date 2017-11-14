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
