/**
* @brief      Bluetooth communications via HC-05.
*
* @copyright  Copyright (c) 2022 Bebop Arms. All rights reserved.
*
*
* @details    Wrapper for serial communication functions between uC and bluetooth module.
*
*
* @link       TODO: Link to the article that describe your module in the
*                   WIKI.
**/

#pragma once
#ifndef BLUE_TOOTH_H
#define BLUE_TOOTH_H

/************************************************************************/
/*                           Include section                            */
/************************************************************************/
#include <msp430.h>
#include <stdint.h>

/************************************************************************/
/*                         #define declarations                         */
/************************************************************************/
#define BT_BUFFER_LEN   128

/************************************************************************/
/*                         Forward declarations                         */
/************************************************************************/
__interrupt void USCI0RX_ISR(void);
/************************************************************************/
/*                     Data structures declarations                     */
/************************************************************************/

/************************************************************************/
/*                         Classes declarations                         */
/************************************************************************/

class Bluetooth
{
    Bluetooth();
    ~Bluetooth();

    friend __interrupt void USCI0RX_ISR(void);

    // buffers
    volatile static uint8_t rxBuffer[BT_BUFFER_LEN];
    volatile static uint8_t txBuffer[BT_BUFFER_LEN];

    // buffer insertion position
    volatile static uint8_t rxIndex;
    volatile static uint8_t txIndex;

    // buffer read position
    volatile static uint8_t rxInBuffer;
    volatile static uint8_t txInBuffer;

    volatile static bool txComplete;

public:
    static void Init();
    static void Send(const char* buffer, int length);
    static void print(const char* buffer);
    static void print(uint32_t val);
    static void println(uint32_t val);
    static void println(const char* buffer);
    static bool HasData();
    static bool ReadByte(uint8_t& destination);
    static uint8_t read();
};

/************************************************************************/
/*                         Routine declarations                         */
/************************************************************************/



#endif // !BLUE_TOOTH_H

