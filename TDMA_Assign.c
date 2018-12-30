/*
 * TDMA_Assign.c
 *
 *  Created on: 11/30/2015
 *      Author: KDolgikh
 */

#include "Radio.h"
#include "States.h"
#include "Message_Prep.h"
#include "sleep_timer.h"
#include "TDMA_Assign.h"
#include "pindefs.h"
#include <stdint.h>


Machine_State TDMA_Assign(uint32_t *cycles, uint8_t ch_addr) {
	volatile struct Listen_Struct RX_Message;

	RX_Message = LBT_Listen(0); // listen forever until the packet is received

	*cycles = long_int_merge(RX_Message.payload[CYCLES_POS_LOWER],
							 RX_Message.payload[CYCLES_POS_MIDDLE],
							 RX_Message.payload[CYCLES_POS_UPPER]);

	Radio_Sleep();

	Sleep_Timer(*cycles - GT);

	return Sensing;

}
