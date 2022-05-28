# define your device
# refer to https://gcc.gnu.org/onlinedocs/gcc/AVR-Options.html
MCU_TYPE = attiny13
# MCU_TYPE = attiny85
# MCU_TYPE = attiny88
# MCU_TYPE = atmega128
# MCU_TYPE = atmega328p

AVRGCC = avr-gcc
AVRDUDE = avrdude

DEBUGFLAGS=-g
CFLAGS = $(DEBUGFLAGS) -Os 
MFLAGS = -mmcu=$(MCU_TYPE)
OBJCOPY_FLAGS = -R .eeprom

ifneq (,$(findstring attiny,$(MCU_TYPE)))
OBJCOPY_FLAGS = 
endif
 
SRCS = main.c

TARGET = main.hex

all: $(TARGET)

OBJECTS = $(notdir $(SRCS:.c=.o))
vpath %.c $(sort $(dir $(SRCS)))

%.o: %.c Makefile
	$(AVRGCC) -c $(CFLAGS) $(MFLAGS) $< -o $@

main.elf: $(OBJECTS)
	$(AVRGCC) $(MFLAGS) $(LDFLAGS) $^ -o $@

main.hex: main.elf
	avr-objcopy -O ihex $(OBJCOPY_FLAGS) $< $@

flash:
ifeq ($(MCU_TYPE), attiny13)
	avrdude -c usbasp -P usb -p t13 -B4 -U flash:w:$(TARGET)
else ifeq ($(MCU_TYPE), attiny85)
	avrdude -c usbasp -P usb -p t85 -B4 -U flash:w:$(TARGET)
else ifeq ($(MCU_TYPE), attiny88)
	avrdude -c usbasp -P usb -p t88 -B4 -U flash:w:$(TARGET)
else ifeq ($(MCU_TYPE), atmega128)
	avrdude -c usbasp -P usb -p m128 -U flash:w:$(TARGET)
else ifeq ($(MCU_TYPE), atmega328p)
	avrdude -c arduino -p m328p -P /dev/ttyUSB0 -b 115200 -U flash:w:$(TARGET)
endif

clean:
	rm -f $(OBJECTS) main.elf main.hex
 
#clean:
#	rm -f $(OBJECTS) *.asm *.lk *.lst *.map *.rel *.sym *.rst
#
#
#buildm128:
#	avr-gcc -g -Os -mmcu=atmega128 -c -o blink.o blink.c
#	avr-gcc -mmcu=atmega128 blink.o -o blink.elf
#	avr-objcopy -O ihex -R .eeprom blink.elf blink.hex
#
#buildm328:
#	avr-gcc -g -Os -mmcu=atmega328p -c -o blink.o blink.c
#	avr-gcc -mmcu=atmega328p blink.o -o blink.elf
#	avr-objcopy -O ihex -R .eeprom blink.elf blink.hex
#
#buildt13:
#	avr-gcc -g -Os -mmcu=attiny13 -c -o blink.o blink.c
#	avr-gcc -mmcu=attiny13 blink.o -o blink.elf
#	avr-objcopy -O ihex -R .eeprom blink.elf blink.hex
#
#buildt85:
#	avr-gcc -g -Os -mmcu=attiny13 -c -o blink.o blink.c
#	avr-gcc -mmcu=attiny13 blink.o -o blink.elf
#	avr-objcopy -O ihex -R .eeprom blink.elf blink.hex
#
#progm328: buildm328 
#	avrdude -F -V -c arduino -p m328p -P /dev/ttyUSB0 -b 115200 -U flash:w:blink.hex
#
#progm128: buildm128
#	avrdude -c usbasp -P usb -p m128 -U flash:w:blink.hex
#
#prog_with_arduino_as_isp_to_128: buildm128
#	avrdude -c avrisp -P /dev/ttyUSB0 -b 19200 -p m128 -U flash:w:blink.hex
#
#prog_with_arduino_as_isp_to_328: buildm328
#	avrdude -c avrisp -P /dev/ttyUSB0 -b 19200 -p m328 -U flash:w:blink.hex
#
#debug:
#	sudo avarice --part atmega128 --jtag /dev/ttyUSB0 :3333	
#
#clean:
#	rm -f blink.o blink blink.hex
