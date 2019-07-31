
// The MIT License (MIT)

// Copyright (c) 2019 Erturk Kocalar, http://8Bitforce.com/
// Copyright (c) 2019 Steve Kemp, https://steve.kemp.fi/

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#include <avr/pgmspace.h>
#include <stdlib.h>
#include <Arduino.h>
#include <z80retroshield.h>


#define byte char

//
// This stuff should go away ..
//

#define ADDR_8251_DATA          0x00
#define ADDR_8251_MODCMD        0x01

#define STATE_8251_RESET        0x01
#define STATE_8251_INITIALIZED  0x00
#define CMD_8251_INTERNAL_RESET 0x40
#define CMD_8251_RTS            0x20
#define CMD_8251_DTR            0x02
#define STAT_8251_TxRDY         0x01
#define STAT_8251_RxRDY         0x02
#define STAT_8251_TxE           0x04
#define STAT_DSR                0x80

byte reg8251_STATE;      // register to keep track of 8251 state: reset or initialized
byte reg8251_MODE;
byte reg8251_COMMAND;
byte reg8251_STATUS;
byte reg8251_DATA;



/* Digital Pin Assignments */
#define DATA_OUT (PORTL)
#define DATA_IN  (PINL)
#define ADDR_H   (PINC)
#define ADDR_L   (PINA)
#define ADDR     ((unsigned int) (ADDR_H << 8 | ADDR_L))

#define uP_RESET_N  38
#define uP_MREQ_N   41
#define uP_IORQ_N   39
#define uP_RD_N     53
#define uP_WR_N     40
#define uP_NMI_N    51
#define uP_INT_N    50
#define uP_CLK      52

// Fast routines to drive clock signals high/low; faster than digitalWrite
// required to meet >100kHz clock
//
#define CLK_HIGH      (PORTB = PORTB | 0x02)
#define CLK_LOW       (PORTB = PORTB & 0xFC)
#define STATE_RD_N    (PINB & 0x01)
#define STATE_WR_N    (PING & 0x02)
#define STATE_MREQ_N  (PING & 0x01)
#define STATE_IORQ_N  (PING & 0x04)

#define DIR_IN  0x00
#define DIR_OUT 0xFF
#define DATA_DIR   DDRL
#define ADDR_H_DIR DDRC
#define ADDR_L_DIR DDRA

unsigned int  uP_ADDR;
byte uP_DATA;

byte prevIORQ = 0;
byte prevDATA = 0;

/*
 * Constructor
 */
Z80RetroShield::Z80RetroShield()
{

    //
    // Callbacks are all empty by default.
    //
    m_on_memory_read = NULL;
    m_on_memory_write = NULL;
    m_on_io_read = NULL;
    m_on_io_write = NULL;

    // Set directions
    DATA_DIR = DIR_IN;
    ADDR_H_DIR = DIR_IN;
    ADDR_L_DIR = DIR_IN;

    pinMode(uP_RESET_N, OUTPUT);
    pinMode(uP_WR_N, INPUT);
    pinMode(uP_RD_N, INPUT);
    pinMode(uP_MREQ_N, INPUT);
    pinMode(uP_IORQ_N, INPUT);
    pinMode(uP_INT_N, OUTPUT);
    pinMode(uP_NMI_N, OUTPUT);
    pinMode(uP_CLK, OUTPUT);

    Reset();
    digitalWrite(uP_CLK, LOW);

}

/*
 * Destructor
 */
Z80RetroShield::~Z80RetroShield()
{
}

/*
 * Run the processor.
 *
 * This will step the processor by a single clock-tick.
 */
void Z80RetroShield::Tick()
{
    CLK_HIGH;    // CLK goes high


    uP_ADDR = ADDR;

    // unlike memory mapped devices in 6502 & 6809,
    // Z80RetroShield bus has two modes: Memory (MREQ_N) and IO (IORQ_N)

    //////////////////////////////////////////////////////////////////////
    // Memory Access?
    if (!STATE_MREQ_N)
    {

        // Memory Read?
        if (!STATE_RD_N)
        {
            // change DATA port to output to uP:
            DATA_DIR = DIR_OUT;

            if (m_on_memory_read == NULL)
                DATA_OUT = 0x00;  // nop
            else
            {
                DATA_OUT = m_on_memory_read(uP_ADDR);
            }
        }
        else if (!STATE_WR_N)
        {
            if (m_on_memory_write != NULL)
                m_on_memory_write(uP_ADDR, DATA_IN);
        }

        goto tick_tock;

    }

    //////////////////////////////////////////////////////////////////////
    // IO Access?
    if (!STATE_IORQ_N)
    {
        // IO Read?
        if (!STATE_RD_N && prevIORQ)
        {
            // change DATA port to output to uP:
            DATA_DIR = DIR_OUT;

            // output data at this cycle too
            DATA_OUT = m_on_io_read(ADDR_L);

        }
        // IO Write?
        if (!STATE_WR_N && prevIORQ)
        {
            m_on_io_write(ADDR_L, DATA_IN);
        }
    }
tick_tock:
    prevIORQ = STATE_IORQ_N;

    //////////////////////////////////////////////////////////////////////
    // start next cycle
    CLK_LOW;    // E goes low

    // natural delay for DATA Hold time (t_HR)
    DATA_DIR = DIR_IN;

}

/*
 * Reset the processor.
 *
 * This will run the clock a few cycles to ensure that the processor
 * is reset fully.
 */
void Z80RetroShield::Reset()
{

    // Drive RESET conditions
    digitalWrite(uP_RESET_N, LOW);
    digitalWrite(uP_INT_N, HIGH);
    digitalWrite(uP_NMI_N, HIGH);

    for (int i = 0; i < 4; i++)
        Tick();

    // Drive RESET conditions
    digitalWrite(uP_RESET_N, HIGH);

}
