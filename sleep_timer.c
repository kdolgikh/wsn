/*
 * sleep_timer.c
 *
 *  Created on: 11/20/2015
 *      Author: kdolgikh
 */

#include <MSP430.h>
#include <stdint.h>
#include "pinDefs.h"

volatile extern uint8_t Break_Sleep;
extern uint16_t Timer_Rollover_Count;

uint8_t Sleep_Timer(uint32_t cycles) {
	uint16_t i;
	uint32_t rollovers;
	uint16_t Sleep_Cycles;
	uint8_t retVal = 1;

	Break_Sleep = False;
	rollovers = cycles >> 16;	// Upper word is number of rollovers
	Sleep_Cycles = cycles & 0xFFFF;	// Lower word is number of cycles to run

	TA0CCTL1 &= ~CCIFG;		// Clear existing flags
	TA0CCTL1 |= CCIE;		// Enable interrupts
	TA0CCR1 = TA0R + Sleep_Cycles; // Set the CCR to wait the number of cycles given

	if(cycles != 0)
	{
		LPM3;
	}

	for (i = 0; i < rollovers && !Break_Sleep; i++) {	// Sleeps for i rollovers or until the Break_Sleep variable is set true
		LPM3;
	}

	if(Break_Sleep)
	{
		retVal = 0;
	}
	TA0CCTL1 &= ~CCIE;
	return retVal;
}

uint32_t Get_Time()
{
	uint32_t time;
	TA0CCTL1 &= ~BIT5 + BIT4;	// Pause the clock
	time = Timer_Rollover_Count; 	// Upper word is the rollover count
	time = time << 16;
	time |= TA0R;		// Lower word is the timer register value
	TA0CCTL1 |= MC_2;	// Restart the timer
	return time;
}
