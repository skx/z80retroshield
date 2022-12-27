
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




#if not defined(Z80RetroShield_DEBUG) && not defined(_Z80RetroShield_DRIVER) || \
    defined(Z80RetroShield_DEBUG) && not defined(_Z80RetroShieldDebug_DRIVER)

#ifdef Z80RetroShieldClassName
#undef Z80RetroShieldClassName
#endif
#ifdef  Z80RetroShield_DEBUG
#define Z80RetroShieldClassName Z80RetroShieldDebug
#define _Z80RetroShieldDebug_DRIVER 1
#else
#define Z80RetroShieldClassName Z80RetroShield
#define _Z80RetroShield_DRIVER 1
#endif

/**
 *
 *  This class abstracts the usage of the retroshield to drive a Z80.
 *
 *  The intention is that you can instantiate an instance of this class
 * and ensure it operates but keep the details of RAM & I/O inside your
 * main-sketch.
 *
 *  You provide callbacks to be invoked when memory is read/written, and
 * another pair of callbacks for handling port-based I/O.
 *
 */
class Z80RetroShieldClassName
{

public:

    /**
     * Signatures for our callback functions.
     *
     * There are two callbacks for memory, and two for port-based I/O.
     *
     * You must provide the memory-read callback, otherwise the processor
     * will not be able to fetch instructions to execute, however the rest
     * are optional.
     *
     * It is suggested you allow writing to memory at least though, because
     * without the ability to write to memory things like the `call` instruction
     * will be broken - i.e. If you set the stack-pointer to 0x100 and a call
     * is made the return address will be pushed into that memory..
     */
    typedef char (*memoryRead)(int address);
    typedef void (*memoryWrite)(int address, char byte);
    typedef char (*ioread)(int address);
    typedef void (*iowrite)(int address, char byte);
    typedef void (*debugOutput)(const char* msg);

    /**
     * Constructor.
     */
    Z80RetroShieldClassName();

    /**
     * Destructor.
     */
    ~Z80RetroShieldClassName();

    /**
     * Reset the processor.
     *
     * After this is called the CPU will begin executing code at 0x0000 again.
     */
    void Reset();

    /**
     * Run a single cycle.
     */
    void Tick();

    /**
     * Set the callback to be invoked when memory is to be read.
     */
    void set_memory_read(memoryRead cb)
    {
        m_on_memory_read = cb;
    }

    /**
     * Set the callback to be invoked when memory is to be read.
     *
     * Deprecated.  use `set_memory_read` instead.
     */
    void set_ram_read(memoryRead cb)
    {
        set_memory_read(cb);
    };

    /**
     * Set the callback to be invoked when memory is to be written to.
     */
    void set_memory_write(memoryWrite cb)
    {
        m_on_memory_write = cb;
    }

    /**
     * Set the callback to be invoked when memory is to be written to.
     *
     * Deprecated.  use `set_memory_read` instead.
     */
    void set_ram_write(memoryWrite cb)
    {
        set_memory_write(cb);
    };

    /**
     * Set the callback to be invoked when an I/O read-request is made.
     */
    void set_io_read(ioread cb)
    {
        m_on_io_read = cb;
    };

    /**
     * Set the callback to be invoked when an I/O write-request is made.
     */
    void set_io_write(iowrite cb)
    {
        m_on_io_write = cb;
    };

    void set_debug_output(debugOutput cb)
    {
        m_debug_output = cb;
        m_debug = true;
    };


private:

    /**
     * Pointers to our configured callback functions.
     */
    memoryRead m_on_memory_read;
    memoryWrite m_on_memory_write;
    ioread       m_on_io_read;
    iowrite      m_on_io_write;
    debugOutput m_debug_output;
    bool m_debug;
    unsigned long m_cycle;

    void show_status(const char* header);

#ifdef Z80RetroShield_DEBUG
    inline void debug_show_status(const char* header) {
        if (m_debug)
            show_status(header);
    };

    inline void debug_count_cycle(void) {
        if (m_debug)
            m_cycle++;
    };
#else
    inline void debug_show_status(const char* header) { };
    inline void debug_count_cycle(void) { };
#endif

};

#endif /* _Z80RetroShield_DRIVER */
