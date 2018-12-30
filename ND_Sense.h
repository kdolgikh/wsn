/*
 * ND_Sense.h
 *
 *  Created on: 12/05/2015
 *      Author: KDolgikh
 */

#include <stdint.h>
#include "States.h"

#ifndef ND_SENSE_H_
#define ND_SENSE_H_

/**
 *\brief Node performs sensing operation
 *
 *@param my_addr Node address
 *@param ch_addr Cluster head address
 *@param tdma_sleep_cycles Number of cycles node is asleep in TDMA mode
 *@param gt Guard time
 *@param *message Ptr to a message
 *@param *rx_flag) Ptr to the flag indicating reception of the message
 */

Machine_State ND_Sense(
		uint32_t cycles,
		uint8_t my_addr,
		uint8_t ch_addr);

#endif /* ND_SENSE_H_ */
