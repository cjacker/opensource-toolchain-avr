# Opensource toolchain for AVR
to be written.
We are talking AVR, not arduino.

# Hardware prerequist

* AVR development board, such as atmega128, atmega328, attinyXX.
* AVR ISP programmer (Such as usbasp or usbtinyisp)
* AVR JTAG ICE MK1/MK2/3, for JTAG/debugwire/UPDI etc. debugging, or a self-made debugwire adapter, depending on which debug protocol your MCU support.

**NOTE:**

- There are [various programming/debugging prototols](https://www.kanda.com/blog/microcontrollers/avr-microcontrollers/avr-microcontroller-programming-interfaces-isp-jtag-tpi-pdi-updi/) for different AVR models, such as JTAG, debugwire, UPDI, etc. The latest official 'AVR JTAG ICE 3' is always the best choice, but a little bit expensive. ICE3 can support all protocols for different models, and can be supported by avarice. Earlier version of AVR JTAG ICE may lack some protocol support, for example, if you want to disable debugwire FUSE, you have to use AVR JTAG ICE MK2 and above.

- Not all arduino board but most of them is AVR board with atmega328, usually with a USB bootloader to make programming easy and may lack of debugging support depending on the circuit design.


# Toolchain overview
* Compiler: avr-gcc
* SDK: avr-libc
* Debugger: avarice/dwdebug/avr-gdb
* Programer: avrdude

