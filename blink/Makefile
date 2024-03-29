# define your device
# refer to https://gcc.gnu.org/onlinedocs/gcc/AVR-Options.html

MCU_TYPE = attiny13
# MCU_TYPE = attiny85
# MCU_TYPE = attiny88
# MCU_TYPE = attiny816

# MCU_TYPE = atmega128

# Arduino uno/nano
# MCU_TYPE = atmega328p

# Arduino nano every
# MCU_TYPE = atmega4809

# Thinary nano 4808
# MCU_TYPE = atmega4808

AVRGCC = avr-gcc
AVRDUDE = avrdude
PROGRAMMER = usbasp
# PROGRAMMER = usbtiny
# PROGRAMMER = atmelice_isp
# PROGRAMMER = atmelice_dw
# PROGRAMMER = atmelice_jtag
# PROGRAMMER = atmelice_updi

DEBUGFLAGS=-g
CFLAGS = $(DEBUGFLAGS) -Os 
MFLAGS = -mmcu=$(MCU_TYPE)
OBJCOPY_FLAGS =
# Some USBASP programmer may need -B settings 
AVRDUDE_FLAGS = -B4

ifneq (,$(findstring atmelice,$(PROGRAMMER)))
AVRDUDE_FLAGS =
endif

ifneq (,$(findstring attiny,$(MCU_TYPE)))
OBJCOPY_FLAGS = 
endif

# Pur your sources here
SRCS = main.c

# For Attiny 816
ifneq (,$(findstring attiny816,$(MCU_TYPE)))
SRCS = main_for_t816_m4808_m4809.c
PROGRAMMER=atmelice_updi
AVRDUDE_FLAGS=
endif

# For Arduino nano every and Thinary nano 4808
ifneq (,$(findstring atmega480,$(MCU_TYPE)))
SRCS = main_for_t816_m4808_4809.c
PROGRAMMER=atmelice_updi
AVRDUDE_FLAGS=
endif

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
	avrdude -c $(PROGRAMMER) -p t13 $(AVRDUDE_FLAGS) -U flash:w:$(TARGET)
else ifeq ($(MCU_TYPE), attiny85)
	avrdude -c $(PROGRAMMER) -p t85 $(AVRDUDE_FLAGS) -U flash:w:$(TARGET)
else ifeq ($(MCU_TYPE), attiny88)
	avrdude -c $(PROGRAMMER) -p t88 $(AVRDUDE_FLAGS) -U flash:w:$(TARGET)
else ifeq ($(MCU_TYPE), attiny816)
	pyupdi -d attiny816 -b 115200 -c /dev/ttyUSB0 -e -f main.hex
# pymcuprog -t uart -d attiny816 -u /dev/ttyUSB0 write -f main.hex --erase --verify
# avrdude -c $(PROGRAMMER) -p t816 $(AVRDUDE_FLAGS) -U flash:w:$(TARGET)
else ifeq ($(MCU_TYPE), atmega128)
	avrdude -c $(PROGRAMMER) -p m128 $(AVRDUDE_FLAGS) -U flash:w:$(TARGET)
else ifeq ($(MCU_TYPE), atmega328p)
# use USB cabel to program arduino directly.
	avrdude -c arduino -p m328p -P /dev/ttyUSB0 -b 115200 -U flash:w:$(TARGET)
else ifeq ($(MCU_TYPE), atmega4808)
	/usr/bin/avrdude -c $(PROGRAMMER) -p m4808 $(AVRDUDE_FLAGS) -U flash:w:$(TARGET)
# pymcuprog with CH340 and self-made 4.7k resistor adapter:
# pymcuprog -t uart -d atmega4808 -u /dev/ttyUSB0 write -f main.hex --erase --verify
# pyupdi with CH340 and self-made 4.7k resistor adapter:
# pyupdi -d atmega4808 -e -b 115200 -f main.hex -c /dev/ttyUSB0
else ifeq ($(MCU_TYPE), atmega4809)
	/usr/bin/avrdude -c $(PROGRAMMER) -p m4809 $(AVRDUDE_FLAGS) -U flash:w:$(TARGET)
endif

flash_debugwire:
	dwdebug l ./main.elf,qr

clean:
	rm -f $(OBJECTS) *.o main.elf main.hex
