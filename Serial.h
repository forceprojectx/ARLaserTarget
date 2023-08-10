/**
* @brief      Mesh Network communications via NRF24L01.
*
* @copyright  Copyright (c) 2022 Bebop Arms. All rights reserved.
*
* @details    TODO: More comments to quickly describe functionalities and
*                   the file content.
*
* @link       TODO: Link to the article that describe your module in the
*                   WIKI.
**/

#pragma once
#ifndef SERIAL_H
#define SERIAL_H

/************************************************************************/
/*                           Include section                            */
/************************************************************************/
#include <msp430.h>
#include <stdint.h>

/************************************************************************/
/*                         #define declarations                         */
/************************************************************************/
#define SERIAL_BUFFER_LEN   128

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

class Serial
{
    Serial();
    ~Serial();



    friend __interrupt void USCI0RX_ISR(void);



    // buffers
    volatile static uint8_t rxBuffer[SERIAL_BUFFER_LEN];
    volatile static uint8_t txBuffer[SERIAL_BUFFER_LEN];

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



#endif // !SERIAL_H

