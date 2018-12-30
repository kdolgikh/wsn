/*
 * sleep_timer.h
 *
 *  Created on: 11/17/15
 *      Author: kdolgikh
 */
/**
 * \file sleep_timer.h
 * \brief Sleep library for the MSP430
 *
 */

#ifndef SLEEP_TIMER_H_
#define SLEEP_TIMER_H_
#define _Second 32768

#include <stdint.h>

/**
 * Puts the MCU to sleep for the specified number of ACLK rollovers and cycles.
 *
 * @param cycles	The number of cycles to sleep for, in terms of ACLK ticks
 * @return Returns 0 if the sleep is broken due to radio action, 1 if it is not.
 */
uint8_t Sleep_Timer (uint32_t cycles);
uint32_t Get_Time();

#endif /* SLEEP_TIMER_H_ */
