//
// This is a very basic program which uses our library to execute a
// very simple program.
//
// The program is hard-wired and just increments a single byte of RAM:
//
//     ld hl, output
//     xor a,a
//  loop:
//     inc a
//     ld (hl), a
//     jp loop
//  output:
//     db 00
//
// The program compiles to a few bytes, so we only pretend we have
// 16-bytes of RAM/ROM - and output the contents every ten cycles.
//
// After a few iterations, sufficient to prove it works, we stop the
// processor.
//
// Steve
//


#include <z80retroshield.h>

//
// our program.
//
unsigned char rom[16] = {  0x21, 0x09, 0x00, 0xaf, 0x3c, 0x77, 0xc3, 0x04, 0x00, 0x00 };
int rom_len = sizeof(rom) / sizeof(rom[0]);



//
// Our helper
//
Z80RetroShield cpu;

//
// RAM I/O function handler.
//
char ram_read(int address)
{
    if (address >= rom_len)
        address = 0;

    return (rom[address]) ;
}

//
// RAM I/O function handler.
//
void ram_write(int address, char byte)
{
    if (address >= rom_len)
        address = 0;

    rom[address] = byte;
}


//
// Setup routine: Called once.
//
void setup()
{
    Serial.begin(115200);

    //
    // We have to setup a RAM-read callback, otherwise the program
    // won't be fetched from RAM and executed.
    //
    // We also setup a callback for RAM-write, so the updated value
    // can be stored in RAM.
    //
    cpu.set_ram_read(ram_read);
    cpu.set_ram_write(ram_write);

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
    // Dump the RAM every few cycles.
    //
    if ((cycles % 16) == 0)
    {

        // Temporary buffer for a line of output.
        char tmp[64] = { '\0' };

        // Update the buffer with our RAM.
        for (int i = 0 ; i < 10; i++)
        {
            char x[5] = { '\0' };
            sprintf(x, "%02X ", rom[i]);
            strcat(tmp, x);
        }

        // Show to the serial console.
        Serial.print("RAM [");
        Serial.print(tmp);
        Serial.print("] ");
        Serial.print(" counter: ");
        Serial.println(rom[9], HEX);
    }

    // We've run a tick.
    cycles++;
}
