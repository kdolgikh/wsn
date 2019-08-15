/**
 * \file main.c
 * \brief Main control code and interrupt definitions for sensor node
 *
 * Initializes both the MSP430 controller and the attached CC110l radio.
 * After completing the initialization process the controller goes into LPM3 and waits.
 */

#include <msp430.h> // Base header files
#include <stdint.h> // pull in standard datatypes
#include "pinDefs.h" // Pin defines
#include "CC110l.h" // Literals for helping with the radio
#include "MSP_Init.h" // Code to set initial board state
#include "Radio.h"
#include "SPI_Library.h" // SPI control for the radio
#include "sleep_timer.h" // Sleep code for controller
#include "states.h" // Enum for the possible machine states.
#include "SPI_Pins.h"
#include "States.h"
#include "Message_Prep.h"
#include "CH_TDMA_Assign.h"
#include "TDMA_Assign.h"
#include "CH_Sense.h"
#include "ND_Sense.h"

// Globals
volatile uint16_t Timer_Rollover_Count = 0; // check if leave uint16_t
volatile uint8_t Break_Sleep = False;
uint8_t _MyAddress = 0x05; // user defined address for each node

uint8_t _ChildCount;
uint8_t _ChildList[20];

uint8_t __MyHead;
Machine_State __State = Waiting_For_Start;

uint32_t curr_time = 0;
uint32_t start = 0;
int32_t GT_Adjust = 0;

uint32_t tdma_sleep_cycles = 0;

/**
 * \brief Main control sequence for sensor node
 * @return Constant 0, but it has nowhere to go.
 */

int main(void) {
	Board_Init();
	Timer_Init();
	SPI_Init(); // Start SPI
	Radio_Init(); // Prep the radio

	Radio_Sleep();

	LPM3;

	SPI_Send(ADDR, _MyAddress);	// Lode the nodes address into the radio for packet filtering
	SPI_Send(PKTCTRL1, 0b00001111);	// Enable address filtering with 0xFF and 0x00 as broadcast

	P1DIR |= LED1;	// Set LED to output

	while (True)
	{

		if (__State == CH_TDMA_Assignment) {
			__State = CH_TDMA_Assign(_MyAddress, _ChildList, _ChildCount);
		}

		if (__State == CH_Sensing) {
			__State = CH_Sense(_MyAddress, _ChildList, _ChildCount);
		}

		if (__State == TDMA_Assignment) {
			__State = TDMA_Assign(&tdma_sleep_cycles, __MyHead);
		}

		if (__State == Sensing) {
			__State = ND_Sense(tdma_sleep_cycles, _MyAddress, __MyHead);
		}

		if (__State == CH_Sensing) {
			__State = CH_Sense(_MyAddress, _ChildList, _ChildCount);
		}
		if (__State == broken) {// Should never hit this step, infinite loop for debugging purposes
			while (1)
				;
		}
	}
}

/**
 * \brief Interrupt service routine for slow timer
 */
#pragma vector=TIMER0_A1_VECTOR
__interrupt void TimerA_0_ISR(void)
{
		switch (TA0IV)
		{
		case (TA0IV_TAIFG):
			Timer_Rollover_Count++;
			break;
		case (TA0IV_TACCR1):
			LPM3_EXIT; // Wake up if CCR1 is hit, used for sleeping function
			break;
		case (TA0IV_TACCR2):
			break;
		default:
			break;
		}
}

/**
 * \brief Interrupt service routine for packet received pin from CC100l
 */
#pragma vector=PORT1_VECTOR
__interrupt void MSP_RX_ISR (void)
{
	if (__State == Sensing)
	{
				curr_time = Get_Time();
				GT_Adjust = curr_time - start - GT;
				if (GT_Adjust > (int32_t)tdma_sleep_cycles)
					GT_Adjust = GT;
		Break_Sleep = True;
		LPM3_EXIT; // Wake up on interrupt
	}
}


#pragma vector=ADC10_VECTOR
__interrupt void ADC_ISR (void)
{
		ADC10CTL0 &= ~ADC10IFG; // Clear flag
		LPM0_EXIT;
}
