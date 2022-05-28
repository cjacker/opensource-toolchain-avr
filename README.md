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
  + or USB to TTL adapter with self-made adapter for debugwire with [dwdebug](https://github.com/dcwbrown/dwire-debug).
 
**NOTE:**

- You'd better have an ISP programmer to program or change the FUSE bits.

- Changing FUSE bits is a little bit dangerous for beginners, it may 'brick' a device, for example, Any ISP programmer is able to enable debugwire FUSE bit, but if you want to disable it, you have to use AVR DRAGON/ICE MKII and above or you have a High-Voltage programmer. for UPDI, you have to use HV UPDI programmer.

- Not all arduino but uno/nano and most other models are AVR board with atmega mcu and suite for this tutorial. Arduino uno/nano have a USB bootloader to make programming easy (no additional hardwire required to program)  and can be turnned to a ISP programmer. that's to say, if you already have an arduino board, it's not necessary to buy ISP programmer anymore.

- Arduino uno/nano are lack of debugging support due to circuit design related to RESET pin(Debugwire pin for 328), you need modify the hardware to enable it (and do not do this).

- If the chip support debugwire debugging and you do not need to disable DWEN fuse bit later, you can use CH340 USB to TTL adapter with dwdebug to debug it, it's not necesary to buy a AVR ICE MKII and above.

- For other satuations, buy a AVR ICE 3 or ATMEL-ICE. There are [various programming/debugging prototols](https://www.kanda.com/blog/microcontrollers/avr-microcontrollers/avr-microcontroller-programming-interfaces-isp-jtag-tpi-pdi-updi/) for different AVR models, such as ISP/JTAG/debugwire/UPDI, etc. Earlier version of AVR JTAG ICE may lack some protocol support，The latest official AVR ICE 3 or ATMEL ICE is always the best choice except price.

- Pickit4 also support all avr debug protocols include hv updi support after atmel was acquired by microchip, but lack of good opensource support except pymcuprog for avr mode. 

# Toolchain overview

* Compiler: avr-gcc
* SDK: avr-libc
* Programer: avrdude/updiprog
* Debugger: avarice/dwdebug, avr-gdb
* [Optional] Simulator: simavr.


# Compiler and SDK
AVR has very good support from opensource community include avr-binutils(binary utils), avr-gcc(compiler), avr-libc(c libraries), avrdude(the programmer) and avrrice(the debug bridge), it's not necessary to build the toolchain yourself, since almost every linux distribution shipped this packages, just install it via pkg management tools of your distribution.




# how to update usbasp firmware

If **"avrdude : warning : Can not Set sck period . usbasp please check for firmware update"**, it means your usbasp adapter's firmware is outdated. it should still works well, but If you insist to update the usbasp firmware, follow this guide:

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




  
 
