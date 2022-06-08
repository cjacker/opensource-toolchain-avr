all: upload
compile:
	arduino-cli core install arduino:avr
	arduino-cli compile -b arduino:avr:nano debugwire-hv-reprogrammer
upload: compile
	arduino-cli upload -p /dev/ttyUSB0 --fqbn arduino:avr:nano debugwire-hv-reprogrammer	
