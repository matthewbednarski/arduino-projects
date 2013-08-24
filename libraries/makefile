
TARGET	:= $(notdir $(CURDIR))
INSTALL_DIR	:= /usr/avr/include/arduino
AVR_PART_NO	:= ATMEGA328P
MCU	:= atmega328p
F_CPU	:= 16000000UL
PINS_VARIANT	:= standard

############################################################################
# Below here nothing should be changed...

OUT_DIR = obj
ARDUINO = $(INSTALL_DIR)
AVR_TOOLS_PATH = /usr/bin
AVRDUDE_PATH = /usr/bin
SRC	:= $(shell find . -type f -name '*.c')
CXXSRC	:= $(shell find . -type f -name '*.cpp')
#SRC	:= $(wildcard *.c)
#CXXSRC	:= $(wildcard *.cpp)
OBJ	:= $(SRC:%.c=%.o)
OBJ	+= $(CXXSRC:%.cpp=%.o)
#OBJ	:= $(patsubst %.c,$(OUT_DIR)/%.o,$(SRC))
#OBJ	+= $(patsubst %.cpp,$(OUT_DIR)/%.o, $(CXXSRC))


# Name of this Makefile (used for "make depend").
MAKEFILE = Makefile

# Debugging format.
# Native formats for AVR-GCC's -g are stabs [default], or dwarf-2.
# AVR (extended) COFF requires stabs, plus an avr-objcopy run.
DEBUG = stabs

OPT = s

# Place -D or -U options here
CDEFS = -DF_CPU=$(F_CPU) -DARDUINO=101
CXXDEFS = -DF_CPU=$(F_CPU) -DARDUINO=101

# Place -I options here
CINCS = -I$(ARDUINO) -I$(ARDUINO)/variants/$(PINS_VARIANT) -I/usr/lib/arduino/libraries 
CXXINCS = -I$(ARDUINO) -I$(ARDUINO)/variants/$(PINS_VARIANT) -I/usr/lib/arduino/libraries 

# Compiler flag to set the C Standard level.
# c89   - "ANSI" C
# gnu89 - c89 plus GCC extensions
# c99   - ISO C99 standard (not yet fully implemented)
# gnu99 - c99 plus GCC extensions
CSTANDARD = -std=gnu99
CDEBUG = -g$(DEBUG)
CWARN = -Wall -Wstrict-prototypes
CTUNING = -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -ffunction-sections -fdata-sections
#CEXTRA = -Wa,-adhlns=$(<:.c=.lst)
CEXTRA = --pedantic 
CXXWARN = -Wall -Wreturn-type
CXXTUNING = -fno-exceptions
CXXEXTRA = 
CFLAGS = $(CDEBUG) $(CDEFS) $(CINCS) -O$(OPT) $(CWARN) $(CSTANDARD) $(CEXTRA) $(CTUNING)
CXXFLAGS = $(CDEBUG) $(CDEFS) $(CINCS) -O$(OPT) $(CXXWARN) $(CXXEXTRA) $(CXXTUNING)
#ASFLAGS = -Wa,-adhlns=$(<:.S=.lst),-gstabs
LDFLAGS = -Wl,-Map,$(OUT_DIR)/$(TARGET).map,--cref -Wl,--gc-sections -L/home/ekr/sketchbook/lib
# Programming support using avrdude. Settings and variables.
AVRDUDE_PORT = $(PORT)
AVRDUDE_WRITE_FLASH = -Uflash:w:$(OUT_DIR)/$(TARGET).hex:a
#AVRDUDE_FLAGS = -V -F -C $(AVRDUDE_PATH)/avrdude.conf 
AVRDUDE_FLAGS = -pATMEGA328P  -P$(AVRDUDE_PORT) -c$(AVRDUDE_PROGRAMMER) 
#-b$(UPLOAD_RATE)

# Program settings
CC = avr-gcc
CXX = avr-g++
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AR  = avr-ar
SIZE = avr-size
NM = avr-nm
AVRDUDE = $(AVRDUDE_PATH)/avrdude
REMOVE = rm -f
MV = mv -f

# Define all object files.
# OBJ = $(SRC:.c=.o) $(CXXSRC:.cpp=.o) $(ASRC:.S=.o)

# Define all listing files.
LST = $(ASRC:.S=.lst) $(CXXSRC:.cpp=.lst) $(SRC:.c=.lst)

# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS = -mmcu=$(MCU) -I. $(CFLAGS)
ALL_CXXFLAGS = -mmcu=$(MCU) -I. $(CXXFLAGS)
ALL_ASFLAGS = -mmcu=$(MCU) -I. -x assembler-with-cpp $(ASFLAGS)
ALL_LDFLAGS =  $(LDFLAGS) -mmcu=$(MCU)

# Default target.
all: build


# Display size of file.
HEXSIZE = $(SIZE) --target=$(FORMAT) $(OUT_DIR)/$(TARGET).hex
ELFSIZE = $(SIZE)  $(OUT_DIR)/$(TARGET).elf
sizebefore:
	@if [ -f $(OUT_DIR)/$(TARGET).elf ]; then echo; echo $(MSG_SIZE_BEFORE); $(HEXSIZE); echo; fi

sizeafter:


# Link: create ELF output file from library.
#
build: $(OBJ)
	cp $^ ../../lib/lib$(^:.o=.so)


# Compile: create object files from C++ source files.
%.o: %.cpp
	$(CXX) -c $(ALL_CXXFLAGS) $^ -o $@

# Compile: create object files from C source files.
$(OUT_DIR)/%.o: %.c
	mkdir -p $(OUT_DIR)
	$(CC) -c $(ALL_CFLAGS) $^ -o $@




# Target: clean project.
clean:
	$(REMOVE) $(OUT_DIR)/*.hex $(OUT_DIR)/*eep $(OUT_DIR)/*cof $(OUT_DIR)/*elf \
	$(OUT_DIR)/*map $(OUT_DIR)/*sym $(OUT_DIR)/*lss $(OUT_DIR)/core.a \
	$(OUT_DIR)/*.o $(OUT_DIR)/*.so \
	$(OBJ) $(LST) $(SRC:.c=.s) $(SRC:.c=.d) $(CXXSRC:.cpp=.s) $(CXXSRC:.cpp=.d)

depend:
	if grep '^# DO NOT DELETE' $(MAKEFILE) >/dev/null; \
	then \
		sed -e '/^# DO NOT DELETE/,$$d' $(MAKEFILE) > \
			$(MAKEFILE).$$$$ && \
		$(MV) $(MAKEFILE).$$$$ $(MAKEFILE); \
	fi
	echo '# DO NOT DELETE THIS LINE -- make depend depends on it.' \
		>> $(MAKEFILE); \
	$(CC) -M -mmcu=$(MCU) $(CDEFS) $(CINCS) $(SRC) $(ASRC) >> $(MAKEFILE)

.PHONY:	all build elf hex eep lss sym program coff extcoff clean depend sizebefore sizeafter
