/*
 * Radio_LBT.c
 *
 *  Created on: Oct 22, 2015
 *      Author: Cody Gossel, kdolgikh
 */

#include "Radio.h"

#include "SPI_Library.h"
#include "CC110l.h"
#include "stdint.h"
#include "msp430.h"
#include "sleep_timer.h"
#include "Radio.h"
#include "SPI_Pins.h"
#include "pindefs.h"

LBT_Status LBT_Send(uint8_t dest_address, uint8_t source_address, uint8_t *message, uint8_t length)
{
	uint8_t status;
	uint8_t state;
	uint8_t FIFO_Space;
	uint8_t Old_GDO;
	uint8_t Old_MSP_RX_Port_IES;
	uint8_t Old_MSP_RX_Port_IE;
	uint8_t Old_MSP_RX_Port_Out;
	uint8_t Old_MSP_RX_Port_Dir;
	LBT_Status return_status;

	// Configure machine state
	SPI_Read(GDO_RX, &Old_GDO);			// Capture and save the old setting for the GDO pin
	Old_MSP_RX_Port_IES = MSP_RX_Port_IES;	// Save the old interrupt edge select value
	Old_MSP_RX_Port_IE = MSP_RX_Port_IE;	// Save the old interrupt enable value
	Old_MSP_RX_Port_Out = MSP_RX_Port_OUT;	// Save the old value of the GDO pin port
	Old_MSP_RX_Port_Dir = MSP_RX_Port_DIR;	// Save the old pin direction

	MSP_RX_Port_DIR &= ~MSP_RX_Pin;		// Set the GDO pin to input

	status = SPI_Send(GDO_RX, 0x06);	// Set the GDO to assert on preamble start and TX complete
	state = status & State_Bits;		// Mask off the state bits from the status byte

	if(state != SB_Idle) // If a TX was already in progress.
	{
		return_status = Radio_Busy;
		goto Cleanup;		// Yes. A Goddamn goto.
	}

	// Change to recieve mode to check for clear channel
	SPI_Strobe(SRX, Get_TX_FIFO); // Listen before transmit
	Sleep_Timer(328); // Sleep 10 ms
	SPI_Read_Status(PKTSTATUS, &status);	// Get packet status

	if(!(status & BIT4)) // Check for clear channel. If true then a carrier signal was recv'd and a collision occured.
	{
		return_status = Channel_Busy;	// shit!
		goto Cleanup;
	}

	SPI_Strobe(SIDLE, Get_TX_FIFO);		// stop the radio

	// Load the TX fifo
	SPI_Send(TXFIFO, length + 2);
	SPI_Send(TXFIFO, dest_address);
	SPI_Send(TXFIFO, source_address);
	status = SPI_Send_Burst(TXFIFO, message, length);	// Load the TX fifo
	FIFO_Space = status & FIFO_Bytes;		// Get the space left in the FIFO

	if(FIFO_Space == 0)	// FIFO space remaining of 0 means overflow
	{
		return_status = TX_Buffer_Overflow;
		goto Cleanup;
	}

	// Enable interrupt on falling edge
	MSP_RX_Port_IES |= MSP_RX_Pin;
	MSP_RX_Port_IE |= MSP_RX_Pin;

	SPI_Strobe(STX, Get_TX_FIFO); // Tell radio to transmit
	LPM3; // Sleep until TX is done, interrupt will wake up the

	return_status = Transmit_Success;

	// Clear the interrupt and set the GDO pin back to its old function here. n
	Cleanup:

	MSP_RX_Port_IFG &= ~MSP_RX_Pin;			// Clear interrupt flags before exit
	MSP_RX_Port_DIR = Old_MSP_RX_Port_Dir;	// Restore the old input/output setting
	MSP_RX_Port_IES = Old_MSP_RX_Port_IES;	// Restore the old interrupt edge select value
	MSP_RX_Port_IE = Old_MSP_RX_Port_IE;	// Restore the old interrupt enable value
	MSP_RX_Port_OUT = Old_MSP_RX_Port_Out;	// Restore the old port setting
	SPI_Send(GDO_RX, Old_GDO);				// Set the GDO pin back to its old function

	return return_status;
}

