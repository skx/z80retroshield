# Z80RetroShield

The [RetroShield](http://www.8bitforce.com/projects/retroshield/) comes in a Z80 flavour, complete with a small amount of [sample code](https://gitlab.com/8bitforce/retroshieldz80).

However the sample code mixes the actual Z80-usage with some unrelated things, such as button-scanning, LCD display, and SPI-RAM access which makes it harder to use as a standalone "thing".

This repository aims to make the retroshield easier to deal with, by abstracting the CPU-driving into a single standalone class, and packaging it as a library for the Arduino IDE.  This should allow you to use it in your arduino-sketch along with whatever else you wish to do.

The official example code for the Z80 retroshield contains two examples, one useing the `loop()`-based approach this repository contains, the other uses a timer-function to drive the processor.  Using a timer is cleaner since it gives you more speed and predictability, however the execution becomes more complex because you can't do things like invoke `Serial.read()`/`Serial.write()` inside an interrupt-handler - though you can disable the time for the duration of your "stuff".

In short running things in `loop()` is more predictable, but slower, so that is what this library prefers.


## Examples

The examples included within this library should be sufficient to demonstrate how to use the class, but in brief you:

* Instantiate an instance of the Z80RetroShield object.
* Configure a small number of callbacks
  * Which are fired to read/write RAM and handle I/O.
* Call `Tick()` to allow the Z80 processor to execute.

You can view the examples here, and should be able to select them from your Arduino-IDE:

* [ram-test.ino](examples/ram-test/ram-test.ino)
  * Tests a simple program for reading/writing to RAM.
* [hello.ino](examples/hello/hello.ino)
  * A simple example which writes output to the serial-console.
* [uc.ino](examples/uc/uc.ino)
  * Read from serial-console, and output the input in upper-case.

Steve
--
