# Opensource toolchain for AVR

AVR is a family of modified Harvard architecture 8-bit RISC microcontrollers, it was developed in the year 1996 by Atmel Corporation and acquired by Microchip Technology in 2016. 

The architecture of AVR was developed by Alf-Egil Bogen and Vegard Wollan. AVR derives its name from its developers and stands for Alf-Egil Bogen Vegard Wollan RISC microcontroller, also known as Advanced Virtual RISC. The AT90S8515 was the first microcontroller which was based on AVR architecture.

AVR microcontrollers are available in three categories:
- TinyAVR – Less memory, small size, suitable only for simpler applications
- MegaAVR – These are the most popular ones having good amount of memory (upto 256 KB), higher number of inbuilt peripherals and suitable for moderate to complex applications.
- XmegaAVR – Used commercially for complex applications, which require large program memory and high speed.

Maybe, the most famous development board using AVR is Arduino. Arduino is an AVR processor running special code that lets you use the Arduino environment to program and upload code easily. 

This tutorial is not a tutorial for Arduino development, it's for AVR opensource toolchain.

# 1. Hardware prerequist

* AVR development board:
  + This tutorial will use atmega128(ISP and JTAG)、atmega328(Arduino uno or nano)、attiny13(ISP and Debugwire)、atmega4808(UPDI)。
 
* Programmer: 
  + AVR ISP Programmer (either usbasp or usbtinyisp)
  + Or an arduino uno/nano board (which can be turn to a ISP programer)
  + Or CH340 USB to TTL adatper to support programming with debugwire/UPDI.
  + High voltage debugwire and UPDI programmer to rescue your “bricked” device.
  + All debugers mentioned below. 

