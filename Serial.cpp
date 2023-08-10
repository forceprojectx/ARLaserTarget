/**
* @brief      Mesh Network communications via NRF24L01.
*
* @copyright  Copyright (c) 2022 Bebop Arms. All rights reserved.
*
*
* @details    TODO: More comments to quickly describe functionalities and
*                   the file content.
*
* @link       TODO: Link to the article that describe your module in the
*                   WIKI.
**/

/************************************************************************/
/*                           Include section                            */
/************************************************************************/

#include "Serial.h"


/************************************************************************/
/*                            Using section                             */
/************************************************************************/

/************************************************************************/
/*                         #define declarations                         */
/************************************************************************/
#define TXD_BIT BIT0
#define RXD_BIT BIT1
#define TXD 0
#define RXD 1

/************************************************************************/
/*                         Forward declarations                         */
/************************************************************************/

/************************************************************************/
/*                     Data structures declarations                     */
/************************************************************************/

/************************************************************************/
/*                         Routine declarations                         */
/************************************************************************/
int itoa(uint32_t value, char *sp)
{
    char tmp[16];// be careful with the length of the buffer
    char *tp = tmp;
    int i;
    uint32_t v = value;



    while (v || tp == tmp)
    {
        i = v % 10;
        v /= 10;
        if (i < 10)
            *tp++ = i + '0';
        else
            *tp++ = i + 'a' - 10;
    }

    int len = tp - tmp;



    while (tp > tmp)
        *sp++ = *--tp;
    *sp = 0;

    return len;
}
/************************************************************************/
/*                        Variables declarations                        */
/************************************************************************/
volatile uint8_t Serial::rxBuffer[SERIAL_BUFFER_LEN] = { 0 };
volatile uint8_t Serial::txBuffer[SERIAL_BUFFER_LEN] = { 0 };
volatile uint8_t Serial::rxIndex = 0;
volatile uint8_t Serial::txIndex = 0;
volatile uint8_t Serial::rxInBuffer = 0;
volatile uint8_t Serial::txInBuffer = 0;
volatile bool Serial::txComplete = true;

const char newLine[] = { 0x0D,0x0A };

/************************************************************************/
/*                      Implementation (PRIVATE)                        */
/************************************************************************/

Serial::Serial()
{
}


Serial::~Serial()
{
}


/************************************************************************/
/*                      Implementation (PROTECTED)                      */
/************************************************************************/



/************************************************************************/
/*                      Implementation (PUBLIC)                         */
/************************************************************************/


void Serial::Init()
{

    rxIndex = 0;
    txIndex = 0;
    rxInBuffer = 0;
    txInBuffer = 0;

    // select UART function for pins p2.0,1
    P2SEL0 &= ~(TXD_BIT + RXD_BIT);
    P2SEL1 |= TXD_BIT + RXD_BIT;

    //set 9600 baud
    UCA0CTLW0 = UCSSEL__SMCLK + UCSWRST;
    UCA0BR0 = 19;
    UCA0BR1 = 0;
    UCA0MCTLW = UCBRF_8 + UCOS16;

    UCA0CTL1 &= ~UCSWRST; // allow uart comms
    // enable RX and TX interrupt
    UCA0IE |= UCRXIE | UCTXIE;
    UCA0IFG ^= UCA0IFG; // clear any interrupts

}

void Serial::Send(const char * buffer, int length)
{
    if ((buffer != nullptr) && (length > 0))
    {
        for (int i = 0; (i < length); i++)
        {
            txBuffer[txIndex] = buffer[i];
            txIndex++;
            if (txIndex >= SERIAL_BUFFER_LEN)
            {
                txIndex = 0;
            }

            if (txIndex == txInBuffer)
            {
                // message wrapped around
                break;
            }
        }
        UCA0IE |= UCTXIE;
        UCA0IFG |= UCTXIFG;
    }
}

void Serial::print(const char * buffer)
{

    if ((buffer != nullptr))
    {
        uint8_t i = 0;
        while (buffer[i] != 0)
        {
            txBuffer[txIndex] = buffer[i++];
            txIndex++;
            if (txIndex >= SERIAL_BUFFER_LEN)
            {
                txIndex = 0;
            }

            if (txIndex == txInBuffer)
            {
                // message wrapped around
                break;
            }
        }
        Serial::txComplete = false;
        UCA0IE |= UCTXIE;
        UCA0IFG |= UCTXIFG;
        // wait for print to complete
        while (!Serial::txComplete)
        {
            //__no_operation();
        }

    }
}

void Serial::print(uint32_t val)
{
    char ch[24] = { 0 };

    int len = itoa(val, ch);
    print(ch);
}

void Serial::println(uint32_t val)
{
    print(val);

    Send(newLine, 2);
}

void Serial::println(const char * buffer)
{
    print(buffer);
    Send(newLine, 2);
}

bool Serial::HasData()
{
    return (rxInBuffer != rxIndex);
}

bool Serial::ReadByte(uint8_t& destination)
{
    bool retval = false;
    if (rxInBuffer != rxIndex)
    {
        destination = rxBuffer[rxInBuffer];
        rxInBuffer++;
        if (rxInBuffer >= SERIAL_BUFFER_LEN)
        {
            rxInBuffer = 0;
        }
        retval = true;
    }

    return retval;
}

uint8_t Serial::read()
{
    uint8_t byte = 0;
    ReadByte(byte);
    return byte;
}


/************************************************************************/
/*                      Implementation (INTERRUPTS)                     */
/************************************************************************/
// uart interrupt handler

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__((interrupt(USCI_A0_VECTOR))) USCI0RX_ISR(void)
#else
#error Compiler not supported!
#endif
{
    switch (__even_in_range(UCA0IV, 8))
    {
    case  0: break;                          // No interrupt
    case  2:                                  // rx buffer full
        // get char from hardware buffer
        Serial::rxBuffer[Serial::rxIndex] = UCA0RXBUF_L;

        // update the insertion point
        Serial::rxIndex++;
        if (Serial::rxIndex >= SERIAL_BUFFER_LEN)
        {
            Serial::rxIndex = 0;
        }

        break;
    case  4:                                // Tx buffer empty
        if (Serial::txInBuffer != Serial::txIndex)
        {
            UCA0TXBUF = Serial::txBuffer[Serial::txInBuffer];
            Serial::txInBuffer++;
            if (Serial::txInBuffer >= SERIAL_BUFFER_LEN)
            {
                Serial::txInBuffer = 0;
            }

        }
        else
        {
            UCA0IE ^= UCTXIE;
            Serial::txComplete = true;
        }


        break;
    case  6: break;                          // start bit received
    case  8: break;                          // transmit complete
        break;
    default: break;
    }
}

