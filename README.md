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
  + This tutorial will use atmega128(ISP and JTAG)、atmega328(Arduino nano)、attiny13(ISP and debugwire)、atmega4808(UPDI)。
 
* Programmer: 
  + AVR ISP Programmer (either usbasp or usbtiny)
  + Or an arduino uno/nano board (which can be turn to a ISP programer)
  + Or CH340 serial USB adatper to support programming with debugwire/UPDI.
  + [Optional] Or High voltage debugwire and UPDI programmer to rescue your “bricked” device.
  + Or All debugers mentioned below. 

* Debugger: 
  + AVR JTAG ICE and above for JTAG
  + or AVR JTAG ICE MKII and above for JTAG/debugwire
  + or AVR JTAG ICE 3 / ATMEL ICE / PICKIT4 for all debugging protocol include UPDI
  + or CH340 serial USB adapter with a self-made adapter for [dwdebug](https://github.com/dcwbrown/dwire-debug).
 
**NOTE:**

- You'd better have an ISP programmer to program or change the FUSE bits.

- Changing FUSE bits is a little bit dangerous for beginners, it may 'brick' a device. For example, any ISP programmer is able to program DWEN debugwire FUSE bit. but if you want to unprogram it, you have to use AVR DRAGON and above devices, or you have a High-Voltage programmer. For UPDI, if you set the UPDI pin to GPIO, you have to use HV UPDI programmer to unprogram it. for attiny FUSE rescue, please refer to the section "how to make a debugwire FUSE rescue board". 

- Not all arduino but most of them are AVR board and suite for this tutorial. Arduino uno/nano have a USB bootloader to make programming easy (no additional hardwire required to program)  and can be turnned to a ISP programmer. that's to say, if you already have an arduino board, it's not necessary to buy ISP programmer anymore.

- Arduino uno/nano are lack of debugging support due to circuit design related to RESET pin, you need modify the hardware to enable it (and do not do this).

- There are [various programming/debugging prototols](https://www.kanda.com/blog/microcontrollers/avr-microcontrollers/avr-microcontroller-programming-interfaces-isp-jtag-tpi-pdi-updi/) for different AVR models, such as ISP, JTAG/debugwire/PDI/UPDI, etc. Earlier version of ICE devices may lack of support for some protocols，The latest official ATMEL ICE is always the best choice except the price.

- Pickit4 also support all avr debug protocols include HV UPDI support (which is not supported by ATMEL-ICE) after atmel was acquired by microchip, but lack of good opensource support except avadude and [pymcuprog](https://github.com/microchip-pic-avr-tools/pymcuprog) for avr mode. 


# 2. Toolchain overview

* Compiler: avr-gcc
* SDK: avr-libc
* Programer: avrdude/dwdebug/pyupdi/pymcuprog
* Debugger: avarice/dwdebug/bloom and avr-gdb
* [optional]Simulator: simavr.


# 3. Compiler and SDK

AVR has very good support from opensource community, the opensource toolchain consists of **avr-binutils**(binary utilities), **avr-gcc**(compiler), **avr-libc**(c libraries), **avrdude**(the programmer), **avarice**(the debug bridge) and **avr-gdb**(the debugger). it's not necessary to build the toolchain yourself, since almost every linux distribution shipped these packages, just install them with pkg management tools of your distribution.

If you want to find a prebuilt toolchain, the AVR toolchain from Arduino IDE (at `<ide dir>/hardware/tools/var`) is the best choice.

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

As mentioned above, the software we mostly used to program AVR MCU is `avrdude`, it supports almost all AVR programmers, and can directly program Arduino with arduino bsl. The common usage of avrdude looks like as below.

To detect target MCU:
  
```
sudo avrdude -c <programmer> -p <target>
```
  
If you use AVR Dragon/JTAG ICE MKxx/ATMEL-ICE, you may need to supply power to target device seperately.
  
  
To program target MCU:

```
sudo avrdude -c <programmer> -p <target> -U flash:w:<hex file>
```

The `<programer>` can be:
* usbasp
* usbtiny
* atmelice/\_isp/\_dw/\_pdi/\_updi for ATMEL-ICE
* arduino

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

I prefer this way to program and debug low pin attiny MCUs with debugwire. If your target MCU support debugwire protocol, such as attiny13, you can program 'DWEN' FUSE bit to enable debugwire with any ISP programmer.

**NOTE 1: Any ISP programmer can program 'DWEN' FUSE bit, but after debugwire enabled, if you want to unprogram 'DWEN' FUSE bit, you have to use AVR Dragon/AVR JTAG ICE MKII and above, or HV ISP (supported by AVR Dragon).**

**NOTE 2: Never touch the 'RSTDISBL' FUSE bit (keep it 1 always), unless you really understand what you are doing and you really have a High Voltage programmer.**

**NOTE 3: If you do not have any HV programmer, you can made a rescue board to restore the FUSE bit according to https://www.electronics-lab.com/recover-bricked-attiny-using-arduino-as-high-voltage-programmer/.**


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
 |                  TX +---+-----------------------+ DebugWire/UPDI     |
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

## 4.3 with pyupdi

With the self-made '4.7k serial adapter' (refer to above section), you can program UPDI with [pyupdi](https://github.com/mraardvark/pyupdi).

Use Thinary nano 4808 and blink demo as example:

```
pyupdi -c /dev/ttyUSB0 -d atmega4808 -b 115200 -e  -f main.hex 
```

## 4.4 with pymcuprog
pymcuprog is a Python utility for programming various Microchip MCU devices using Microchip CMSIS-DAP based debuggers.

pymcuprog is primarily intended for use with PKOB nano (nEDBG) debuggers which are found on Curiosity Nano kits and other development boards. This means that it is continuously tested with a selection of AVR devices with UPDI interface as well as a selection of PIC devices. However since the protocol is compatible between all EDBG-based debuggers (pyedbglib) it is possible to use pymcuprog with a wide range of debuggers and devices, although not all device families/interfaces have been implemented.

**pymcuprog supports:**

* PKOB nano (nEDBG) - on-board debugger on Curiosity Nano
* MPLAB PICkit 4 In-Circuit Debugger (when in 'AVR mode')
* MPLAB Snap In-Circuit Debugger (when in 'AVR mode')
* Atmel-ICE
* Power Debugger
* EDBG - on-board debugger on Xplained Pro/Ultra
* mEDBG - on-board debugger on Xplained Mini/Nano
* JTAGICE3 (firmware version 3.0 or newer)


**Install using pip from pypi:**

```
pip install pymcuprog
```

**Usage:**

Test connectivity by reading the device ID:
```
pymcuprog ping
```

Erase memories then write contents of an Intel(R) hex file to flash using Curiosity Nano (pymcuprog does NOT automatically erase before writing):
```
pymcuprog erase
pymcuprog write -f app.hex
```

Erase memories and write an Intel hex file (using the --erase switch):
```
pymcuprog write -f app.hex --erase
```

Erase memories, write an Intel hex file and verify the content:
```
pymcuprog write -f app.hex --erase --verify
```


Pymcuprog also support self-made '4.7k serial adapter' (refer to above section), 

For example: checks connectivity by reading the device identity:
```
pymcuprog ping -d atmega4808 -t uart -u /dev/ttyUSB0
```

Erase and write:
```
pymcuprog -t uart -d atmega4808 -u /dev/ttyUSB0 write -f main.hex --erase --verify
```

  
# 5. Debugging

## 5.1 with avarice

AVaRICE is a program which interfaces the GNU Debugger GDB with the AVR JTAG ICE available from Atmel. It can support a lot of debugger devices, such as AVR JTAG ICE MKI/MKII/3, AVR Dragon and ATMEL-ICE etc.

Up to this tutorial written, most linux distribution shipped avarice-2.13 by default, version 2.13 can not support ATMEL-ICE, if you use ATMEL-ICE, you should use this version: https://github.com/Florin-Popescu/avarice/

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
avarice-2.14 --help
```

Using atmega128 as example, after programming with `avrdude`, wire up the debugger and target board correctly (usually, there are two 10pin headers in your board, one for ISP and another one is JTAG), and open the terminal to luanch a avarice session,

For JTAG ICE, you may need to specify the port, here is '/dev/ttyUSB0':

```
avarice-2.14 -j /dev/ttyUSB0 :3333
```
  
And the output looks like
  
```
AVaRICE version 2.14dev

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
avarice-2.14 -4 :3333
```

And the output looks like:

```
AVaRICE version 2.14dev

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

**NOTE:** the above command is for JTAG debugging protocol, apply '-w' arg for debugwire and '-u' for UPDI.


## 5.2 with dwdebug

For AVR MCU supporting debugwire protocol, you can use `dwdebug` with self-made CH340/FTx232 adapter mentioned above for debugging. dwdebug itself is a debugger can be used standalone, and it can also work as a bridge to avr-gdb.

I prefer this way to debug some lowpin ATTINY models

Program the target device with:

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


## 5.3 with bloom

Bloom implements a number of user-space device drivers, enabling support for many debug tools (such as the Atmel-ICE, Power Debugger, MPLAB SNAP, etc). Bloom exposes an interface to the connected target, via a GDB RSP server. This allows any IDE with GDB RSP client capabilities to interface with Bloom and gain full access to the target.

Currently, Bloom only supports AVR8 targets from Microchip. Bloom was designed to accommodate targets from different families and architectures. Support for other target families will be considered as and when requested.

You can download the prebuilt binary release from https://bloom.oscillate.io, it provides deb and rpm packages.

After installed, in your project dir, run:

```
bloom init
```

A 'bloom.json' will be generated, you need to modify this file to match your target, use thinary nano 4808 as example, 'bloom.json' should be:

```
{
  "environments": {
    "default": {
      "debugTool": {
        "name": "atmel-ice",
        "releasePostDebugSession": true
      },

      "target": {
        "name": "atmega4808",
        "physicalInterface": "updi"
      },

      "debugServer": {
        "name": "avr-gdb-rsp",
        "ipAddress": "127.0.0.1",
        "port": "1442"
      }
    }
  },

  "insight": {
    "enabled": true
  }
}
```

And then run `bloom`, the output looks like:

<img src="https://user-images.githubusercontent.com/1625340/171673137-2885e208-6b90-4acf-ba07-f7a09e328fff.png" width="50%"/>

And the insight window also start but will not show useful information until avr-gdb connected.

<img src="https://user-images.githubusercontent.com/1625340/171673776-e7b1bec6-b5e9-48bf-b133-ff9a45dc0a2d.png" width="50%"/>

Then you can launch a avr-gdb session with:

```
$ avr-gdb
(gdb) target remote :1442
```

# 6. how to make a debugwire FUSE rescue board

According to https://www.electronics-lab.com/recover-bricked-attiny-using-arduino-as-high-voltage-programmer/, you can make a rescue board if you brick your attiny device. actually, it can be used to modify FUSE bit of attiny devices as you like.
  
The rescue board use arduino uno/nano as conroller and the circuit diagram here:
  
![rescue-fuse](https://user-images.githubusercontent.com/1625340/171389363-0e8bdd47-315d-4463-9b00-6b681ff43fa9.png)

I suggest to make a board permanently and I make one looks like (with a 5v to 12v convertor):

<img src="https://user-images.githubusercontent.com/1625340/171394141-1c7098a8-f603-461a-9cb0-75018a45c980.png"  width="70%"/>

'5V (2)' and 'GND (2)' are for seperated power supply to 5v to 12v convertor, the current of 12v output should less than 500ma, usually it's safe to use a 5V-1A charger.
  
Then upload below sketch to uno or nano:

```
 // AVR High-voltage Serial Fuse Reprogrammer
 // Adapted from code and design by Paul Willoughby 03/20/2010
 // http://www.rickety.us/2010/03/arduino-avr-high-voltage-serial-programmer/
 // Fuse Calc:
 // http://www.engbedded.com/fusecalc/

 #define RST 13 // Output to level shifter for !RESET from transistor
 #define SCI 12 // Target Clock Input
 #define SDO 11 // Target Data Output
 #define SII 10 // Target Instruction Input
 #define SDI 9 // Target Data Input
 #define VCC 8 // Target VCC

 #define HFUSE 0x747C
 #define LFUSE 0x646C
 #define EFUSE 0x666E

 // Define ATTiny series signatures
 #define ATTINY13 0x9007 // L: 0x6A, H: 0xFF 8 pin
 #define ATTINY24 0x910B // L: 0x62, H: 0xDF, E: 0xFF 14 pin
 #define ATTINY25 0x9108 // L: 0x62, H: 0xDF, E: 0xFF 8 pin
 #define ATTINY44 0x9207 // L: 0x62, H: 0xDF, E: 0xFFF 14 pin
 #define ATTINY45 0x9206 // L: 0x62, H: 0xDF, E: 0xFF 8 pin
 #define ATTINY84 0x930C // L: 0x62, H: 0xDF, E: 0xFFF 14 pin
 #define ATTINY85 0x930B // L: 0x62, H: 0xDF, E: 0xFF 8 pin

 void setup() {
 pinMode(VCC, OUTPUT);
 pinMode(RST, OUTPUT);
 pinMode(SDI, OUTPUT);
 pinMode(SII, OUTPUT);
 pinMode(SCI, OUTPUT);
 pinMode(SDO, OUTPUT); // Configured as input when in programming mode
 digitalWrite(RST, HIGH); // Level shifter is inverting, this shuts off 12V
 Serial.begin(19200);
 Serial.println("Code is modified by Rik. Visit riktronics.wordpress.com and electronics-lab.com for more projects");
 Serial.println("-------------------------------------------------------------------------------------------------");
 Serial.println("Enter any character to start process..");}


 void loop() {
 if (Serial.available() > 0) {
 Serial.read();
 pinMode(SDO, OUTPUT); // Set SDO to output
 digitalWrite(SDI, LOW);
 digitalWrite(SII, LOW);
 digitalWrite(SDO, LOW);
 digitalWrite(RST, HIGH); // 12v Off
 digitalWrite(VCC, HIGH); // Vcc On
 delayMicroseconds(20);
 digitalWrite(RST, LOW); // 12v On
 delayMicroseconds(10);
 pinMode(SDO, INPUT); // Set SDO to input
 delayMicroseconds(300);
 unsigned int sig = readSignature();
 Serial.println("Reading signature from connected ATtiny......");
 Serial.println("Reading complete..");
 Serial.print("Signature is: ");
 Serial.println(sig, HEX);
 readFuses();
 if (sig == ATTINY13) {

 Serial.println("The ATtiny is detected as ATtiny13/ATtiny13A..");
 Serial.print("LFUSE: ");
 writeFuse(LFUSE, 0x6A);
 Serial.print("HFUSE: ");
 writeFuse(HFUSE, 0xFF);
 Serial.println("");
 } else if (sig == ATTINY24 || sig == ATTINY44 || sig == ATTINY84 ||
 sig == ATTINY25 || sig == ATTINY45 || sig == ATTINY85) {

 Serial.println("The ATtiny is detected as "); 
 if(sig == ATTINY24) Serial.println("ATTINY24..");
 else if(sig == ATTINY44) Serial.println("ATTINY44..");
 else if(sig == ATTINY84) Serial.println("ATTINY84..");
 else if(sig == ATTINY25) Serial.println("ATTINY25..");
 else if(sig == ATTINY45) Serial.println("ATTINY45..");
 else if(sig == ATTINY85) Serial.println("ATTINY85..");
 
 writeFuse(LFUSE, 0x62);
 writeFuse(HFUSE, 0xDF);
 writeFuse(EFUSE, 0xFF);
 }

 Serial.println("Fuses will be read again to check if it's changed successfully..");
 readFuses();
 digitalWrite(SCI, LOW);
 digitalWrite(VCC, LOW); // Vcc Off
 digitalWrite(RST, HIGH); // 12v Off

 Serial.println("");
 Serial.println("");
 Serial.println("");
 Serial.println("");
 }
 }

 byte shiftOut (byte val1, byte val2) {
 int inBits = 0;
 //Wait until SDO goes high
 while (!digitalRead(SDO))
 ;
 unsigned int dout = (unsigned int) val1 << 2;
 unsigned int iout = (unsigned int) val2 << 2;
 for (int ii = 10; ii >= 0; ii--) {
 digitalWrite(SDI, !!(dout & (1 << ii)));
 digitalWrite(SII, !!(iout & (1 << ii)));
 inBits <<= 1; inBits |= digitalRead(SDO);
 digitalWrite(SCI, HIGH);
 digitalWrite(SCI, LOW);
 } 
 return inBits >> 2;
 }

 void writeFuse (unsigned int fuse, byte val) {
 
 Serial.println("Writing correct fuse settings to ATtiny.......");
 
 shiftOut(0x40, 0x4C);
 shiftOut( val, 0x2C);
 shiftOut(0x00, (byte) (fuse >> 8));
 shiftOut(0x00, (byte) fuse);

 Serial.println("Writing complete..");
 }

 void readFuses () {

 Serial.println("Reading fuse settings from connected ATtiny.......");
 
 byte val;
 shiftOut(0x04, 0x4C); // LFuse
 shiftOut(0x00, 0x68);
 val = shiftOut(0x00, 0x6C);
 Serial.print("LFuse: ");
 Serial.print(val, HEX);
 shiftOut(0x04, 0x4C); // HFuse
 shiftOut(0x00, 0x7A);
 val = shiftOut(0x00, 0x7E);
 Serial.print(", HFuse: ");
 Serial.print(val, HEX);
 shiftOut(0x04, 0x4C); // EFuse
 shiftOut(0x00, 0x6A);
 val = shiftOut(0x00, 0x6E);
 Serial.print(", EFuse: ");
 Serial.println(val, HEX);
 Serial.println("Reading complete..");
 }

 unsigned int readSignature () {
 unsigned int sig = 0;
 byte val;
 for (int ii = 1; ii < 3; ii++) {
 shiftOut(0x08, 0x4C);
 shiftOut( ii, 0x0C);
 shiftOut(0x00, 0x68);
 val = shiftOut(0x00, 0x6C);
 sig = (sig << 8) + val;
 }
 return sig;
 }

```

After upload the sketch, open 'serial monitor' of arduino IDE and set baudrate to 19200/no line ending, or use `minicom -b 19200 -D /dev/ttyUSB0`, wire up as circuit diagram indicated.
                
When you get the msg:
```
Code is modified by Rik. Visit riktronics.wordpress.com and electronics-lab.com for more projects
-------------------------------------------------------------------------------------------------
Enter any character to start process..
```
                
Enter any character and the output will look like:

```
Reading signature from connected ATtiny......
Reading complete..
Signature is: 9007
Reading fuse settings from connected ATtiny.......
LFuse: 6A, HFuse: F7, EFuse: FF
Reading complete..
The ATtiny is detected as ATtiny13/ATtiny13A..
LFUSE: Writing correct fuse settings to ATtiny.......
Writing complete..
HFUSE: Writing correct fuse settings to ATtiny.......
Writing complete..

Fuses will be read again to check if it's changed successfully..
Reading fuse settings from connected ATtiny.......
LFuse: 6A, HFuse: FF, EFuse: FF
Reading complete..

```                

# 7. how to make your own HV UPDI programer
I did not try it since already have a HV UPDI programer, if you want to give a try, please refer to:

https://www.electronics-lab.com/diy-arduino-nano-hv-updi-programmer/


I thought the above solution of HV debugwire rescue adapter can be used as a HV UPDI rescue adapter with code modification for UPDI protocol, but not try up to now.

# 8. how to update USBASP firmware

If **"avrdude : warning : Can not Set sck period . usbasp please check for firmware update"**, it means your usbasp adapter's firmware is outdated. it should still works well, it is NOT neccesary to update the firmware. but if you insist to do that, please follow this guide:

**8.1. Download the USBASP firmware**

Download the firmware from https://www.fischl.de/usbasp/, the lastest version is "https://www.fischl.de/usbasp/usbasp.2011-05-28.tar.gz". there are 3 hex files in 'bin/firmware' dir, choose the one according to your usbasp adapter. since most of them is atmega8, that's to say, you should use "usbasp.atmega8.2011-05-28.hex" for atmega8.


**8.2. Prepare another workable ISP progammer**

To flash the new firmware onto the target atmega8, we need another ISP programmer. either usbasp adapter or usbtinyisp adapter or arduino board is OK.

If you use arduino as ISP programmer, you need:

* Download arduino IDE.
* Go to "File" > "Examples" > "ArduinoISP"
* Connect an Arduino board to your PC
* Select the port and board type and "Click" on "Upload"

It will turn your arduino board to ISP programmer.

**8.3. Gain control of the chip's RESET pin**

The 'RESET' pin of the ISP header on usbasp adapter need to be updated is not connect to the atmega8 chip's RESET pin, thus you can not program the adapter until gain control of the chip's RESET pin.

Usually, the usbasp programmer need to be updated should have a Jumper "JP2" on board as:

<img src="https://user-images.githubusercontent.com/1625340/170300994-dec83783-461e-478b-b673-723484030026.png" width=50%/>

**This jumper need to be closed to connect the "RESET" pin of ISP header to atmega8 chip's RESET pin (pin 29) to enable self-programming.**

There are a lot of cheap and old USBASP programmer do not have such a Jumper JP2 on board, such as:

<img src="https://user-images.githubusercontent.com/1625340/170302304-7ddabb6f-31f7-43db-9a34-5ad9db00800d.jpg" width=50%/>

If you have such a programmer need to be updated, you have to solder a wire to the chip's RESET pin or use test hook to connect a wire to it. **this wire will be used as RESET pin to program the target usbasp adapter later", note the RESET pin of atmega8 is pin 29:

![atmega8-pin](https://user-images.githubusercontent.com/1625340/170304456-496d3b60-cc4b-4109-b1e4-f1ad015040f0.png)

**8.4. Wire up ISP programmer and target usbasp adapter**

If you have another usbasp or usbtinyisp programmer, just wire them up as usual. If there is no JP2 jumper on board, you need connect the programmer's RESET pin directly to target atmega8 chip's RESET pin.

If you use arduino as ISP programmer, you need connect the Arduino to target usbasp adatper with the following connections:

* Arduino 5v to target VCC
* Arduino GND to target GND
* Arduino D13 to target SCK
* Arduino D12 to target MISO
* Arduino D11 to target MOSI
* Arduino D10 to target RESET(if has JP2 jumper) or to the wire from target chip's PIN 29.

**8.5. Detect the target adapter**

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

**8.6. Finally, update the firmwire**
With a working connection to the target ATmega8,

```
avrdude -p m8 -c <your programmer> -P /dev/ttyUSB0 -b 19200 -U flash:w:usbasp.atmega8.2011-05-28.hex:i
```

to use arduino as ISP programmer, you still need to use `-c avrisp -C <Where your Arduino IDE>/hardware/tools/avr/etc/avrdude.conf`.



  
 
