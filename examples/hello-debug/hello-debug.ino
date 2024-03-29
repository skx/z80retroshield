//
// This sketch uses code on the Z80 to write a greeting to the serial-console.
//
// We let this program run for a few thousand cycles, which should be
// sufficient to allow the text-output, "Hello\n", to complete.
//
// Steve
//

//
// You can see how it runs with additional debug message by following steps below,
// 1. Use Z80RetroShieldDebug instance instead of z80retroshield
// 2. Set debug_output callback
// 3. Set some debug flags by calling enable_debug()
// 

#include <z80retroshield.h>
#include <z80retroshieldDebug.h>

//
// Our program, as hex.
//
// The source code is located in `hello.z80`.
//
unsigned char memory[32] =
{
    0x3e, 0x48, 0xd3, 0x01, 0x3e, 0x65, 0xd3, 0x01, 0x3e, 0x6c, 0xd3, 0x01,
    0xd3, 0x01, 0x3e, 0x6f, 0xd3, 0x01, 0x3e, 0x0a, 0xd3, 0x01, 0xc3, 0x16,
    0x00
};
int memory_len = sizeof(memory) / sizeof(memory[0]);


//
// Our helper
//
Z80RetroShieldDebug cpu;

//
// RAM I/O function handler.
//
char memory_read(int address)
{
    return (memory[address]);
}


//
// I/O function handler.
//
void io_write(int address, char byte)
{
    if (address == 1) {
        Serial.write(byte);
        // Display characters one by one because you get so many debug output
        Serial.println();
    }
}


//
// Debug message handler.
//
void debug_output(const char* msg) {
    Serial.println(msg);
}


//
// Setup routine: Called once.
//
void setup()
{
    Serial.begin(115200);
    while (!Serial);
    delay(1000);

    //
    // Setup callbacks.
    //
    // We must setup a memory-read callback, otherwise the program
    // won't be fetched and executed.
    //
    cpu.set_memory_read(memory_read);

    //
    // Then we setup a callback to be executed every time an "out (x),y"
    // instruction is encountered.
    //
    cpu.set_io_write(io_write);

    //
    // Enable debug output.
    //
    cpu.set_debug_output(debug_output);
    cpu.enable_debug(cpu.DEBUG_FLAG_IO);
    cpu.enable_debug(cpu.DEBUG_FLAG_MEM);

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
    if (cycles > 150)
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
    // We've run a tick.
    //
    cycles++;
}
