/*
 * RandomGen.c
 *
 *  Created on: Dec 2, 2015
 *      Author: Brandon
 */

#include"MSP430g2553.h"
#include"CC110l.h"
#include"SPI_Library.h"
#include"SPI_Pins.h"

#include <msp430.h>



/**
 * Random number generator.
 *
 * It is recommended that this function is run during setup, before any clocks
 * or timers have been set and before interrupts have been enabled. While it
 * does restore all used control registers to their original values, it also
 * expects that nothing interesting will happen when it modifies them.
 *
 * Algorithm from TI SLAA338:
 * http://www.ti.com/sc/docs/psheets/abstract/apps/slaa338.htm
 *
 * @return 16 random bits generated from a hardware source.
 */
unsigned int rand(void) {
	int i, j;
	unsigned int result = 0;

	/* Save state */
	unsigned int BCSCTL3_old = BCSCTL3;
	unsigned int TACCTL0_old = TACCTL0;
	unsigned int TACTL_old = TACTL;

	/* Halt timer */
	TACTL = 0x0;

	/* Set up timer */
	BCSCTL3 = (~LFXT1S_3 & BCSCTL3) | LFXT1S_2; // Source ACLK from VLO
	TACCTL0 = CAP | CM_1 | CCIS_1;            // Capture mode, positive edge
	TACTL = TASSEL_2 | MC_2;                  // SMCLK, continuous up

	/* Generate bits */
	for (i = 0; i < 16; i++) {
		unsigned int ones = 0;

		for (j = 0; j < 5; j++) {
			while (!(CCIFG & TACCTL0));       // Wait for interrupt

			TACCTL0 &= ~CCIFG;                // Clear interrupt

			if (1 & TACCR0)                   // If LSb set, count it
				ones++;
		}

		result >>= 1;                         // Save previous bits

		if (ones >= 3)                        // Best out of 5
			result |= 0x8000;                 // Set MSb
	}

	/* Restore state */
	BCSCTL3 = BCSCTL3_old;
	TACCTL0 = TACCTL0_old;
	TACTL = TACTL_old;

	return result;
}


uint8_t Add_Gen(unsigned int value1, unsigned int value2)
{
	uint8_t address;
	value1 = value1 % 256;
	value2 = value2 % value1;
	address = value2 + 1;


	return address;
}


