PATH=/home/skx/basic/arduino/kz80_grantz80/firmware/sbc_NascomBasic/asl-current:$$PATH

all: basic.hex intmini.hex

clean:
	/bin/rm -f *.hex *.lst *.p

basic.hex: basic.asm
	asl -cpu z80 -L basic.lst basic.asm
	p2hex -F C basic.p

intmini.hex: intmini.asm
	asl -cpu z80 -L intmini.lst intmini.asm
	p2hex -F C intmini.p
