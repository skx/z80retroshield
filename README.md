# Z80RetroShield

This library contains code for interfacing with the Z80 RetroShield available for purchase from:

* http://www.8bitforce.com/projects/retroshield/

To install clone/download into `~/Arduino/libraries` folder.  Once you've done
that you should find the examples available the next time you start your
arduino IDE.


## Overview


The [RetroShield](http://www.8bitforce.com/projects/retroshield/) comes in a Z80 flavour, complete with a small amount of [sample code](https://gitlab.com/8bitforce/retroshieldz80).

However the sample code mixes the actual Z80-usage with some unrelated things, such as button-scanning, LCD display, and SPI-RAM access which makes it harder to use as a standalone "thing".

This repository aims to make the retro-shield easier to deal with, by abstracting the CPU-driving into a single standalone class.  Allowing you to use it in your arduino-sketch along with whatever else you wish to do.

There are currently two pieces of sample-code for the Z80 retroshield, one uses the `loop()`-based approach this repository contains, the other uses a timer-function to drive the processor.  Using a timer is cleaner since it gives you more speed and predictability, however the execution becomes more complex because you can't do things like invoke `Serial.read()`/`Serial.write()` inside an interrupt-handler - though you can disable the time for the duration of your "stuff".

In short running things in `loop()` is more predictable, but slower.

### Examples

The following examples demonstrate how to use the class, but in brief you:

* Instantiate an instance of the Z80RetroShield object.
* Configure a small number of callbacks
  * Which are fired to read/write RAM
  * And to read/write output
* Call `Tick()` to allow the Z80 processor to execute.

These are the examples included in the library:

* [ram-test.ino](examples/ram-test/ram-test.ino)
  * Tests a simple program for reading/writing to RAM
* [hello.ino](examples/hello/hello.ino)
  * A simple example which writes output to the serial-console.
