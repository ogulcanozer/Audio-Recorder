# ----------------------------------------------------------------------------
#         ATMEL Microcontroller Software Support 
# ----------------------------------------------------------------------------
# Copyright (c) 2008, Atmel Corporation
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# - Redistributions of source code must retain the above copyright notice,
# this list of conditions and the disclaimer below.
#
# Atmel's name may not be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
# DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
# EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# ----------------------------------------------------------------------------

# 	Makefile for compiling the Getting Started project

#-------------------------------------------------------------------------------
#		User-modifiable options
#-------------------------------------------------------------------------------

# Chip & board used for compilation
# (can be overriden by adding CHIP=chip and BOARD=board to the command-line)
CHIP  = at91sam7s256
MEMORIES = sram

#-------------------------------------------------------------------------------
#		Directories
#-------------------------------------------------------------------------------

# List of subdirectories
FILTER_LIST = Makefile bin obj
SUB_DIRS = $(filter-out $(wildcard *.*), $(filter-out $(FILTER_LIST), $(wildcard *)))
BUILD_DIRS = $(addsuffix .bdir, $(SUB_DIRS))
HIERPATH = ./
NEWHIERPATH = $(addsuffix ../, $(HIERPATH))

# AT91 library directory
ifeq (0, $(MAKELEVEL))
AT91LIB = ../at91lib
OBJ = obj
BIN = bin
else
AT91LIB = $(addsuffix ../at91lib, $(HIERPATH))
OBJ = $(addsuffix obj, $(HIERPATH))
BIN = $(addsuffix bin, $(HIERPATH))
endif

#-------------------------------------------------------------------------------
#		Tools
#-------------------------------------------------------------------------------

# Tool suffix when cross-compiling
CROSS = arm-none-eabi-

# Compilation tools
CC = $(CROSS)gcc
SIZE = $(CROSS)size
STRIP = $(CROSS)strip
OBJCOPY = $(CROSS)objcopy

# Flags
INCLUDES = -I$(AT91LIB)/include -I. -I$(HIERPATH)

CFLAGS = -mlong-calls -ffunction-sections -Wall -fshort-enums -mcpu=arm7tdmi -msoft-float -fno-builtin
CFLAGS += -g $(OPTIMIZATION) $(INCLUDES) -D$(CHIP)
ASFLAGS = -g $(OPTIMIZATION) $(INCLUDES) -D$(CHIP) -D__ASSEMBLY__
LDFLAGS = -g $(OPTIMIZATION) -nostartfiles -Wl,--gc-sections -L$(AT91LIB)/lib

#-------------------------------------------------------------------------------
#		Files
#-------------------------------------------------------------------------------

# Objects built from C source files
C_OBJECTS = $(addprefix $(OBJ)/, $(addsuffix .o, $(basename $(wildcard *.c))))

# Objects built from Assembly source files
ASM_OBJECTS = $(addprefix $(OBJ)/, $(addsuffix .o, $(basename $(wildcard *.S))))

HDR_FILES = $(wildcard *.h)

#-------------------------------------------------------------------------------
#		Rules
#-------------------------------------------------------------------------------

all: $(BIN) $(OBJ) $(BUILD_DIRS) $(C_OBJECTS) $(ASM_OBJECTS) $(MEMORIES)

$(BIN) $(OBJ):
	mkdir $@

$(C_OBJECTS): $(OBJ)/%.o: %.c $(HDR_FILES)
	$(CC) $(CFLAGS) -c -o $@ $<

$(ASM_OBJECTS): $(OBJ)/%.o: %.S
	$(CC) $(ASFLAGS) -c -o $@ $<
	
$(BUILD_DIRS):
	cp Makefile $(basename $@)/Makefile
	$(MAKE) -C $(basename $@) all HIERPATH=$(NEWHIERPATH)

define RULES

$(1): 
	$(CC) $(LDFLAGS) -T"$(AT91LIB)/lib/$$@.lds" $(AT91LIB)/lib/crt0.o -lmachdep -o $(BIN)/$$@.elf $(OBJ)/*.o -lboardlib -lbasicIO -lg -lnosys
	$(OBJCOPY) -O binary $(BIN)/$$@.elf $(BIN)/$$@.bin
	$(SIZE) $$^ $(BIN)/$$@.elf
	
endef

ifeq (0, $(MAKELEVEL))
$(foreach MEMORY, $(MEMORIES), $(eval $(call RULES,$(MEMORY))))
else
$(MEMORIES):
endif

clean:
	-rm -f $(OBJ)/*.o $(BIN)/*.bin $(BIN)/*.elf

