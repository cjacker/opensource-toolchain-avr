# Opensource toolchain for AVR

AVR is a family of modified Harvard architecture 8-bit RISC microcontrollers, it was developed in the year 1996 by Atmel Corporation and acquired by Microchip Technology in 2016. 

The architecture of AVR was developed by Alf-Egil Bogen and Vegard Wollan. AVR derives its name from its developers and stands for Alf-Egil Bogen Vegard Wollan RISC microcontroller, also known as Advanced Virtual RISC. The AT90S8515 was the first microcontroller which was based on AVR architecture.

AVR microcontrollers are available in three categories:
- TinyAVR – Less memory, small size, suitable only for simpler applications
- MegaAVR – These are the most popular ones having good amount of memory (upto 256 KB), higher number of inbuilt peripherals and suitable for moderate to complex applications.
- XmegaAVR – Used commercially for complex applications, which require large program memory and high speed.

Maybe, the most famous development board using AVR is Arduino. Arduino is an AVR processor running special code that lets you use the Arduino environment to program and upload code easily. 

This tutorial is not a tutorial for Arduino development, it's for AVR opensource toolchain.

# Hardware prerequist

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

# Arduino pin map
Arduino board names all pins from D0 to D13 and A0 to A7. the corresponding pin name of AVR you can use is listed as below table:

| Arduino uno/nano pin name | atmega328 pin |
|---------------------------|---------------|
| D0                        | PD0           |
| D1                        | PD1           |
| D2                        | PD2           |
| D3                        | PD3           |
| D4                        | PD4           |
| D5                        | PD5           |
| D6                        | PD6           |
| D7                        | PD7           |
| D8                        | PB0           |
| D9                        | PB1           |
| D10                       | PB2           |
| D11                       | PB3           |
| D12                       | PB4           |
| D13                       | PB5           |
| A0                        | PC0           |
| A1                        | PC1           |
| A2                        | PC2           |
| A3                        | PC3           |
| A4                        | PC4           |
| A5                        | PC5           |
| A6                        | PC6           |
| A7                        | PC7           |

# Toolchain overview

* Compiler: avr-gcc
* SDK: avr-libc
* Programer: avrdude/updiprog/pyupdi/pymcuprog
* Debugger: avarice/dwdebug/pyavrdbg, avr-gdb
* Simulator: simavr.


# Compiler and SDK
AVR has very good support from opensource community, the opensource toolchain consists of 'avr-binutils'(binary utilities), 'avr-gcc'(compiler), 'avr-libc'(c libraries), 'avrdude'(the programmer), 'avarice'(the debug bridge) and 'avr-gdb'(the debugger). it's not necessary to build the toolchain yourself, since almost every linux distribution shipped this packages, just install these packages via pkg management tools of your distribution.

As usual, let's start with a blink example, below codes is well self-explained:

