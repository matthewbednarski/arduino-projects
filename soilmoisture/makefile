
TARGET			:= $(notdir $(CURDIR))
INSTALL_DIR		:= /home/matthew/arduino/arduino
AVR_SRCS_ROOT		:= /home/matthew/arduino/arduino/hardware/arduino/cores/arduino
#AVR_SRCS_ROOT		:= /usr/lib/avr/include/arduino

ARDUINO_VERS		:=105

#ARDUINO_OBJ_DIR		:= obj/core
#LIB_OBJ_DIR		:= obj/lib
#OBJ_DIR			:= obj
#BIN_DIR			:= bin
ARDUINO_OBJ_DIR		:= obj.$(CFG)/core
LIB_OBJ_DIR		:= obj.$(CFG)/lib
OBJ_DIR			:= obj.$(CFG)
BIN_DIR			:= bin.$(CFG)


###########################################################################
# Below here nothing should be changed...

# ARDUINO_SKETCHBOOK	= $(HOME)/sketchbook
# AS			= $(ARDUINO_SKETCHBOOK)
USER_LIBS_PATHS		= $(addprefix ../libraries/, $(ARDUINO_LIBS))

ARDUINO_CPP_SRCS	= $(wildcard $(AVR_SRCS_ROOT)/*.cpp)
ARDUINO_AVR_CPP_SRCS	= $(foreach src, $(wildcard $(AVR_SRCS_ROOT)/avr-libc/*.cpp), $(src))

ARDUINO_C_SRCS		= $(wildcard $(AVR_SRCS_ROOT)/*.c)
ARDUINO_AVR_C_SRCS	= $(foreach src, $(wildcard $(AVR_SRCS_ROOT)/avr-libc/*.c), $(src))

LIB_CPP_SRCS		= $(foreach src_dir,$(USER_LIBS_PATHS),$(wildcard $(src_dir)/*.cpp))
LIB_C_SRCS		= $(foreach src_dir,$(USER_LIBS_PATHS),$(wildcard $(src_dir)/*.c))

LOCAL_ARD_SRCS		+= $(wildcard *.ino)
LOCAL_ARD_SRCS		+= $(wildcard *.pde)
LOCAL_CPP_SRCS		+= $(wildcard *.cpp)
LOCAL_C_SRCS		+= $(wildcard *.c)

ARDUINO_SRCS		= $(ARDUINO_CPP_SRCS) $(ARDUINO_C_SRCS)
ARDUINO_AVR_SRCS	= $(ARDUINO_AVR_CPP_SRCS) $(ARDUINO_AVR_C_SRCS)
LIB_SRCS		= $(LIB_CPP_SRCS) $(LIB_C_SRCS)
SRCS			= $(LOCAL_C_SRCS) $(LOCAL_CPP_SRCS) $(LOCAL_ARD_SRCS)



LIB_SRC_DIR		= src/lib

SRC_DIR			= .

ARDUINO_OBJS		= $(foreach src, $(ARDUINO_SRCS), $(addsuffix .o, $(ARDUINO_OBJ_DIR)/$(notdir $(src))))
ARDUINO_OBJS		+= $(foreach src, $(ARDUINO_AVR_SRCS), $(addsuffix .o, $(ARDUINO_OBJ_DIR)/avr-libc/$(notdir $(src))))
LIB_OBJS		= $(foreach src, $(LIB_SRCS), $(addsuffix .o, $(LIB_OBJ_DIR)/$(notdir $(src))))
LIB_OBJS_TMP		= $(addsuffix .o, $(LIB_SRCS))
OBJS			= $(foreach src, $(SRCS), $(addsuffix .o, $(OBJ_DIR)/$(notdir $(src))))


#-include $(OBJS:.o=.d)

########################################################################
# Determine ARDUINO_LIBS automatically
#
ifndef ARDUINO_LIBS
	ARDUINO_LIBS += $(PERM_LIBS)
    # automatically determine included libraries
    #ARDUINO_LIBS += $(filter $(notdir $(wildcard $(ARDUINO_DIR)/libraries/*)), $(shell sed -ne "s/^ *\# *include *[<\"]\(.*\)\.h[>\"]/\1/p" $(LOCAL_SRCS)))
    ARDUINO_LIBS += $(filter $(notdir $(wildcard ../libraries/*)), \
        $(shell sed -ne "s/^ *\# *include *[<\"]\(.*\)\.h[>\"]/\1/p" $(SRCS)))
endif
########################################################################

ARDUINO			= $(INSTALL_DIR)
ARDUINO_LIB 		=  /home/matthew/arduino/arduino/hardware/arduino/cores/arduino
AVR_TOOLS_PATH		= /usr/bin
AVRDUDE_PATH		= /usr/bin

FORMAT			= ihex



# Name of this Makefile (used for "make depend").
MAKEFILE = Makefile

# Debugging format.
# Native formats for AVR-GCC's -g are stabs [default], or dwarf-2.
# AVR (extended) COFF requires stabs, plus an avr-objcopy run.
DEBUG = stabs

# Place -I options here
CINCS = -I$(ARDUINO_LIB) -I$(ARDUINO_LIB)/variants/$(PINS_VARIANT) -I/home/matthew/arduino/arduino/libraries 
CXXINCS = -I$(ARDUINO_LIB) -I$(ARDUINO_LIB)/variants/$(PINS_VARIANT) -I/home/matthew/arduino/arduino/libraries 

CINCS			+= $(addprefix -I,$(USER_LIBS_PATHS))
CXXINCS			+= $(addprefix -I,$(USER_LIBS_PATHS))

# Compiler flag to set the C Standard level.
# c89   - "ANSI" C
# gnu89 - c89 plus GCC extensions
# c99   - ISO C99 standard (not yet fully implemented)
# gnu99 - c99 plus GCC extensions
OPT = s
# Place -D or -U options here
CDEFS = -DF_CPU=$(F_CPU) -MMD -DUSB_VID=null -DUSB_PID=null -DARDUINO=$(ARDUINO_VERS)
CXXDEFS = -DF_CPU=$(F_CPU) -MMD -DUSB_VID=null -DUSB_PID=null -DARDUINO=$(ARDUINO_VERS)

CSTANDARD = -std=gnu99
CDEBUG = -g$(DEBUG)

CWARN = -Wall -Wstrict-prototypes
CXXWARN = -Wall

CTUNING = -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -ffunction-sections -fdata-sections
CXXTUNING = -fno-exceptions -ffunction-sections -fdata-sections

CEXTRA = --pedantic 
CXXEXTRA = 

CFLAGS =  -c -g -O$(OPT) $(CSTANDARD) $(CWARN) $(CTUNING) -mmcu=$(MCU) $(CDEFS) $(CEXTRA) $(CINCS) 
CXXFLAGS = -c -g -O$(OPT) $(CXXWARN) $(CXXTUNING) -mmcu=$(MCU) $(CXXDEFS) $(CXXEXTRA) $(CXXINCS) 

LDFLAGS 		= -Wl,-Map,$(BIN_DIR)/$(TARGET).map,--cref -Wl,--gc-sections 
LDFLAGS			+= $(addprefix -L,$(ARDUINO_OBJ_DIR))
LDFLAGS			+= $(addprefix -L,$(LIB_OBJ_DIR))
LDFLAGS			+= $(addprefix -L,$(OBJ_DIR)) 

# Programming support using avrdude. Settings and variables.

AVRDUDE_PORT = $(PORT)
AVRDUDE_WRITE_FLASH 	= -Uflash:w:$(BIN_DIR)/$(TARGET).hex:i
#AVRDUDE_FUSES		= -U lfuse:w:0xFF:m -U hfuse:w:0xDA:m -U efuse:w:0x05:m
AVRDUDE_FUSES		= -U lfuse:w:0xE2:m -U hfuse:w:0xD8:m -U efuse:w:0x05:m
AVRDUDE_LOCK		= -U lock:w:0x0f:m
AVRDUDE_UNLOCK		= -U lock:w:0x3f:m

AVRDUDE_FLAGS = -patmega328p -D -P$(AVRDUDE_PORT) -c$(AVRDUDE_PROGRAMMER) -b$(UPLOAD_RATE) -u
#$(AVRDUDE_FUSES)

# Program settings
CC 		= avr-gcc
CXX 		= avr-g++
OBJCOPY 	= avr-objcopy
OBJDUMP 	= avr-objdump
AR  		= avr-ar
SIZE 		= avr-size
NM 		= avr-nm
AVRDUDE 	= $(AVRDUDE_PATH)/avrdude
REMOVE 		= rm -f
MV 		= mv -f

# Define all object files.
# OBJ = $(SRC:.c=.o) $(CXXSRC:.cpp=.o) $(ASRC:.S=.o)


# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS = $(CFLAGS) -I.
ALL_CXXFLAGS = $(CXXFLAGS) -I.

ALL_LDFLAGS =  $(LDFLAGS) -mmcu=$(MCU)

ifeq ($(strip $(LIB_OBJS)),)
LD_LIBS		:=-lcore -lm
else
LD_LIBS		:=-ltouse -lcore -lm
endif

# Targets Specific Variables
#nano:	PORT	= /dev/ttyUSB1
nano:	F_CPU	= 16000000L
nano:	UPLOAD_RATE	= 57600
nano:	PINS_VARIANT	= eightanaloginputs
nano:	AVR_PART_NO	= ATMEGA328P
nano:	AVRDUDE_PROGRAMMER= arduino
nano:	MCU		= atmega328p
nano:	elf hex upload
.nano: nano

#uno:	PORT	= /dev/ttyACM0
uno:	F_CPU	= 16000000L
uno:	UPLOAD_RATE	= 115200
uno:	PINS_VARIANT	= standard
uno:	AVR_PART_NO	= ATMEGA328P
uno:	AVRDUDE_PROGRAMMER= arduino
uno:	MCU		= atmega328p
uno:	elf hex upload
.uno:	uno

#panstamp:	PORT	= /dev/ttyUSB0
#panstamp:	PORT	= /dev/ttyUSB1
panstamp:	F_CPU	= 8000000L
panstamp:	UPLOAD_RATE	= 57600
panstamp:	PINS_VARIANT	= standard
panstamp:	AVR_PART_NO	= ATMEGA328P
panstamp:	AVRDUDE_PROGRAMMER= arduino
panstamp:	MCU		= atmega328p
panstamp:	elf hex upload
.panstamp:	panstamp
	
all: build
build: .$(CFG)

# Program the device.
upload: $(BIN_DIR)/$(TARGET).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH)



core:	$(ARDUINO_OBJ_DIR)/libcore.a
libs:   $(LIB_OBJS) $(LIB_OBJ_DIR)/libtouse.a 
ifeq ($(strip $(LIB_OBJS)),)
elf: 	core $(BIN_DIR)/$(TARGET).elf
else
elf: 	core copy_libs libs $(BIN_DIR)/$(TARGET).elf
endif
hex: 	$(BIN_DIR)/$(TARGET).hex
eep: 	$(BIN_DIR)/$(TARGET).eep
lss: 	$(BIN_DIR)/$(TARGET).lss
sym: 	$(BIN_DIR)/$(TARGET).sym


# Display size of file.
HEXSIZE = $(SIZE) --target=$(FORMAT) $(BIN_DIR)/$(TARGET).hex
ELFSIZE = $(SIZE)  $(BIN_DIR)/$(TARGET).elf
sizebefore:
	@if [ -f $(BIN_DIR)/$(TARGET).elf ]; then echo; echo $(MSG_SIZE_BEFORE); $(HEXSIZE); echo; fi

sizeafter:


.SUFFIXES: .elf .hex .eep .lss .sym

.elf.hex:
	$(OBJCOPY) -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=1 $< $*.eep 
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

# Link: create ELF output file from library.
#
$(ARDUINO_OBJ_DIR)/libcore.a: $(ARDUINO_OBJS)
	$(AR) rcs $@ $^
	#@cp -f $@ $(LIB_OBJ_DIR)
	
	
$(LIB_OBJ_DIR)/libtouse.a: $(LIB_OBJS)
	@echo $(LIB_OBJS) !!!!! lib OBJ
	$(AR) rcs $@ $^


$(BIN_DIR)/$(TARGET).elf: $(OBJS)
	echo $^
	@mkdir -p $(BIN_DIR)
	$(CC) $(ALL_LDFLAGS) $^ -o $@ $(LD_LIBS)


	
# Compile: create object files from C++ source files.
$(ARDUINO_OBJ_DIR)/%.cpp.o: $(AVR_SRCS_ROOT)/%.cpp
	@echo $(ARDUINO_OBJS)
	@mkdir -p $(ARDUINO_OBJ_DIR)
	@mkdir -p $(ARDUINO_OBJ_DIR)/avr-libc
	@echo ARDUINO LIBRARIES OBJ FILES FROM Cpp $@
	$(CXX) -c $(ALL_CXXFLAGS) $^ -o $@
	

# Compile: create object files from C source files.
$(ARDUINO_OBJ_DIR)/%.c.o: $(AVR_SRCS_ROOT)/%.c
	@mkdir -p $(ARDUINO_OBJ_DIR)
	@mkdir -p $(ARDUINO_OBJ_DIR)/avr-libc
	@echo ARDUINO LIBRARIES OBJ FILES FROM C $@
	$(CC) -c $(ALL_CFLAGS) $^ -o $@
	
# Compile: create object files from C++ source files.
copy_libs:
	@echo Copying library sources into sketch directory $(LIB_SRC_DIR)
	@mkdir -p $(LIB_SRC_DIR)
	@cp -f $(patsubst %.cpp.o, %.cpp, $(LIB_SRCS)) $(LIB_SRC_DIR)
	
#$(LIB_OBJ_DIR)/%.cpp.o: $(LIB_SRC_DIR)/%.cpp
$(LIB_OBJ_DIR)/%.cpp.o:
	@echo LIBRARIES OBJ FILES FROM Cpp
	@mkdir -p $(LIB_OBJ_DIR)
	$(CXX) -c $(ALL_CXXFLAGS) $(LIB_SRC_DIR)/$*.cpp -o $@
	

# Compile: create object files from C source files.
#$(LIB_OBJ_DIR)/%.c.o: $(LIB_SRC_DIR)/%.c
$(LIB_OBJ_DIR)/%.c.o: 
	@echo LIBRARIES OBJ FILES FROM C
	@mkdir -p $(LIB_OBJ_DIR)
	$(CC) -c $(ALL_CFLAGS) $(LIB_SRC_DIR)/$*.c -o $@

	
# Compile: create object files from C++ source files.
$(OBJ_DIR)/%.cpp.o: $(SRC_DIR)/%.cpp
	@echo OBJ FILES FROM Cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) -c $(ALL_CXXFLAGS) $(SRC_DIR)/$*.cpp -o $@
	

# Compile: create object files from C source files.
$(OBJ_DIR)/%.c.o: $(SRC_DIR)/%.c
	@echo OBJ FILES FROM C
	@mkdir -p $(OBJ_DIR)
	$(CC) -c $(ALL_CFLAGS) $(SRC_DIR)/$*.c -o $@

%.cpp.o: %.cpp
	@echo $(OBJS) $* FILES FROM Cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) -c $(ALL_CXXFLAGS) $*.cpp -o $@
	

# Compile: create object files from C source files.
%.c.o: %.c
	@echo OBJ FILES FROM C
	@mkdir -p $(OBJ_DIR)
	$(CC) -c $(ALL_CFLAGS) $*.c -o $@

# Compile: create object files from C++ source files.
##%.cpp.o: %.cpp
	#@echo BOTTOM 1
	#$(CXX) -c $(ALL_CXXFLAGS) $^ -o $@
	

# Compile: create object files from C source files.
#%.c.o: %.c
#	@echo BOTTOM 2
#	$(CC) -c $(ALL_CFLAGS) $^ -o $@
#




# Target: clean project.
clean:
	@rm -f *.o *.d *.a *.so
	@rm -Rf bin.*/
	@rm -Rf bin/
	@rm -Rf obj.*/
	@rm -Rf obj/
	@rm -Rf src/

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

.PHONY:	all build elf hex eep lss sym program coff extcoff clean depend sizebefore sizeafter copy_libs 
