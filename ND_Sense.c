/*
 * ND_Sense.c
 *
 *  Created on: 12/05/2015
 *      Author: KDolgikh
 */

#include "Radio.h"
#include "States.h"
#include "Message_Prep.h"
#include "sleep_timer.h"
#include "pinDefs.h"

extern uint32_t curr_time;
extern int32_t GT_Adjust;
extern uint32_t start;

static MessageType opcode = ND_Data;

Machine_State ND_Sense(
					uint32_t cycles,
					uint8_t my_addr,
					uint8_t ch_addr)
{
	struct Listen_Struct RX_Message;
	uint8_t message[5];
	int32_t temp = 0;
	uint8_t length = 0;
	uint32_t stop = 0;


	while (1) {

		LED1Reg |= LED1;
		start = Get_Time();

		RX_Message = LBT_Listen(0);

		if (RX_Message.Status == Message_Recieved
				&& RX_Message.payload[OPCODE_POS] == CH_Poll) {
			LED1Reg &= ~LED1;

			temp = 100;
			length = Message_Prep_Sense(opcode, temp, &message[0]);
			TDMA_Send(ch_addr, my_addr, &message[0], length);
			Radio_Sleep();

			stop = Get_Time();
			Sleep_Timer(cycles - (stop - start) + GT_Adjust);
		}
	}

	return broken;
}
