/*
 * Message_Prep.c
 *
 *  Created on: 11/30/2015
 *      Author: KDolgikh
 */


#include <stdint.h>
#include "Message_Prep.h"
#include "Radio.h"
#include "States.h"
#include "Message_Prep.h"
#include "CH_TDMA_Assign.h"


uint8_t Message_Prep_CH_TDMA (
		uint8_t opcode,
		uint32_t cycles,
		uint8_t *ptr)
{
	uint8_t length = 0; // payload length
	uint8_t lgth = 0;

	ptr[OPCODE_POS] = opcode; // 0
	lgth = long_int_divide(cycles, &ptr[CYCLES_POS_LOWER]);
	length = lgth + 1;

	return length;
}

uint8_t Message_Prep_CH_Sense (uint8_t opcode, uint8_t *ptr)
{
	uint8_t length = 0; // payload length

	ptr[OPCODE_POS] = opcode;
	length = 1;

	return length;
}

uint8_t Message_Prep_Sense (uint8_t opcode, int32_t temp, uint8_t *ptr)
{
	uint8_t length = 0; // payload length
	uint8_t lgth = 0;

	ptr[OPCODE_POS] = opcode;
	lgth = long_int_divide_temp(temp, &ptr[TEMP_POS_LOWER]);
	length = lgth + 1;

	return length;
}


uint8_t Message_Prep_BS (uint8_t opcode, uint8_t node_addr, int32_t temp, uint8_t *ptr)
{
uint8_t length = 0; // payload length
uint8_t lgth = 0;

ptr[OPCODE_POS] = opcode;
ptr[BS_ND_ADDR_POS] = node_addr;
lgth = long_int_divide_temp(temp, &ptr[BS_TEMP_POS_LOW]);
length = lgth + 2;

return length;
}
