/**
* @brief      Light Sensor Setup and control functions
*
* @copyright  Copyright (c) 2022 Bebop Arms. All rights reserved.
*
* @details    Uses ADC along with a phototransistor and voltage divider circuit to implement a light sensor circuit.
*               The voltage across the divider is measured by the internal ADC, and ultimately performs "hit" detection.
*
* @link       Datasheets:
*             KDT00030 Phototransistor: https://www.mouser.com/datasheet/2/308/1/KDT00030_D-2314605.pdf
*
**/

#pragma once
#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

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

class LightSensor
{
public:

	static unsigned int ADC_value;

	// Initialize the uC's GPIO to use the voltage sensor pin as input.
	static void InitGPIO();

	// Initialize the uC's ADC Module
	static void InitADC();

};

/************************************************************************/
/*                         Routine declarations                         */
/************************************************************************/



#endif // !LIGHT_SENSOR_H