```
// main.c
// blink led every second
// wire: PB0(Arduino D8)->Resistor-> LED->GND

#include <avr/io.h>

#ifndef F_CPU
#define F_CPU 8000000UL // change it to clock speed, used by delay.h
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

**NOTE:**, change the `<MCU TYPE>` to the model you use, for mcu type avr-gcc already supported, please refer to: https://gcc.gnu.org/onlinedocs/gcc/AVR-Options.html

The blink example in this repo provide a configurable 'Makefile', you can change the 'MCU_TYPE' in 'Makefile'.


# Programming

After 'main.hex' generated, there are various way to program 'main.hex' to AVR. 

## by avrdude

**NOTE:** if you use ATMEL-ICE, it does NOT supply power to target board, you have to supply power to the target board separately.

As mentioned above, the software we mostly use to program AVR MCU is avrdude, it can support a lot of programmers include but not limited to usbasp, usbtinyisp, AVR Dragon, ATMEL-ICE and PICKIT4. It also can directly program Arduino via USB cable. The common usage of avrdude looks like,

to detect target MCU:
```
sudo avrdude -c <programmer> -p <target>
```

to program target MCU:

```
sudo avrdude -c <programmer> -p <target> -U flash:w:<hex file>
```

The `<programer>` can be:
* usbasp
* usbtiny
* atmelice/isp/dw/pdi/updi for ATMEL-ICE
* ...

The `<target>` can be:
* m128 for atmega128
* m328p for atmega328p
* t13 for attiny13
* t85 for attiny85

For all programmers and targets 'avrdude' can support, try run:

```
sudo avrdude -c help
sudo avrdude -p help
```

## by dwdebug

[dwdebug](https://github.com/dcwbrown/dwire-debug) is a simple stand-alone programmer and debugger for AVR processors that support DebugWIRE.

If your target MCU support debugwire protocol, such as attiny13/85, you can set 'DWEN' FUSE bit to enable debugwire by any ISP programmer.

**NOTE: Any ISP programmer can program 'DWEN' FUSE bit, but after debugwire enabled, to unprogram 'DWEN' FUSE bit, you have to use AVR Dragon/AVR JTAG ICE MKII and above, or HV ISP (supported by AVR Dragon).**

Usually, such a programmer is more expensive than a MCU chip. If you only have a USBASP/USBTINY ISP programmer, you must understand the satuation here before do anything. And I suggest buying another chip instead of buying a high-end programmer if you wan to use ISP again.

### Program DWEN FUSE bit
If your target MCU support debugwire protocol, you can enable it and use debugwire protocol to program.

Take attiny 13 as example, according to the datasheet of attiny13:

![screenshot-2022-05-29-14-30-31](https://user-images.githubusercontent.com/1625340/170855409-7c0eec2f-0811-4638-8506-188cc71e84ff.png)

the default value of hfuse is '0xff', and set it to '0xf7' means debugwire enabled:

```
sudo avrdude -c usbasp -p t13 -U hfuse:w:0xf7:m
```

If you have AVR dragon and above programmer, you can disable debugwire by:

```
sudo avrdude -c usbasp -p t13 -U hfuse:w:0xff:m
```

### prepare the hardware

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
                         
For convenient, I make a board easy to plug into my CH340 and FT2232 adapter directly:

<img src="https://user-images.githubusercontent.com/1625340/170856074-ef342ae1-792f-413e-91f0-3448a62a5bfe.png" width="50%"/>

### program

```
sudo dwdebug l ./main.elf,qr
```
For more help of dwdebug, please refer to [dwdebug manual](https://github.com/dcwbrown/dwire-debug/blob/master/Manual.md).


# Debugging


# how to update USBASP firmware

If **"avrdude : warning : Can not Set sck period . usbasp please check for firmware update"**, it means your usbasp adapter's firmware is outdated. it should still works well, it is NOT neccesary to update the firmware. but if you insist to do that, please follow this guide:

**1. Download the USBASP firmware**

Download the firmware from https://www.fischl.de/usbasp/, the lastest version is "https://www.fischl.de/usbasp/usbasp.2011-05-28.tar.gz". there are 3 hex files in 'bin/firmware' dir, choose the one according to your usbasp adapter. since most of them is atmega8, that's to say, you should use "usbasp.atmega8.2011-05-28.hex" for atmega8.


**2. Prepare another workable ISP progammer**

To flash the new firmware onto the target atmega8, we need another ISP programmer. either usbasp adapter or usbtinyisp adapter or arduino board is OK.

If you use arduino as ISP programmer, you need:

* Download arduino IDE.
* Go to "File" > "Examples" > "ArduinoISP"
* Connect an Arduino board to your PC
* Select the port and board type and "Click" on "Upload"

It will turn your arduino board to ISP programmer.

**3. Gain control of the chip's RESET pin**

The 'RESET' pin of the ISP header on usbasp adapter need to be updated is not connect to the atmega8 chip's RESET pin, thus you can not program the adapter until gain control of the chip's RESET pin.

Usually, the usbasp programmer need to be updated should have a Jumper "JP2" on board as:

<img src="https://user-images.githubusercontent.com/1625340/170300994-dec83783-461e-478b-b673-723484030026.png" width=50%/>

**This jumper need to be closed to connect the "RESET" pin of ISP header to atmega8 chip's RESET pin (pin 29) to enable self-programming.**

There are a lot of cheap and old USBASP programmer do not have such a Jumper JP2 on board, such as:

<img src="https://user-images.githubusercontent.com/1625340/170302304-7ddabb6f-31f7-43db-9a34-5ad9db00800d.jpg" width=50%/>

If you have such a programmer need to be updated, you have to solder a wire to the chip's RESET pin or use test hook to connect a wire to it. **this wire will be used as RESET pin to program the target usbasp adapter later", note the RESET pin of atmega8 is pin 29:

![atmega8-pin](https://user-images.githubusercontent.com/1625340/170304456-496d3b60-cc4b-4109-b1e4-f1ad015040f0.png)

**4. Wire up ISP programmer and target usbasp adapter**

If you have another usbasp or usbtinyisp programmer, just wire them up as usual. If there is no JP2 jumper on board, you need connect the programmer's RESET pin directly to target atmega8 chip's RESET pin.

If you use arduino as ISP programmer, you need connect the Arduino to target usbasp adatper with the following connections:

* Arduino 5v to target VCC
* Arduino GND to target GND
* Arduino D13 to target SCK
* Arduino D12 to target MISO
* Arduino D11 to target MOSI
* Arduino D10 to target RESET(if has JP2 jumper) or to the wire from target chip's PIN 29.

**5. Detect the target adapter**

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

**6. Finally, update the firmwire**
With a working connection to the target ATmega8,

```
avrdude -p m8 -c <your programmer> -P /dev/ttyUSB0 -b 19200 -U flash:w:usbasp.atmega8.2011-05-28.hex:i
```

to use arduino as ISP programmer, you still need to use `-c avrisp -C <Where your Arduino IDE>/hardware/tools/avr/etc/avrdude.conf`.




  
 
