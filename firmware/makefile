## Project specific:
MCU   = atmega328pb
F_CPU = 8000000UL  
BAUD  = 9600UL

##  Paths for source files, header files and libraries.
INC_DIR		= ./include
LIB_DIR		= ./lib
SRC_DIR		= ./src

## Programmer hardware - list options with: $ avrdude -c ?
PROGRAMMER_TYPE = usbtiny
## extra arguments to avrdude: baud rate, chip type, -F flag, etc.
PROGRAMMER_ARGS = 	

## Executables:
CC = avr-g++
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AVRSIZE = avr-size
AVRDUDE = avrdude



## The name of your project (without the .c or .cpp)
TARGET = grinder_timer
## Or name it automatically after the enclosing directory
#TARGET = $(lastword $(subst /, ,$(CURDIR)))

## Define objects and headers.  The following will work for .c/.h or .cpp/.hpp or a combination.
SOURCES=$(wildcard *.cpp $(SRC_DIR)/*.cpp *.c $(SRC_DIR)/*.c)

CPP_OBJECTS=$(SOURCES:.cpp=.o)
OBJECTS=$(CPP_OBJECTS:.c=.o)
CPP_HEADERS=$(SOURCES:.cpp=.hpp)
HEADERS=$(CPP_HEADERS:.c=.h)

## Compilation options, type man avr-gcc if you're curious.
CPPFLAGS = -DF_CPU=$(F_CPU) -DBAUD=$(BAUD) -I. -I$(INC_DIR) -I$(LIB_DIR)
CFLAGS = -Os -g -std=gnu++17 -Wall
## Use short (8-bit) data types 
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums 
## Splits up object files per function
CFLAGS += -ffunction-sections -fdata-sections 
LDFLAGS = -Wl,-Map,$(TARGET).map 
## Optional, but often ends up with smaller code
LDFLAGS += -Wl,--gc-sections 
## Relax shrinks code even more, but makes disassembly messy
## LDFLAGS += -Wl,--relax
## LDFLAGS += -Wl,-u,vfprintf -lprintf_flt -lm  ## for floating-point printf
## LDFLAGS += -Wl,-u,vfprintf -lprintf_min      ## for smaller printf
TARGET_ARCH = -mmcu=$(MCU)


## We want to compile source files into .elf files then convert .elf files into .hex.

##  To make .o files from .c and .cpp files 
%.o: %.cpp
	 $(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c -o $@ $<;

## Make .elf from .o
$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) $(TARGET_ARCH) $^ $(LDLIBS) -o $@

## Make .hex from .elf
%.hex: %.elf
	 $(OBJCOPY) -j .text -j .data -O ihex $< $@

%.eeprom: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@ 

%.lst: %.elf
	$(OBJDUMP) -S $< > $@

## These targets don't have files named after them
.PHONY: all disassemble disasm eeprom size clean squeaky_clean flash fuses

all: $(TARGET).hex 

debug:
	@echo
	@echo "Source files:"   $(SOURCES)
	@echo "Objects:"   $(OBJECTS)
	@echo "Header files:"   $(HEADERS)
	@echo "MCU, F_CPU, BAUD:"  $(MCU), $(F_CPU), $(BAUD)
	@echo	

# Optionally create listing file from .elf
# This creates approximate assembly-language equivalent of your code.
# Useful for debugging time-sensitive bits, 
# or making sure the compiler does what you want.
disassemble: $(TARGET).lst

disasm: disassemble

# Optionally show how big the resulting program is 
size:  $(TARGET).elf
	$(AVRSIZE) -C --mcu=$(MCU) $(TARGET).elf

clean:
	rm -f *.elf *.hex *.obj *.o *.d *.eep *.lst *.lss *.sym *.map *~ *.eeprom \
	$(SRC_DIR)/*.elf $(SRC_DIR)/*.hex $(SRC_DIR)/*.obj $(SRC_DIR)/*.o $(SRC_DIR)/*.d \
	$(SRC_DIR)/*.eep $(SRC_DIR)/*.lst $(SRC_DIR)/*.lss $(SRC_DIR)/*.sym $(SRC_DIR)/*.map \
	$(SRC_DIR)/*~ $(SRC_DIR)/*.eeprom

## Upload the hex to the mcu.
flash: $(TARGET).hex 
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -U flash:w:$<

## An alias
program: flash

flash_eeprom: $(TARGET).eeprom
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -U eeprom:w:$<

avrdude_terminal:
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -nt

##########------------------------------------------------------##########
##########       Fuse settings and suitable defaults            ##########
##########------------------------------------------------------##########

## Mega 48, 88, 168, 328 default values
LFUSE = 0x62
HFUSE = 0xdf
EFUSE = 0x00

## Generic 
FUSE_STRING = -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m 

fuses: 
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) \
	           $(PROGRAMMER_ARGS) $(FUSE_STRING)
show_fuses:
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -nv	

## Called with no extra definitions, sets to defaults
set_default_fuses:  FUSE_STRING = -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m 
set_default_fuses:  fuses

## Set the fuse byte for full-speed mode
## Note: can also be set in firmware for modern chips
set_fast_fuse: LFUSE = 0xE2
set_fast_fuse: FUSE_STRING = -U lfuse:w:$(LFUSE):m 
set_fast_fuse: fuses

## Set the EESAVE fuse byte to preserve EEPROM across flashes
set_eeprom_save_fuse: HFUSE = 0xD7
set_eeprom_save_fuse: FUSE_STRING = -U hfuse:w:$(HFUSE):m
set_eeprom_save_fuse: fuses

## Clear the EESAVE fuse byte
clear_eeprom_save_fuse: FUSE_STRING = -U hfuse:w:$(HFUSE):m
clear_eeprom_save_fuse: fuses