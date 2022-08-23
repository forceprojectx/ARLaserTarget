/**
* @brief      Interrupt Functions and variables
*
* @copyright  Copyright (c) 2022 Bebop Arms. All rights reserved.
*
* @details    define interrupts here in order to reduce clutter in main.cpp
*
**/

#pragma once
#ifndef INTERRUPTS_H
#define INTERRUPTS_H

/************************************************************************/
/*                           Include section                            */
/************************************************************************/
#include <msp430.h>
#include <stdint.h>

/************************************************************************/
/*                         #define declarations                         */
/************************************************************************/

/************************************************************************/
/*                         Forward declarations                         */
/************************************************************************/

/************************************************************************/
/*                     Data structures declarations                     */
/************************************************************************/

/************************************************************************/
/*                         Classes declarations                         */
/************************************************************************/

class Interrupts
{
public:

	// Frame Rate Interrupt counter
	volatile static uint32_t FrameInterruptCount;
	// do we need to transition to the next led animation frame
	volatile static bool LED_State;
	// default ADC value
	volatile static int calibratedADC;
};

/************************************************************************/
/*                         Routine declarations                         */
/************************************************************************/



#endif // !INTERRUPTS_H
