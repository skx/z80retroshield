# Z80RetroShield

The [RetroShield](http://www.8bitforce.com/projects/retroshield/) is an addon/shield for the Arduino Mega which allows your arduino to drive, control, and interact with a _real_ processor.

This library is designed to work with the Z80-based retroshield, allowing you to experiment and build projects which use a Z80.


## Motivation

The retroshield project contains some [sample code](https://gitlab.com/8bitforce/retroshieldz80), however that sample code mixes the actual Z80-usage with some unrelated things, (button-scanning, LCD display, SPI-RAM access etc), which makes it harder to use as a standalone "thing".

This repository aims to make the Z80 retroshield easier to deal with, by abstracting the CPU-driving into a single standalone class, and packaging it as a library for the Arduino IDE.  This should allow you to use it in your arduino-sketch along with whatever else you wish to do.


## Usage

The examples included within this library should be sufficient to demonstrate how to use the class, but in brief you:

* Instantiate an instance of the Z80RetroShield object.
* Configure a small number of callbacks
  * Which are fired to read/write RAM and handle I/O.
* Call `Tick()` to allow the Z80 processor to execute.


## Examples

You can view the examples by exploring the repository, and the examples should be visible via the Arduino-IDE interface:

* [ram-test.ino](examples/ram-test/ram-test.ino)
  * Tests a simple program for reading/writing to RAM.
* [hello.ino](examples/hello/hello.ino)
  * A simple example which writes a message to the serial-console.
* [uc.ino](examples/uc/uc.ino)
  * Read from serial-console, and output the input in upper-case.


## Links

Now some Z80 links

* Sample Z80 code which contains similar code to the examples here:
  * [https://github.com/skx/z80-examples](https://github.com/skx/z80-examples)
* A Z80 emulator which will let you run the examples:
  * [https://github.com/skx/z80emulater/](https://github.com/skx/z80emulater/).
  * This emulator has been updated so that `in (1)` and `out (1)` handle STDIN/STDOUT as the examples do.
* My blog posts on the topic of creating a Z80-based computer:
  * [https://blog.steve.fi/tags/z80/](https://blog.steve.fi/tags/z80/)


Steve
--
