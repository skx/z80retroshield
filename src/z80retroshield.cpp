
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


#include <stdlib.h>
#include <Arduino.h>
#include <z80retroshield.h>

#include <avr/pgmspace.h>

/* Digital Pin Assignments */
static inline void DATA_OUT(uint8_t data) { (PORTL) = data; }
static inline uint8_t DATA_IN(void) { return (PINL); }
static inline uint8_t ADDR_H(void)  { return (PINC); }
static inline uint8_t ADDR_L(void)  { return (PINA); }
static inline unsigned int ADDR(void) { return ((unsigned int) (ADDR_H() << 8 | ADDR_L())); }

static const uint8_t uP_RESET_N = 38;
static const uint8_t uP_MREQ_N  = 41;
static const uint8_t uP_IORQ_N  = 39;
static const uint8_t uP_RD_N    = 53;
static const uint8_t uP_WR_N    = 40;
static const uint8_t uP_NMI_N   = 51;
static const uint8_t uP_INT_N   = 50;
static const uint8_t uP_CLK     = 52;

//
// Fast routines to drive clock signals high/low; faster than digitalWrite
// required to meet >100kHz clock
//
static inline void CLK_HIGH(void) { PORTB = PORTB | 0x02; }
static inline void CLK_LOW(void) { PORTB = PORTB & 0xFC; }
static inline uint8_t STATE_RD_N(void) { return (PINB & 0x01); }
static inline uint8_t STATE_WR_N(void) { return (PING & 0x02); }
static inline uint8_t STATE_MREQ_N(void) { return (PING & 0x01); }
static inline uint8_t STATE_IORQ_N(void) { return (PING & 0x04); }

static const uint8_t DIR_IN = 0x00;
static const uint8_t DIR_OUT = 0xFF;
static inline void DATA_DIR(uint8_t dir) { DDRL = dir; }
static inline void ADDR_H_DIR(uint8_t dir) { DDRC = dir; }
static inline void ADDR_L_DIR(uint8_t dir) { DDRA = dir; }


/*
 * Constructor
 */
Z80RetroShieldClassName::Z80RetroShieldClassName()
{
    //
    // Callbacks are all empty by default.
    //
    m_on_memory_read = NULL;
    m_on_memory_write = NULL;
    m_on_io_read = NULL;
    m_on_io_write = NULL;
    m_debug_output = NULL;
    m_debug = false;
    m_cycle = 0;

    //
    // Set directions
    //
    DATA_DIR(DIR_IN);
    ADDR_H_DIR(DIR_IN);
    ADDR_L_DIR(DIR_IN);

    //
    // Handle other pins.
    //
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
Z80RetroShieldClassName::~Z80RetroShieldClassName()
{
}

void Z80RetroShieldClassName::show_status(const char* header) {
    if (m_debug_output) {
        char buf[100];
        sprintf(buf, "%s%4ld addr=%04x data=%04x ~MREQ=%s ~IOREQ=%s  RW=%s",
                header,
                m_cycle,
                ADDR(),
                DATA_IN(),
                STATE_MREQ_N() ? "H" : "L",
                STATE_IORQ_N() ? "H" : "L",
                STATE_RD_N() ? "" : "R",
                STATE_WR_N() ? "" : "W");
        m_debug_output(buf);
#ifdef ADAFRUIT_GRAND_CENTRAL_M4
        uint32_t dataa = PORT->Group[PGA].IN.reg;
        uint32_t datab = PORT->Group[PGB].IN.reg;
        uint32_t datac = PORT->Group[PGC].IN.reg;
        uint32_t datad = PORT->Group[PGD].IN.reg;
        sprintf(buf, "IN    PA: %08X  PB: %08X  PC: %08X  PD: %08X",
                dataa, datab, datac, datad);
        m_debug_output(buf);
        uint32_t outa = PORT->Group[PGA].OUT.reg;
        uint32_t outb = PORT->Group[PGB].OUT.reg;
        uint32_t outc = PORT->Group[PGC].OUT.reg;
        uint32_t outd = PORT->Group[PGD].OUT.reg;
        sprintf(buf, "OUT   PA: %08X  PB: %08X  PC: %08X  PD: %08X",
                outa, outb, outc, outd);
        m_debug_output(buf);
        uint32_t dira = PORT->Group[PGA].DIR.reg;
        uint32_t dirb = PORT->Group[PGB].DIR.reg;
        uint32_t dirc = PORT->Group[PGC].DIR.reg;
        uint32_t dird = PORT->Group[PGD].DIR.reg;
        sprintf(buf, "DIR   PA: %08X  PB: %08X  PC: %08X  PD: %08X",
                dira, dirb, dirc, dird);
        m_debug_output(buf);
#endif
    }
}

/*
 * Run the processor.
 *
 * This will step the processor by a single clock-tick.
 */
void Z80RetroShieldClassName::Tick()
{
    /*
     * The memory address we're reading/writing to.
     */
    static unsigned int  uP_ADDR = 0;

    /*
     * The I/O address we're reading/writing to.
     */
    static uint8_t prevIORQ = 0;

    // CLK goes high
    CLK_HIGH();

    // Store the contents of the address-bus in case we're going to use it.
    uP_ADDR = ADDR();

    //////////////////////////////////////////////////////////////////////
    // Memory Access?
    if (!STATE_MREQ_N())
    {
        // RAM Read?
        if (!STATE_RD_N())
        {
            // change DATA port to output to uP:
            DATA_DIR(DIR_OUT);

            if (m_on_memory_read)
                DATA_OUT(m_on_memory_read(uP_ADDR));
            else
                DATA_OUT(0);
            debug_show_status("MEMR: ");
        }
        else if (!STATE_WR_N())
        {
            debug_show_status("MEMW: ");
            // RAM write
            if (m_on_memory_write != NULL)
                m_on_memory_write(uP_ADDR, DATA_IN());
        }

        goto tick_tock;
    }

    //////////////////////////////////////////////////////////////////////
    // IO Access?
    if (!STATE_IORQ_N())
    {
        // IO Read?
        if (!STATE_RD_N() && prevIORQ)
        {
            // change DATA port to output to uP:
            DATA_DIR(DIR_OUT);

            // output data at this cycle too
            if (m_on_io_read)
                DATA_OUT(m_on_io_read(ADDR_L()));
            else
                DATA_OUT(0);
            debug_show_status("IOR : ");
        }

        // IO Write?
        if (!STATE_WR_N() && prevIORQ)
        {
            debug_show_status("IOW : ");
            if (m_on_io_write != NULL)
                m_on_io_write(ADDR_L(), DATA_IN());
        }

        goto tick_tock;
    }
    debug_show_status("    : ");

tick_tock:
    prevIORQ = STATE_IORQ_N();

    //////////////////////////////////////////////////////////////////////
    // start next cycle
    CLK_LOW();
    debug_count_cycle();

    // natural delay for DATA Hold time (t_HR)
    DATA_DIR(DIR_IN);

}

/*
 * Reset the processor.
 *
 * This will run the clock a few cycles to ensure that the processor
 * is reset fully.
 */
void Z80RetroShieldClassName::Reset()
{
    // Drive RESET conditions
    digitalWrite(uP_RESET_N, LOW);
    digitalWrite(uP_INT_N, HIGH);
    digitalWrite(uP_NMI_N, HIGH);

    // Run for a few cycles.
    for (int i = 0; i < 4; i++)
        Tick();

    // Drive RESET conditions
    digitalWrite(uP_RESET_N, HIGH);
}
