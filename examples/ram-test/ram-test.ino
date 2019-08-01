//
// This is an example which uses our library to execute a very simple program
// which reads and writes to RAM.
//
// We let this program run for a few thousand cycles, dump the contents
// of the memory every few cycles.  The result should be that the value
// of our counter increments.
//
// Steve
//


#include <z80retroshield.h>

//
// Our program.
//
// The source code is located in `ram-test.z80`.
//
unsigned char memory[9] =
{
    0x21, 0x08, 0x00, 0x7e, 0x3c, 0x77, 0x18, 0xfb, 0x00
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
// RAM I/O function handler.
//
void memory_write(int address, char byte)
{
    memory[address] = byte;
}


//
// Setup routine: Called once.
//
void setup()
{
    Serial.begin(115200);

    //
    // We must setup a memory-read callback, otherwise the program
    // won't be fetched and executed.
    //
    // We also setup a callback for memory-write, so the updated value
    // of our counter may be persisted.
    //
    cpu.set_memory_read(memory_read);
    cpu.set_memory_write(memory_write);

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
    // We stop running after a specific number of cycles
    //
    // Have we reached that point?
    //
    static bool done = false;

    //
    // Count the number of times we've pumped the Z80.
    //
    static int cycles = 0;

    //
    // Are we done?  If so return.
    //
    if (done)
        return;

    //
    // We stop running after running a specific number of cycles
    //
    if (cycles > 4096)
    {
        Serial.print("Z80 processor stopped ");
        Serial.print(cycles);
        Serial.println(" cycles executed.");
        done = true;
        return;
    }

    //
    // Step the CPU.
    //
    cpu.Tick();

    //
    // Dump the contents of our memory every few cycles.
    //
    if ((cycles % 16) == 0)
    {

        // Temporary buffer for a line of output.
        char tmp[64] = { '\0' };

        // Update the buffer with our RAM.
        for (int i = 0 ; i < memory_len; i++)
        {
            char x[5] = { '\0' };
            sprintf(x, "%02X ", memory[i]);
            strcat(tmp, x);
        }

        // Output the dump to the serial console.
        Serial.print("RAM [ ");
        Serial.print(tmp);
        Serial.print("] ");
        Serial.print(" counter value: ");
        Serial.println(memory[8], HEX);
    }

    //
    // We've run a tick.
    //
    cycles++;
}
