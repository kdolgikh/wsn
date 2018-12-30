/*
 * CH_Sense.h
 *
 *  Created on: 12/04/2015
 *      Author: KDolgikh
 */

#include <stdint.h>
#include "States.h"


#ifndef CH_SENSE_H_
#define CH_SENSE_H_


/**
 *\brief Sensing operation of the cluster head. Polls its child nodes and forwards the data to the base station
 *
 *@param my_addr The address of this node
 *@param *children Pointer to the array containing addresses of the nodes which reside in the current cluster
 *@param count Number of nodes in the cluster (number of elements in the array "children")
 *
 */

Machine_State CH_Sense(
		uint8_t my_addr,
		uint8_t *children,
		uint8_t count);

#endif /* CH_SENSE_H_ */
