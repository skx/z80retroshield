//
// This is a very basic program which reads input from the serial-console,
// converts it to upper-cases, and outputs it.
//
// Steve
//


#include <z80retroshield.h>

//
// Our program.
//
// The source code is located in `uc.z80`.
//
unsigned char memory[32] =
{
    0x3e, 0x3e, 0xd3, 0x01, 0xdb, 0x01, 0xfe, 0xff, 0xca, 0x1b, 0x00, 0xfe,
    0x61, 0xda, 0x17, 0x00, 0xfe, 0x7a, 0xd2, 0x17, 0x00, 0xd6, 0x20, 0xd3,
    0x01, 0x18, 0xe9, 0x76
};
int memory_len = sizeof(memory) / sizeof(memory[0]);


//
// Our helper
//
Z80RetroShield cpu;


//
// RAM I/O function handler.
//
char memory_read(int address)
{
    return (memory[address]) ;
}


//
// I/O function handler.
//
char io_read(int address)
{
    if (address == 1)
    {
        while (! Serial.available())
        {
            delay(1);
        }

        char r = Serial.read();
        return (r);
    }

    return 0;
}

//
// I/O function handler.
//
void io_write(int address, char byte)
{
    if (address == 1)
    {
        Serial.write(byte);
        return;
    }

    return;
}


//
// Setup routine: Called once.
//
void setup()
{
    Serial.begin(115200);


    //
    // Setup callbacks.
    //
    // We must setup a RAM-read callback, otherwise the program
    // won't be fetched from RAM and executed.
    //
    cpu.set_memory_read(memory_read);

    //
    // Now we setup a callback to be executed every time an IN/OUT
    // instruction is encountered.
    //
    cpu.set_io_read(io_read);
    cpu.set_io_write(io_write);

    //
    // Configured.
    //
    Serial.println("Z80 configured; launching program.");
}


//
// Loop function: Called forever.
//
void loop()
{
    //
    // Step the CPU.
    //
    cpu.Tick();

}
