//
// This example just tests how fast we're running our CPU.
//
// Steve
//


#include <z80retroshield.h>


//
// Our helper
//
Z80RetroShield cpu;


//
// RAM I/O function handler.
//
char memory_read(int address)
{
    //
    // All reads will return NOP (0x00)
    //
    return 0x00;
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
    // Configured.
    //
    Serial.println("Z80 configured to run endless stream of NOPs.");
    Serial.println("Cycle/Second will be dumped once per second.");
}


//
// Loop function: Called forever.
//
void loop()
{
    //
    // Static variables.
    //
    static long lastMillis = 0;
    static long cycles = 0;

    long currentMillis = millis();


    //
    // Step the CPU, and count it.
    //
    cpu.Tick(100);
    cycles += 100;


    if (currentMillis - lastMillis > 1000)
    {
        Serial.print("Cycles per second second:");
        Serial.println(cycles*1000/(currentMillis - lastMillis));

        lastMillis = currentMillis;
        cycles = 0;
    }
}
