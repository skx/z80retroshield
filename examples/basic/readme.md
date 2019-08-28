## BASIC

This example contains a BASIC interpreter which runs on the Z80!


## Overview

As with the other projects in this repository the Arduino is responsible
for handling all the RAM & port-based I/O requests.  The BASIC implementation
itself is written in 100% Z80 assembler.

The whole code is too large to fit into one contiguous array, so instead
the core of the project is in `PROGMEM`, with a small byte-array used to
simulate RAM.

The memory map looks like this:

* 0x0000 - 0x1DB2 - ROM (Containing "intmini" + "basic")
* 0x2000 - 0x37FF - RAM (BASIC code, variables, & etc)


## I/O Ports

The code I originally received handled I/O in a two-stage fashion:

* Port 0
  * For serial I/O, as with our other examples.
* Port 1
  * Reading this allows the Z80 to see if there is pending input.
  * Writing is used to configure the serial-port

I simplified this by removing the use of port 1, in `intmini.asm`
where the bootup/configuration magic occurs.

Note that this project is unusual in that it uses an interrupt (mode 1)
which is used to notify the code of pending serial-console input. The Z80
has several interrupt modes, here mode 1 is configured via the following code:

               IM        1
               EI

With interrupt mode one whenever an interrupt is triggered the processor will
jump to the hardwired address 0x38h.  In our case the code at 0x38h will read
a character from the serial console, and add it to the serial-console buffer.

The `basic.ino` sketch triggers the interrupt by activating the interrupt
pin (setting it low), if there is any pending serial-input.  This flag
is then reset later, when the byte has been read/consumed from the buffer.


## Running

Load the project via the arduino-studio as normal.  Once it launches you'll
be prompted to enter RAM-top - just press RETURN.  From there you get the
expected facilities.

You can enter code:

     10 PRINT "hello"
     LIST
     RUN

You can also see free RAM:

     PRINT SPC(0)

Empty memory via `NEW`, and reboot via `RESET`


## Compiling The Assembly

(Re)compiling the Z80 assembly is a bit of a hassle, but not impossible.

The first step is downloading the Macroassembler AS, which is used to compile
the code.  You can find that here:

* http://john.ccac.rwth-aachen.de:8000/as/download.html

Once you have it you'll be able to install it.  Assuming a Linux system:

    cp Makefile.def-samples/Makefile.def-k8-unknown-linux Makefile.def
    make

Once the binaries `asl` and `p2hex` are available and upon your PATH you can compile the `.asm` files by running `make`.  The two files are:

* `intmini.asm`
  * Initialization-code, interrupt-handler, etc.
* `basic.asm`
  * The interpreter.

You will receive two output files with a `.hex` suffix.  Copy and paste the
character-array contents into the appropriate place in the `basic.ino` sketch.

> **NOTE** You'll see there are several sections in the `intmini.hex` file, you should copy them all, in order.


## Credits

* Original BASIC code was written by Microsoft.
* Updates were made by Grant Searle.
* Further updates from 8bitforce.

My changes were to simplify the build-process, and to remove the use of
the second I/O port, simplifying serial I/O.
