# Opensource toolchain for AVR
to be written.
We are talking AVR, not arduino.

# Hardware prerequist

* AVR development board, such as atmega128, atmega328, attinyXX.
* AVR ISP programmer (Such as usbasp or usbtinyisp, or an arduino board).
* AVR JTAG ICE MK1/MK2/3, for JTAG/debugwire/UPDI etc. debugging, or a self-made debugwire adapter, depending on which debug protocol your MCU support.

**NOTE:**

- There are [various programming/debugging prototols](https://www.kanda.com/blog/microcontrollers/avr-microcontrollers/avr-microcontroller-programming-interfaces-isp-jtag-tpi-pdi-updi/) for different AVR models, such as JTAG, debugwire, UPDI, etc. The latest official 'AVR JTAG ICE 3' is always the best choice, but a little bit expensive. ICE3 can support all protocols for different models, and can be supported by avarice. Earlier version of AVR JTAG ICE may lack some protocol support, for example, if you want to disable debugwire FUSE, you have to use AVR JTAG ICE MK2 and above.

- Not all arduino board but most of them is AVR board with atmega328, usually with a USB bootloader to make programming easy and may lack of debugging support depending on the circuit design.


# Toolchain overview
* Compiler: avr-gcc
* SDK: avr-libc
* Debugger: avarice/dwdebug/avr-gdb
* Programer: avrdude


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

**3. Gain control of the chip's RESET pin **

The 'RESET' pin of the ISP header on usbasp adapter need to be updated is not connect to the atmega8 chip's RESET pin, thus you can not program the adapter until gain control of the chip's RESET pin.

Usually, the usbasp programmer need to be updated should have a Jumper "JP2" on board as:

![usbjumper](https://user-images.githubusercontent.com/1625340/170300994-dec83783-461e-478b-b673-723484030026.png)

**This jumper need to be closed to connect the "RESET" pin of ISP header to atmega8 chip's RESET pin (pin 29) to enable self-programming.**

There are a lot of cheap and old USBASP programmer do not have such a Jumper JP2 on board, such as:

![usbasp-nojp2](https://user-images.githubusercontent.com/1625340/170302304-7ddabb6f-31f7-43db-9a34-5ad9db00800d.jpg)

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
avrdude -p m8 -c <your programmer" -P /dev/ttyUSB0 -b 19200 -U flash:w:usbasp.atmega8.2011-05-28.hex:i
```

to use arduino as ISP programmer, you still need to use `-c avrisp -C <Where your Arduino IDE>/hardware/tools/avr/etc/avrdude.conf`.




  
 
