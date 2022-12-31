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

/************************************************************************/
/*                           Include section                            */
/************************************************************************/

#include "Bluetooth.h"
#include "LaserTarget.h"

/************************************************************************/
/*                            Using section                             */
/************************************************************************/

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
volatile uint8_t Bluetooth::rxBuffer[BT_BUFFER_LEN] = { 0 };
volatile uint8_t Bluetooth::txBuffer[BT_BUFFER_LEN] = { 0 };
volatile uint8_t Bluetooth::rxIndex = 0;
volatile uint8_t Bluetooth::txIndex = 0;
volatile uint8_t Bluetooth::rxInBuffer = 0;
volatile uint8_t Bluetooth::txInBuffer = 0;
volatile bool Bluetooth::txComplete = true;

const char newLine[] = { 0x0D,0x0A };

/************************************************************************/
/*                      Implementation (PRIVATE)                        */
/************************************************************************/

Bluetooth::Bluetooth()
{
}


Bluetooth::~Bluetooth()
{
}


/************************************************************************/
/*                      Implementation (PROTECTED)                      */
/************************************************************************/



/************************************************************************/
/*                      Implementation (PUBLIC)                         */
/************************************************************************/


void Bluetooth::Init()
{

    rxIndex = 0;
    txIndex = 0;
    rxInBuffer = 0;
    txInBuffer = 0;

    // Enable changes to port registers
    PM5CTL0 &= ~LOCKLPM5;

    // select UART function for bluetooth pins
    BT_SEL0 |= (BT_TX + BT_RX);
    BT_SEL1 &= ~(BT_TX + BT_RX);
    
    // i/o dir doesnt matter, set both bits for no reason
    BT_DIR |= BT_TX | BT_RX;

    BT_REN &= ~(BT_TX + BT_RX);
    BT_IES &= ~(BT_TX + BT_RX);


    //set 9600 baud based on 2Mhz clock
    BT_CTLW0 = UCSSEL__SMCLK + UCSWRST;
    // generated from https://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
    /*
     * clockPrescalar: 13
     * firstModReg: 0
     * secondModReg: 0
     * overSampling: 1
    */
    BT_BRW = 13;
    BT_MCTLW = UCOS16;

    // eUSCI_A reset released for operation.
    BT_CTLW0 &= ~UCSWRST;
    // enable RX and TX interrupt
    BT_IE |= UCRXIE | UCTXIE;
    // clear any interrupts
    BT_IFG ^= BT_IFG;
}

void Bluetooth::Send(const char * buffer, int length)
{
    if ((buffer != nullptr) && (length > 0))
    {
        for (int i = 0; (i < length); i++)
        {
            txBuffer[txIndex] = buffer[i];
            txIndex++;
            if (txIndex >= BT_BUFFER_LEN)
            {
                txIndex = 0;
            }

            if (txIndex == txInBuffer)
            {
                // message wrapped around
                break;
            }
        }
        BT_IE |= UCTXIE;
        BT_IFG |= UCTXIFG;
    }
}

void Bluetooth::print(const char * buffer)
{

    if ((buffer != nullptr))
    {
        uint8_t i = 0;
        while (buffer[i] != 0)
        {
            txBuffer[txIndex] = buffer[i++];
            txIndex++;
            if (txIndex >= BT_BUFFER_LEN)
            {
                txIndex = 0;
            }

            if (txIndex == txInBuffer)
            {
                // message wrapped around
                break;
            }
        }
        Bluetooth::txComplete = false;
        BT_IE |= UCTXIE;
        BT_IFG |= UCTXIFG;
        // wait for print to complete
        while (!Bluetooth::txComplete)
        {
            //__no_operation();
        }

    }
}

void Bluetooth::print(uint32_t val)
{
    char ch[24] = { 0 };

    int len = itoa(val, ch);
    print(ch);
}

void Bluetooth::println(uint32_t val)
{
    print(val);

    Send(newLine, 2);
}

void Bluetooth::println(const char * buffer)
{
    print(buffer);
    Send(newLine, 2);
}

bool Bluetooth::HasData()
{
    return (rxInBuffer != rxIndex);
}

bool Bluetooth::ReadByte(uint8_t& destination)
{
    bool retval = false;
    if (rxInBuffer != rxIndex)
    {
        destination = rxBuffer[rxInBuffer];
        rxInBuffer++;
        if (rxInBuffer >= BT_BUFFER_LEN)
        {
            rxInBuffer = 0;
        }
        retval = true;
    }

    return retval;
}

uint8_t Bluetooth::read()
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
        Bluetooth::rxBuffer[Bluetooth::rxIndex] = UCA0RXBUF_L;

        // update the insertion point
        Bluetooth::rxIndex++;
        if (Bluetooth::rxIndex >= BT_BUFFER_LEN)
        {
            Bluetooth::rxIndex = 0;
        }

        break;
    case  4:                                // Tx buffer empty
        if (Bluetooth::txInBuffer != Bluetooth::txIndex)
        {
            UCA0TXBUF = Bluetooth::txBuffer[Bluetooth::txInBuffer];
            Bluetooth::txInBuffer++;
            if (Bluetooth::txInBuffer >= BT_BUFFER_LEN)
            {
                Bluetooth::txInBuffer = 0;
            }

        }
        else
        {
            BT_IE ^= UCTXIE;
            Bluetooth::txComplete = true;
        }


        break;
    case  6: break;                          // start bit received
    case  8: break;                          // transmit complete
        break;
    default: break;
    }
}

