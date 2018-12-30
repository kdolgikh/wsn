/*
 * CH_Sense.c
 *
 *  Created on: 12/04/2015
 *      Author: KDolgikh
 */

#include <stdint.h>
#include <msp430.h>
#include "States.h"
#include "Radio.h"
#include "CH_TDMA_Assign.h"
#include "CH_Sense.h"
#include "sleep_timer.h"
#include "Message_Prep.h"
#include "States.h"
#include "pindefs.h"

extern uint8_t __MyHead;

static MessageType opcode = BS_Data;

Machine_State CH_Sense(uint8_t my_addr, uint8_t *children, uint8_t count) {

	struct Listen_Struct RX_Message;
	uint32_t Current_Time = 0;
	uint32_t End_Time = 0;
	uint32_t stop = 0;
	uint32_t listen_time = 0;
	uint8_t length = 1;
	uint8_t length_bs = 0;
	uint8_t i;
	uint8_t message = CH_Poll; //opcode
	uint8_t message_bs[6] = {0};
	int32_t temp[20];

	while (1) {																	// This function never returns
		Current_Time = Get_Time();					// Measure the time at the start of the TDMA loop
		End_Time = Current_Time + TDMA_SLEEP_CYCLES;		// Compute the next TDMA loop start time

		for (i = 0; i < count; i++) {				// Iterate through all the child nodes
			stop = Get_Time() + TDMA_TIME_SLOT;			// Compute the start time for the next time slot

			TDMA_Send(children[i], my_addr, &message, length);	// Poll the child node
			P1OUT ^= BIT0;						// Toggle the light

			listen_time = TDMA_TIME_SLOT >> 1;		// Listen for half of the TDMA time slot
			RX_Message = LBT_Listen(listen_time);

			if (RX_Message.Status == Message_Recieved) {			// Check if a message is recieved
				temp[i] = long_int_merge_temp(
						RX_Message.payload[TEMP_POS_LOWER],
						RX_Message.payload[TEMP_POS_MIDDLE_R],
						RX_Message.payload[TEMP_POS_MIDDLE_L],
						RX_Message.payload[TEMP_POS_UPPER]);
			} else {
				temp[i] = 0xDEADBEEF;		// Use dummy value if no message is recieved
			}

			Current_Time = Get_Time();		// Take current time
			Sleep_Timer(stop - Current_Time);	// Sleep until the start of the next time slot
		}

		// TX to base station
		for (i = 0; i < count; i++) {			// One message per child node
			length_bs = Message_Prep_BS(opcode, children[i], temp[i], &message_bs[0]);
			TDMA_Send(__MyHead, my_addr, &message_bs[0], length_bs);
			Sleep_Timer(200); // Delay for the correct printing on the PC monitor
		}

		Current_Time = Get_Time();		// Take time after sending all of the messages to the base station

		Sleep_Timer(End_Time - Current_Time);	// Sleep until the beginning of the next TDMA loop

	}

	return broken;	// If somehow a value is returned, return the broken value
}
