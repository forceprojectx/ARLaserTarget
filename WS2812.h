/**
* @brief      WS2812 controls
*
* @copyright  Copyright (c) 2022 Bebop Arms. All rights reserved.
*
*
* @details    Functions and related data for controlling WS2812 LEDs
*
* @link       TODO: Link to the article that describe your module in the
*                   WIKI.
**/

#pragma once
#ifndef WS_2812_CONTROLLER_H
#define WS_2812_CONTROLLER_H

/************************************************************************/
/*                           Include section                            */
/************************************************************************/

#include <stdint.h>

/************************************************************************/
/*                         #define declarations                         */
/************************************************************************/
#define WS2812__BIT_HIGH    DEBUG_3|=DEBUG_2_A
#define WS2812__BIT_LOW     DEBUG_3&=~DEBUG_2_A

/************************************************************************/
/*                         Forward declarations                         */
/************************************************************************/

/************************************************************************/
/*                     Data structures declarations                     */
/************************************************************************/

/************************************************************************/
/*                         Classes declarations                         */
/************************************************************************/

/************************************************************************/
/*                         Routine declarations                         */
/************************************************************************/

// Draws a frame for the pattern
// @return bool: true if the pattern has a next frame.
bool RedCCW(uint16_t currentFrame);

#endif // !WS_2812_CONTROLLER_H

