# Makefile for linux with arduino-cli

TARGET=uno
PORT=/dev/ttyUSB0

all:
	@echo "'make upload TARGET=uno' for Arduino uno"
	@echo "'make upload TARGET=nano' for Arduino nano"

compile:
	arduino-cli core install arduino:avr
	arduino-cli compile -b arduino:avr:$(TARGET) debugwire-hv-reprogrammer

upload: compile
	arduino-cli upload -p $(PORT) --fqbn arduino:avr:$(TARGET) debugwire-hv-reprogrammer
