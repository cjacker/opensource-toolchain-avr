# Opensource toolchain for AVR
to be written.
We are talking AVR, not arduino.

# Hardware prerequist
* AVR development board, such as atmega128, atmega328, attinyXX.
* AVR ISP programmer (Such as usbasp or usbtinyisp)
* AVR JTAG ICE MK1/MK2/3, for JTAG/debugwire/UPDI debugging, or a self-made Debugwire adapter, depending on which debug protocol your MCU support.

**NOTE:**

- There are various programming/debugging prototols for different AVR models, such as JTAG, debugwire, UPDI. A latest official 'AVR JTAG ICE 3' is always the best choice, but a little bit expensive. ICE3 can support all protocols for different models, and can be supported by avarice very well.

- Most Arduino board is AVR board such as atmega328, but usually with a USB bootloader to make programming easy and may lack of debugging support.


# Toolchain overview
* Compiler: avr-gcc
* SDK: avr-libc
* Debugger: avarice/dwdebug/avr-gdb
* Programer: avrdude

