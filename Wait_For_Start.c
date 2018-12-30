/*
 * Wait_For_Start.c
 *
 *  Created on: Nov 23, 2015
 *      Author: cgoss
 */

#include <stdint.h>
#include "States.h"
#include "Radio.h"
#include "pindefs.h"
#include "states.h"
#include "sleep_timer.h"

Machine_State Wait_For_Start()
{
	struct Listen_Struct message_status;
	uint8_t Got_Ok = False;
	uint8_t i;
	while(!Got_Ok)
	{
		LED1Reg ^= LED1;

		message_status = LBT_Listen(16384); // Listen for 1/2 seconds
		if(message_status.Status == Message_Recieved)
		{
			if(message_status.payload[0] == ClusterStart)
			{
				Got_Ok = True;
			}
		}
	}

	// Leave the loop when we got the start signal

	LED1Reg |= LED1;	// Durn LED ON
	for(i = 0; i < 20; i++)
	{
		LED1Reg ^= LED1;
		Sleep_Timer(800);
	}

	LED1Reg &= LED1;

	return Clustering;
}
