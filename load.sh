avr-gcc -mmcu=atmega644p -s -DF_CPU=12000000 -Os main.c -o main.elf
avr-objcopy -O ihex main.elf main.hex
avrdude -c usbasp -p m644p -U flash:w:main.hex -q -q