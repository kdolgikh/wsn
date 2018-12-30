/*
 * SPI_Pins.h
 *
 *  Created on: Oct 20, 2015
 *      Author: cgoss
 */

#include "CC110l.h"

#ifndef SPI_PINS_H_
#define SPI_PINS_H_

/*************************** Begin Value line pindefs ******************************/
#ifdef __MSP430G2553__
// SPI Pins on MSP430
#define SOMI BIT6
#define SIMO BIT7
#define SPCLK BIT5
#define CS BIT4

// SPI Registers
#define Port_Reg_Sel P1SEL 				// Port function select 1
#define Port_Reg_Sel2 P1SEL2 			// Port function select 2
#define Port_Reg_Dir P1DIR				// Port direction select
#define Port_In P1IN					// Input values for SPI port
#define CS_Register P1OUT				// Chip select for the slave device
#define CS_RegisterDir P1DIR			// Direction register for chip select pin
#define USCI_Control_Reg0 UCB0CTL0 		// USCI control reg 0
#define USCI_Control_Reg1 UCB0CTL1		// USCI control reg 1
#define USCI_Modulation_Upper UCB0BR1	// Upper byte of modulation value
#define USCI_Modulation_Lower UCB0BR0	// Lower byte of modulation vale
#define USCI_Status_Reg UCB0STAT		// Status register
#define USCI_Interrupt_Flags UC0IFG		// Interrupt flag register
#define USCI_TX_Reg UCB0TXBUF			// Transmit buffer
#define USCI_RX_Reg UCB0RXBUF			// Receive buffer

// Register masks
#define USCI_RX_Flag UCB0RXIFG			// Mask for RX complete
#define USCI_TX_Flag UCB0TXIFG			// Mask for TX complete

// GDO Ports on MSP430
#define MSP_RX_Port_DIR		P1DIR	// Direction register
#define MSP_RX_Port_OUT		P1OUT	// Out value register
#define MSP_RX_Port_REN		P1REN	// Resistor enable register
#define MSP_RX_Port_IN		P1IN	// In value register
#endif
/*************************** End Value line pindefs ******************************/

#endif /* SPI_PINS_H_ */
