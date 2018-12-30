/*
 * CH_TDMA_Assign.c
 *
 *  Created on: 11/30/2015
 *      Author: KDolgikh
 */

#include <stdint.h>
#include <msp430.h>
#include "States.h"
#include "Radio.h"
#include "sleep_timer.h"
#include "CH_TDMA_Assign.h"
#include "Message_Prep.h"
#include "pindefs.h"


Machine_State CH_TDMA_Assign (
		uint8_t my_addr,
		uint8_t *children,
		uint8_t count)
{
	uint32_t start = 0; // number of cycles when TDMA assign starts
	uint32_t stop = 0; 	// number of cycles when TDMA assign stops
	uint8_t i;
	uint8_t length = 0;
	uint8_t message[4];

	length = Message_Prep_CH_TDMA(TDMA, TDMA_SLEEP_CYCLES, &message[0]);

	start = Get_Time(); // Beginning of time slot
	stop = start + TDMA_SLEEP_CYCLES;	// Time the slot should end

	// Iterate through the child nodes
	for (i = 0; i < count; i++) {
		TDMA_Send(children[i], my_addr, &message[0], length);
		P1OUT ^= BIT0;
		Sleep_Timer(TDMA_TIME_SLOT);
	}

	start = Get_Time();	// Check when slot assignment has ended

	Sleep_Timer(stop - start);	// Sleep for the difference

	return CH_Sensing;
}