* Debugger: 
  + AVR JTAG ICE and above for JTAG
  + or AVR JTAG ICE MKII and above for JTAG/debugwire
  + or AVR JTAG ICE 3 or ATMEL ICE or PICKIT4 for JTAG/debugwire/UPDI etc.
  + or USB to TTL adapter with a self-made adapter for [dwdebug](https://github.com/dcwbrown/dwire-debug).
 
**NOTE:**

- You'd better have an ISP programmer to program or change the FUSE bits.

- Changing FUSE bits is a little bit dangerous for beginners, it may 'brick' a device. For example, any ISP programmer is able to enable DWEN(debugwire FUSE bit), but if you want to turn it off, you have to use AVR DRAGON/ICE MKII and above or you have a High-Voltage programmer. For UPDI, if you set the pin to GPIO, you have to use HV UPDI programmer to program it.

- Not all arduino but uno/nano and most other models are AVR board with atmega mcu and suite for this tutorial. Arduino uno/nano have a USB bootloader to make programming easy (no additional hardwire required to program)  and can be turnned to a ISP programmer. that's to say, if you already have an arduino board, it's not necessary to buy ISP programmer anymore.

- Arduino uno/nano are lack of debugging support due to circuit design related to RESET pin(Debugwire pin for 328), you need modify the hardware to enable it (and do not do this).


- There are [various programming/debugging prototols](https://www.kanda.com/blog/microcontrollers/avr-microcontrollers/avr-microcontroller-programming-interfaces-isp-jtag-tpi-pdi-updi/) for different AVR models, such as ISP, JTAG/debugwire/PDI/UPDI, etc. Earlier version ICE devices may lack of support for some protocols，The latest official AVR ICE 3 or ATMEL ICE is always the best choice except the price.

- Pickit4 also support all avr debug protocols include hv updi support (which is not supported by ATMEL-ICE) after atmel was acquired by microchip, but lack of good opensource support except [pymcuprog](https://github.com/microchip-pic-avr-tools/pymcuprog) for avr mode. 


# 2. Toolchain overview

* Compiler: avr-gcc
* SDK: avr-libc
* Programer: avrdude/updiprog/pyupdi/pymcuprog
* Debugger: avarice/dwdebug/pyavrdbg, avr-gdb
* Simulator: simavr.


# 3. Compiler and SDK
AVR has very good support from opensource community, the opensource toolchain consists of 'avr-binutils'(binary utilities), 'avr-gcc'(compiler), 'avr-libc'(c libraries), 'avrdude'(the programmer), 'avarice'(the debug bridge) and 'avr-gdb'(the debugger). it's not necessary to build the toolchain yourself, since almost every linux distribution shipped this packages, just install these packages via pkg management tools of your distribution.

As usual, let's start with a blink example, below codes is well self-explained:

```
// main.c
// blink led every second
// wire: PB0->Resistor-> LED->GND
// for Arduino user: PB0 is the 'D8' pin.

#include <avr/io.h>

#ifndef F_CPU
#define F_CPU 8000000UL // change F_CPU according to clock speed, used by delay.h
#endif
#include <util/delay.h>

// 1 sec if F_CPU set up correctly.
#define MS_DELAY 1000

int main (void) {

    DDRB |= 0x01;// PB0 as OUTPUT

    PORTB &= 0xFE; // PB0 to 0

    while(1) {
        PORTB ^= 0x01; // toggle PB0
        _delay_ms(MS_DELAY);
    }
}
```

Save it to 'main.c' and build:

```
avr-gcc -c -g -Os -mmcu=<MCU TYPE> main.c -o main.o
avr-gcc -mmcu=<MCU TYPE>  main.o -o main.elf
avr-objcopy -O ihex main.elf main.hex
```

You should change the `<MCU TYPE>` according to your board. for mcu type avr-gcc already supported, please refer to: https://gcc.gnu.org/onlinedocs/gcc/AVR-Options.html.

The blink example in this repo provide a configurable 'Makefile', you can change the 'MCU_TYPE' defined in 'Makefile'.


# 4. Programming

After 'main.hex' generated, there are various way to program 'main.hex' to AVR MCU. 

## 4.1 with avrdude

**NOTE:** if you use Dragon/JTAG ICE/ATMEL-ICE as programmer, it does NOT supply power to target board, you have to supply power to the target board separately.

As mentioned above, the software we mostly used to program AVR MCU is `avrdude`, it supports a lot of programmers include but not limited to usbasp, usbtinyisp, AVR Dragon, JTAG ICE/MKII/3, ATMEL-ICE and PICKIT4. It also can directly program Arduino with arduino bsl. The common usage of avrdude looks like as below.

To detect target MCU:
```
sudo avrdude -c <programmer> -p <target>
```

To program target MCU:

```
sudo avrdude -c <programmer> -p <target> -U flash:w:<hex file>
```

The `<programer>` can be:
* usbasp
* usbtiny
* atmelice/\_isp/\_dw/\_pdi/\_updi for ATMEL-ICE
* ...

The `<target>` can be:
* m128 for atmega128
* m328p for atmega328p (Arduino uno/nano)
* t13 for attiny13
* t85 for attiny85
* m4808 for atmega4808 (Thinary nano 4808)
* m4809 for atmega4809 (Arduino nano every)

For all programmers and targets 'avrdude' can support, try run:

```
sudo avrdude -c help
sudo avrdude -p help
```

## 4.2 with dwdebug

[dwdebug](https://github.com/dcwbrown/dwire-debug) is a simple stand-alone programmer and debugger for AVR processors that support DebugWIRE.

I prefer this way to program and debug low pin attiny MCUs. If your target MCU support debugwire protocol, such as attiny13, you can program 'DWEN' FUSE bit to enable debugwire with any ISP programmer.

**NOTE 1: Any ISP programmer can program 'DWEN' FUSE bit, but after debugwire enabled, if you want to unprogram 'DWEN' FUSE bit, you have to use AVR Dragon/AVR JTAG ICE MKII and above, or HV ISP (supported by AVR Dragon).**

**NOTE 2: Never touch the 'RSTDISBL' FUSE bit (keep it 1 always), unless you really understand what you are doing and you really have a High Voltage programmer.**


**4.2.1 program DWEN FUSE bit**

Use attiny13 as example, according to the datasheet:

![screenshot-2022-05-29-14-30-31](https://user-images.githubusercontent.com/1625340/170855409-7c0eec2f-0811-4638-8506-188cc71e84ff.png)

the default value of hfuse is '0xff', and set it to '0xf7' means debugwire enabled:

```
sudo avrdude -c usbasp -p t13 -U hfuse:w:0xf7:m
```

If you have AVR dragon and above programmer, you can unprogram 'DWEN':

```
sudo avrdude -c usbasp -p t13 -U hfuse:w:0xff:m
```

**4.2.2 prepare the hardware**

dwdebug uses an FT232R or CH340 USB serial adapter with RX connected directly to the DebugWIRE pin(the RESET pin now is debugwire pin), and TX connected through a 4.7k resistor to RX.
```                     
 +---------------------+                           +--------------------+
 |                 VCC +---------------------------+ VCC                |
 |                  TX +---+-----------------------+ DebugWire          |
 |                     |   |                       |                    |
 |  CH340/FTx232       |   R(4.7k)                 |   AVR Tiny device  |
 |                     |   |                       |                    |
 |                  RX +---+                       |                    |    
 |                     |                           |                    |
 |                 GND +---+-----------------------+ GND                | 
 +---------------------+                           +--------------------+
```
                         
For convenient, I make a little board easy to fit my CH340 and FT2232 adapter:

<img src="https://user-images.githubusercontent.com/1625340/170856074-ef342ae1-792f-413e-91f0-3448a62a5bfe.png" width="50%"/>

**4.2.3 program by dwdebug**

```
sudo dwdebug l ./main.elf,qr
```

For more usage help of dwdebug, please refer to [dwdebug manual](https://github.com/dcwbrown/dwire-debug/blob/master/Manual.md).

## 4.3 with updiprog

## 4.4 with pymcuprog



# 5. Debugging

## 5.1 with avarice

AVaRICE is a program which interfaces the GNU Debugger GDB with the AVR JTAG ICE available from Atmel. It can support a lot of debugger devices, such as AVR JTAG ICE MKI/MKII/3, AVR Dragon and ATMEL-ICE etc.

Up to this tutorial written, most linux distribution shipped avarice-2.13 by default, version 2.13 can not support ATMEL-ICE, if you use ATMEL-ICE, you can use this version: https://github.com/Florin-Popescu/avarice/

Build and install it:

```
./configure --prefix=/usr/local --program-suffix=-2.14 --datadir=/usr/local/share/avarice-2.14
make
sudo make install
```

The program suffix and another datadir is to avoid conflict with avarice shipped by your distribution.

After installed, the command you can use is `avarice-2.14`.

Try run below command to find help information and determine the command args you should use according to your debugger hardware:

```
avarice --help
```

Using atmega128 as example, after programming with `avrdude`, wire up the debugger and target board correctly (usually, there are two 10pin header in your board, one is ISP and another one is JTAG), and open the terminal to luanch a avarice session,

For JTAG ICE, you may need to specify the port, here is /dev/ttyUSB0:

```
avarice-2.14 -j /dev/ttyUSB0 :3333
```
And the output looks like
```
AVaRICE version 2.13, May 24 2022 00:00:00

Defaulting JTAG bitrate to 250 kHz.

JTAG config starting.
Hardware Version: 0xc0
Software Version: 0x80
Reported JTAG device ID: 0x9702
Configured for device ID: 0x9702 atmega128
JTAG config complete.
Preparing the target device for On Chip Debugging.
Waiting for connection on port 3333.
```

For ATMEL-ICE, you should use:

```
avarice-2.14 -4 --program --file ./main.elf :3333
```

And the output looks like:

```
AVaRICE version 2.14

Defaulting JTAG bitrate to 250 kHz.

JTAG config starting.
Found a device, serial number: J42700018439
Reported device ID: 0x9702
Configured for device ID: 0x9702 atmega128
JTAG config complete.
Preparing the target device for On Chip Debugging.
Waiting for connection on port 3333.
```

And open another terminal to start avr-gdb as:

```
avr-gdb ./main.elf
Reading symbols from main.elf...
(gdb) target remote :3333
Remote debugging using :3333
0x00000000 in __vectors ()
(gdb) break main
Breakpoint 1 at 0xa4: file main.c, line 16.
(gdb) break 16
Note: breakpoint 1 also set at pc 0xa4.
Breakpoint 2 at 0xa4: file main.c, line 16.
(gdb) c
Continuing.
```

**NOTE:** the process above use JTAG debug protocol. if the target use debugwire protocol, apply '-w' arg to avarice. if it's updi protocol, apply '-u' arg.


## 5.2 with dwdebug

For AVR MCU supporting debugwire protocol, you can use `dwdebug` with self-made CH340/FTx232 adapter mentioned above for debugging. dwdebug itself is a debugger can be used standalone, and it can also work as a bridge to avr-gdb.

I prefer this way to debug some ATTINY models

After program the target device with:

```
dwdebug l ./main.elf,qr
```

Launch the dwdebug session with:

```
dwdebug device ttyUSB0
```

The output looks like:
```
Connected to ATtiny13 on /dev/ttyUSB0 at 9495 baud.
0036: 4030  sbci  r19, $0               > 
```

After you get the '>' prompt, type 'help' for more info and launch gdb-server as:

```
0034: 5021  subi  r18, $1               > gdbserver
Target ready, waiting for GDB connection.

Info : avrchip: hardware has something
Use 'target remote :4444'
```

All REPL command of dwdebug can be used as commandline argument, to launch gdbserver, you can also do it as:
```
$ dwdebug gdbserver
```


# 6. how to update USBASP firmware

If **"avrdude : warning : Can not Set sck period . usbasp please check for firmware update"**, it means your usbasp adapter's firmware is outdated. it should still works well, it is NOT neccesary to update the firmware. but if you insist to do that, please follow this guide:

**6.1. Download the USBASP firmware**

Download the firmware from https://www.fischl.de/usbasp/, the lastest version is "https://www.fischl.de/usbasp/usbasp.2011-05-28.tar.gz". there are 3 hex files in 'bin/firmware' dir, choose the one according to your usbasp adapter. since most of them is atmega8, that's to say, you should use "usbasp.atmega8.2011-05-28.hex" for atmega8.


**6.2. Prepare another workable ISP progammer**

To flash the new firmware onto the target atmega8, we need another ISP programmer. either usbasp adapter or usbtinyisp adapter or arduino board is OK.

If you use arduino as ISP programmer, you need:

* Download arduino IDE.
* Go to "File" > "Examples" > "ArduinoISP"
* Connect an Arduino board to your PC
* Select the port and board type and "Click" on "Upload"

It will turn your arduino board to ISP programmer.

**6.3. Gain control of the chip's RESET pin**

The 'RESET' pin of the ISP header on usbasp adapter need to be updated is not connect to the atmega8 chip's RESET pin, thus you can not program the adapter until gain control of the chip's RESET pin.

Usually, the usbasp programmer need to be updated should have a Jumper "JP2" on board as:

<img src="https://user-images.githubusercontent.com/1625340/170300994-dec83783-461e-478b-b673-723484030026.png" width=50%/>

**This jumper need to be closed to connect the "RESET" pin of ISP header to atmega8 chip's RESET pin (pin 29) to enable self-programming.**

There are a lot of cheap and old USBASP programmer do not have such a Jumper JP2 on board, such as:

<img src="https://user-images.githubusercontent.com/1625340/170302304-7ddabb6f-31f7-43db-9a34-5ad9db00800d.jpg" width=50%/>

If you have such a programmer need to be updated, you have to solder a wire to the chip's RESET pin or use test hook to connect a wire to it. **this wire will be used as RESET pin to program the target usbasp adapter later", note the RESET pin of atmega8 is pin 29:

![atmega8-pin](https://user-images.githubusercontent.com/1625340/170304456-496d3b60-cc4b-4109-b1e4-f1ad015040f0.png)

**6.4. Wire up ISP programmer and target usbasp adapter**

If you have another usbasp or usbtinyisp programmer, just wire them up as usual. If there is no JP2 jumper on board, you need connect the programmer's RESET pin directly to target atmega8 chip's RESET pin.

If you use arduino as ISP programmer, you need connect the Arduino to target usbasp adatper with the following connections:

* Arduino 5v to target VCC
* Arduino GND to target GND
* Arduino D13 to target SCK
* Arduino D12 to target MISO
* Arduino D11 to target MOSI
* Arduino D10 to target RESET(if has JP2 jumper) or to the wire from target chip's PIN 29.

**6.5. Detect the target adapter**

Run:

```
avrdude -p m8 -c <your programmer> -P /dev/ttyUSB0
```

to detect the target device. If everything goes right, avrdude should print out:

```
avrdude: AVR device initialized and ready to accept instructions

Reading | ################################################## | 100% 0.02s

avrdude: Device signature = 0x1e9307 (probably m8)

avrdude: safemode: Sorry, reading back fuses was unreliable. I have given up and exited programming mode

avrdude done.  Thank you.
```

**NOTE:**

to use arduino as ISP programmer, you need to use `<Where your Arduino IDE>/hardware/tools/avr/etc/avrdude.conf` with avrdude as:

```
avrdude -C <Where your Arduino IDE>/hardware/tools/avr/etc/avrdude.conf -p m8 -c avrisp -P /dev/ttyUSB0
```

**6.6. Finally, update the firmwire**
With a working connection to the target ATmega8,

```
avrdude -p m8 -c <your programmer> -P /dev/ttyUSB0 -b 19200 -U flash:w:usbasp.atmega8.2011-05-28.hex:i
```

to use arduino as ISP programmer, you still need to use `-c avrisp -C <Where your Arduino IDE>/hardware/tools/avr/etc/avrdude.conf`.




  
 
