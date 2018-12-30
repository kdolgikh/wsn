/*
 * pinDefs.h
 *
 *  Created on: Oct 14, 2015
 *      Author: cgoss
 */

#ifndef PINDEFS_H_
#define PINDEFS_H_
#include <msp430.h>
#include "sleep_timer.h"


/*************************** Begin Value line pindefs ******************************/
#define True 1
#define False 0

#ifdef __MSP430G2553__
// Registers
#define LED1Reg P1OUT
#define LED2Reg P1OUT

#define LED1Dir P1DIR

// Pins
#define LED1 BIT0
#define LED2 BIT6


#endif				// Value line pindefs end


#endif /* PINDEFS_H_ */
