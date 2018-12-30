/*
 * TDMA_Assign.h
 *
 *  Created on: 11/30/2015
 *      Author: KDolgikh
 */

#include <stdint.h>
#include "States.h"

#ifndef TDMA_ASSIGN_H_
#define TDMA_ASSIGN_H_

#define GT 166 // Guard time of 5 ms

/**
 *\brief Node receives a tdma schedule. Opcode check is not needed.
 *
 *@param *cycels Ptr to the tdma sleep cycles
 */

Machine_State TDMA_Assign (uint32_t *cycles, uint8_t ch_addr);

#endif /* TDMA_ASSIGN_H_ */
