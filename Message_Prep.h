/*
 * Message_Prep.h
 *
 *  Created on: 11/30/2015
 *      Author: KDolgikh
 */

#include <stdint.h>

#ifndef MESSAGE_PREP_H_
#define MESSAGE_PREP_H_

/**
 *\brief Defines the message format and populates it with data for different machine states
 *
 *@param *ptr Pointer to the first element of the message
 */


#define OPCODE_POS 0 // position of the opcode field within the message

// Machine_State CH_TDMA_Assignment
#define CYCLES_POS_LOWER 1 // position of the lower byte of cycles field
#define CYCLES_POS_MIDDLE 2 // position of the second byte of cycles field
#define CYCLES_POS_UPPER 3 // position of the upper byte of cycles field
#define GT_POS_LOWER 4 // position of the lower byte of guard time field
#define GT_POS_MIDDLE 5 // position of the middle byte of guard time field
#define GT_POS_UPPER 6 // position of the upper byte of guard time field

// Machine_State CH_Sensing
#define TEMP_POS_LOWER 1
#define TEMP_POS_MIDDLE_R 2
#define TEMP_POS_MIDDLE_L 3
#define TEMP_POS_UPPER 4

#define BS_ND_ADDR_POS 1
#define BS_TEMP_POS_LOW 2
#define BS_TEMP_POS_MID_R 3
#define BS_TEMP_POS_MID_L 4
#define BS_TEMP_POS_UP 5

/**
 *\brief Functions Message_Prep define the message format and populates it with data for different machine states
 *
 *@param *ptr Pointer to the first element of the message
 */
uint8_t Message_Prep_CH_TDMA (
		uint8_t opcode,
		uint32_t cycles,
		//uint32_t gt,
		uint8_t *ptr);

uint8_t Message_Prep_CH_Sense (
		uint8_t opcode,
		uint8_t *ptr);

uint8_t Message_Prep_Sense (
		uint8_t opcode,
		int32_t temp,
		uint8_t *ptr);

uint8_t Message_Prep_BS (uint8_t opcode,
				 uint8_t node_addr,
				 int32_t temp,
				 uint8_t *ptr);

#endif /* MESSAGE_PREP_H_ */
