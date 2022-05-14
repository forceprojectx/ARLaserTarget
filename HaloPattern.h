/**
* @brief      LED Halo pattern controls
*
* @copyright  Copyright (c) 2022 Bebop Arms. All rights reserved.
*
*
* @details    Controls for the LED ring (halo) around the target zone which serves as an annunciator for the target status.
*
* @link       TODO: Link to the article that describe your module in the
*                   WIKI.
**/

#pragma once
#ifndef HALO_PATTERN_H
#define HALO_PATTERN_H

/************************************************************************/
/*                           Include section                            */
/************************************************************************/

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

/************************************************************************/
/*                         Routine declarations                         */
/************************************************************************/

// Draws a frame for the pattern
// @return bool: true if the pattern has a next frame.
bool RedCW(uint16_t currentFrame);

// Draws a frame for the pattern
// @param currentFrame: The current animation frame. Zero indexed. (First frame is frame 0)
// @return bool: true if the pattern has a next frame.
bool BlueCW(uint16_t currentFrame);

// Draws a frame for the pattern
// @param currentFrame: The current animation frame. Zero indexed. (First frame is frame 0)
// @return bool: true if the pattern has a next frame.
bool GreenCW(uint16_t currentFrame);

void InitLEDController();

#endif // !HALO_PATTERN_H

