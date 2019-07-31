//
// This is a very basic program which outputs a greeting, then terminates.
//
// The program is hard-wired:
//
//  org 0
//	ld a, 'H'
//	out (1), a
//	ld a, 'e'
//	out (1) , a
//	ld a, 'l'
//	out (1),a
//	out (1), a
//	ld a, 'o'
//	out(1),a
//	ld a, '\n'
//	out(1),a
//    loop:
// 	jp loop
//
// We let this program run for a few thousand cycles, which should be
// sufficient to write the text "Hello" to the serial-console.
//
// Steve
//


#include <z80retroshield.h>

//
// Our program.
//
unsigned char rom[32] =
{
    0x3e, 0x48, 0xd3, 0x01, 0x3e, 0x65, 0xd3, 0x01, 0x3e, 0x6c, 0xd3, 0x01,
    0xd3, 0x01, 0x3e, 0x6f, 0xd3, 0x01, 0x3e, 0x0a, 0xd3, 0x01, 0xc3, 0x16,
    0x00
};
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
// I/O function handler.
//
void io_write(int address, char byte)
{
    if (address == 1)
    {
        Serial.write(byte);
    }
    else
    {
        Serial.println("Invalid address ");
    }

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
    // We have to setup a RAM-read callback, otherwise the program
    // won't be fetched from RAM and executed.
    //
    // Then we setup a callback to be executed every time an "out (x),y"
    // instruction is encountered.
    //
    cpu.set_ram_read(ram_read);
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

    // We've run a tick.
    cycles++;
}