LBT_Status TDMA_Send(uint8_t dest_address, uint8_t source_address, uint8_t *message, uint8_t length)
{
	uint8_t status;
	uint8_t state;
	uint8_t FIFO_Space;
	uint8_t Old_GDO;
	uint8_t Old_MSP_RX_Port_IES;
	uint8_t Old_MSP_RX_Port_IE;
	uint8_t Old_MSP_RX_Port_Out;
	uint8_t Old_MSP_RX_Port_Dir;


	LBT_Status return_status;

	// Configure machine state
	SPI_Read(GDO_RX, &Old_GDO);			// Capture and save the old setting for the GDO pin
	Old_MSP_RX_Port_Dir = MSP_RX_Port_DIR;	// Save the old pin direction
	Old_MSP_RX_Port_IES = MSP_RX_Port_IES;	// Save the old interrupt edge select value
	Old_MSP_RX_Port_IE = MSP_RX_Port_IE;	// Save the old interrupt enable value
	Old_MSP_RX_Port_Out = MSP_RX_Port_OUT;	// Save the old value of the GDO pin port

	MSP_RX_Port_DIR &= ~MSP_RX_Pin;		// Set the GDO pin to input

	status = SPI_Send(GDO_RX, 0x06);	// Set the GDO to assert on preamble start and TX complete
	state = status & State_Bits;		// Mask off the state bits from the status byte

	if(state != SB_Idle) // If a TX was already in progress.
	{
		return_status = Radio_Busy;
		goto Cleanup;		// Yes. A Goddamn goto.
	}

	// Load the TX fifo
	SPI_Send(TXFIFO, length + 2);
	SPI_Send(TXFIFO, dest_address);
	SPI_Send(TXFIFO, source_address);
	status = SPI_Send_Burst(TXFIFO, message, length);	// Load the TX fifo
	FIFO_Space = status & FIFO_Bytes;		// Get the space left in the FIFO

	if(FIFO_Space == 0)	// FIFO space remaining of 0 means overflow
	{
		return_status = TX_Buffer_Overflow;
		goto Cleanup;
	}

	// Enable interrupt on falling edge
	MSP_RX_Port_IES |= MSP_RX_Pin;
	MSP_RX_Port_IE |= MSP_RX_Pin;

	SPI_Strobe(STX, Get_TX_FIFO); // Tell radio to transmit
	LPM3; // Sleep until TX is done, interrupt will wake up the

	return_status = Transmit_Success;

	// Clear the interrupt and set the GDO pin back to its old function here. n
	Cleanup:

	MSP_RX_Port_DIR = Old_MSP_RX_Port_Dir;	// Restore the old input/output setting
	MSP_RX_Port_IFG &= ~MSP_RX_Pin;			// Clear interrupt flags before exit
	MSP_RX_Port_IES = Old_MSP_RX_Port_IES;	// Restore the old interrupt edge select value
	MSP_RX_Port_IE = Old_MSP_RX_Port_IE;	// Restore the old interrupt enable value
	MSP_RX_Port_OUT = Old_MSP_RX_Port_Out;	// Restore the old port setting
	SPI_Send(GDO_RX, Old_GDO);				// Set the GDO pin back to its old function

	return return_status;
}

struct Listen_Struct LBT_Listen(uint32_t timeoutPeriod)
{
	uint8_t Old_GDO;
	uint8_t Old_MSP_RX_Port_IES;
	uint8_t Old_MSP_RX_Port_IE;
	uint8_t Old_MSP_RX_Port_Out;
	uint8_t Old_MSP_RX_Port_Dir;
	uint8_t timeout = 0;
	uint8_t status;
	uint8_t state;
	uint8_t buffer[64];
	LBT_Status return_status;
	struct Listen_Struct retVal;

	// Flush the RX FIFO
	SPI_Strobe(SFRX, Get_RX_FIFO);

	// Configure machine state
	SPI_Read(GDO_RX, &Old_GDO);			// Capture and save the old setting for the GDO pin
	Old_MSP_RX_Port_Dir = MSP_RX_Port_DIR;	// Save the old pin direction
	Old_MSP_RX_Port_IES = MSP_RX_Port_IES;	// Save the old interrupt edge select value
	Old_MSP_RX_Port_IE = MSP_RX_Port_IE;	// Save the old interrupt enable value
	Old_MSP_RX_Port_Out = MSP_RX_Port_OUT;	// Save the old output value

	MSP_RX_Port_OUT &= ~MSP_RX_Pin;			// Set the GDO pin to low
	MSP_RX_Port_DIR &= ~MSP_RX_Pin;			// Set the GDO pin to input


	status = SPI_Send(GDO_RX, 0x07);	// Set the GDO to assert on pkt recieve with crc OK
	state = status & State_Bits;		// Mask off the state bits from the status byte

	if(state != SB_Idle) // If a radio is already busy
	{
		return_status = Radio_Busy;
		goto Cleanup;		// Yes. A Goddamn goto.
	}

	// Set GDO pin to trigger on rising edge for PKT RX
	MSP_RX_Port_IES &= ~MSP_RX_Pin;	// Set for rising edge
	MSP_RX_Port_IFG &= ~MSP_RX_Pin;	// Clear interrupts
	MSP_RX_Port_IE |= MSP_RX_Pin;	// Enable

