
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



/*
 *  This class abstracts the usage of the retroshield to drive a Z80.
 *
 *  The intention is that you can instantiate an instance of this class
 *  and ensure it operates but keep the details of RAM & I/O inside your
 *  main-sketch.
 *
 *  You provide a callback to be invoked when RAM is read/written, and
 *  they are missing you'll end up with a free-running system.
 *
 */

#ifndef _Z80RetroShield_DRIVER
#define _Z80RetroShield_DRIVER 1


/*
 * Signatures for our callback functions.
 *
 */
typedef char (*memoryRead)(int address);
typedef void (*memoryWrite)(int address, char byte);
typedef char (*ioread)(int address);
typedef void (*iowrite)(int address, char byte);




class Z80RetroShield
{

public:

    // constructor & destructor
    Z80RetroShield();
    ~Z80RetroShield();

    // reset
    void Reset();

    // run a single cycle
    void Tick();

    // set callbacks
    void set_ram_read(memoryRead cb)
    {
        m_on_memory_read = cb;
    };
    void set_ram_write(memoryWrite cb)
    {
        m_on_memory_write = cb;
    };
    void set_io_read(ioread cb)
    {
        m_on_io_read = cb;
    };
    void set_io_write(iowrite cb)
    {
        m_on_io_write = cb;
    };


private:

    // pointers to callback functions
    memoryRead  m_on_memory_read;
    memoryWrite m_on_memory_write;
    ioread      m_on_io_read;
    iowrite     m_on_io_write;


};


#endif /* _Z80RetroShield_DRIVER */
