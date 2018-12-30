/*
 * CH_TDMA_Assign.h
 *
 *  Created on: 11/30/2015
 *      Author: KDolgikh
 */

#include <stdint.h>
#include "States.h"
#include "Radio.h"
#include "Message_Prep.h"


#ifndef CH_TDMA_ASSIGN_H_
#define CH_TDMA_ASSIGN_H_

/**  The duration of a TDMA loop */
#define TDMA_SLEEP_CYCLES 200000

/** The duration of a TDMA time slot */
#define TDMA_TIME_SLOT 16384

/**
 *\brief Performs TDMA schedule dissemination among child nodes within the cluster.
 *
 *@param my_addr Address of a cluster head
 *@param *children Pointer to the array containing addresses of the nodes which reside in the current cluster
 *@param count Number of nodes in the cluster (number of elements in the array "children")
 *
 */





Machine_State CH_TDMA_Assign (
		uint8_t my_addr,
		uint8_t *children,
		uint8_t count);

#endif /* CH_TDMA_ASSIGN_H_ */