	SPI_Strobe(SRX, Get_RX_FIFO); // Set radio to listen

	if (timeoutPeriod == 0) {
		LPM3;
	}
	else {
		timeout = Sleep_Timer(timeoutPeriod);
	}

	if(timeout)
	{
		return_status = RX_Timeout;
		goto Cleanup;
	}

	SPI_Read_Status(RXBYTES, &status);	// Get packet status

	if(status & OverFlow) // If the buffer is overflowed dump it and don't read the values
	{
		return_status = RX_Buffer_Overflow;
		SPI_Strobe(SFRX, Get_RX_FIFO);
		goto Cleanup;
	}

	// Read the FIFO buffer into the out variable. IF there is no overflow then contents of the RXBYTES register is just the
	// number of bytes in the RX FIFO.
	SPI_Read_Burst(RXFIFO, buffer, status);
	return_status = Message_Recieved;

	uint8_t i;
	for(i = 0; i < status - 2; i++)
	{
		retVal.payload[i] = buffer[i + 3];
	}


	Cleanup:

	retVal.length = buffer[0] - 2;
	retVal.address = buffer[2];
	retVal.signal = buffer[status - 2];
	retVal.Status = return_status;

	// Restore old machine state

	SPI_Send(GDO_RX, Old_GDO);				// Set the GDO pin back to its old function
	SPI_Strobe(SIDLE, Get_RX_FIFO);
	MSP_RX_Port_DIR = Old_MSP_RX_Port_Dir;	// Restore the old input/output setting
	MSP_RX_Port_IE = Old_MSP_RX_Port_IE;	// Restore the old interrupt enable value
	MSP_RX_Port_IFG &= ~MSP_RX_Pin;			// Clear interrupt flags before exit
	MSP_RX_Port_IES = Old_MSP_RX_Port_IES;	// Restore the old interrupt edge select value
	MSP_RX_Port_OUT = Old_MSP_RX_Port_Out;	// Restore the old port setting

	return retVal;
}

void Radio_Sleep()
{
	SPI_Strobe(SPWD, Get_RX_FIFO);
}

/*
* Created on: 11/30/2015
*      Author: KDolgikh
*/

uint8_t int_divide (uint16_t two_byte_data, uint8_t *ptr)
{
	uint8_t length = 0;

	*ptr = two_byte_data & 0xFF; 	// lower 8 bit
	*(ptr+1) = two_byte_data >> 8; 	// upper 8 bit, shifts by 8 bit to the rignt
	length = 2;

	return length;

}

uint16_t int_merge (uint8_t lower, uint8_t upper)
{
	uint16_t temp1 = 0;
	uint16_t temp2 = 0;

	temp1 = upper << 8; 	// places upper 8 bit to their final position by shifting to the left
	temp2 = temp1 | lower;	// merges lower and upper bits into an uint16_t
	return temp2;
}


uint8_t long_int_divide (uint32_t four_byte_data, uint8_t *ptr)
{
	uint8_t length = 0;

	*ptr = four_byte_data & 0xFF; 	// lower 8 bit
	*(ptr+1) = (four_byte_data >> 8) & 0xFF; // middle 8 bit
	*(ptr+2) = (four_byte_data >> 16) & 0xFF; // upper 8 bit
	length = 3;

	return length;
}


uint32_t long_int_merge (uint8_t lower, uint8_t middle, uint8_t upper)
{
	uint32_t temp1 = upper;
	uint32_t temp2 = middle;
	uint32_t temp3 = 0;

	temp3 = (temp1 << 16) | (temp2 << 8) | lower;

	return temp3;
}


uint8_t long_int_divide_temp (int32_t four_byte_data, uint8_t *ptr)
{
	uint8_t length = 0;

	*ptr = four_byte_data & 0xFF; 	// lower 8 bit
	*(ptr+1) = (four_byte_data >> 8) & 0xFF; // middle_right
	*(ptr+2) = (four_byte_data >> 16) & 0xFF; // middle_left
	*(ptr+3) = (four_byte_data >> 24) & 0xFF; // upper 8 bit
	length = 4;

	return length;
}


int32_t long_int_merge_temp (uint8_t lower, uint8_t middle_r, uint8_t middle_l , uint8_t upper)
{
	int32_t temp1 = upper;
	int32_t temp2 = middle_l;
	int32_t temp3 = middle_r;
	int32_t temp4 = 0;

	temp4 = (temp1 << 24) | (temp2 << 16) | (temp3 << 8) | lower;

	return temp4;
}
