################################################################################
# Path
################################################################################

SOURCES_PATH := ../src
INCLUDES_PATH := ../include

################################################################################
# Toolchain
################################################################################

TOOLCHAIN_PATH := C:/MinGWToolchain/sjlj/mingw64/bin

CC := @$(TOOLCHAIN_PATH)/gcc
LL := @$(TOOLCHAIN_PATH)/g++

################################################################################
# Command(s)
################################################################################

RM := @rm -rf

################################################################################
# Artefacts
################################################################################

BUILD_ARTIFACT_NAME := sym2srec
BUILD_ARTIFACT_EXTENSION := exe
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

################################################################################
# Compiler/Linker Options
################################################################################

# Optimization Options
OPTIMIZATION_OPTIONS := -O0 

# Options to Request or Suppress Warnings 
WARN_OPTIONS := -pedantic -Wall -Wextra -Wconversion -Wcast-align -Wcast-qual -Wdisabled-optimization -Wlogical-op \
 -Wmissing-declarations -Wmissing-include-dirs -Wredundant-decls -Wshadow -Wsign-conversion -Wswitch-default -Wundef \
 -Wwrite-strings -Wfloat-equal

# Linking Options
LD_OPTIONS := -m32 -s

#h flags
H_FLAGS = "-IC:$(INCLUDES_PATH)"

#C flags
C_FLAGS := $(OPTIMIZATION_OPTIONS) $(WARN_OPTIONS) -fmessage-length=0 -m32

#Linker flags
LD_FLAGS := $(LD_OPTIONS) -o "$(BUILD_ARTIFACT)"

################################################################################
# List
################################################################################

INCLUDE_LIST := $(wildcard $(INCLUDES_PATH)/*.h) 
OBJECT_LIST := sym2srec_hash.o sym2srec_main.o sym2srec_srecord.o sym2srec_utils.o

################################################################################
# Clean Target
################################################################################	

clean: 

#	O files
ifneq ($(strip $(wildcard *.o)),)
	$(RM) $(OBJECT_LIST)	
endif

#	Exe files
ifneq ($(strip $(wildcard *.exe)),)
	$(RM) $(BUILD_ARTIFACT)
endif

#	Text files
ifneq ($(strip $(wildcard *.txt)),)
	$(RM) ln.txt
endif

################################################################################
# Init Target
################################################################################

init:

#	Create empty file (echo. not working)
	@echo. > ln.txt

################################################################################
# Finit Target
################################################################################

finit: 

#	Clean Working Directory (keep object files)
	$(RM) ln.txt

################################################################################
# Linker Target
################################################################################

$(BUILD_ARTIFACT) : $(OBJECT_LIST)
	
#   Link file	
	@echo Building target: $@
	@echo Invoking: MinGW-64 Linker
	@echo $(LD_FLAGS) $(OBJECT_LIST)
	$(LL) $(LD_FLAGS) $(OBJECT_LIST)
	@echo Finished building target: $(BUILD_ARTIFACT)
	@cat ln.txt

################################################################################
# C Source Target
################################################################################

%.o: $(SOURCES_PATH)/%.c $(INCLUDE_LIST)

	@echo $(INCLUDE_LIST)
	@echo Building file: $< 
	@echo Invoking: MinGW-64 C Compiler
	@echo $(C_FLAGS) $(H_FLAGS)
	$(CC) $(C_FLAGS) $(H_FLAGS) -o $@ -c $<
	@echo Finished building: $<
	@cat ln.txt

################################################################################
# All Target
################################################################################

all: init $(BUILD_ARTIFACT) finit
	
################################################################################
# Others
################################################################################	

.PHONY: all clean init finit 
.NOTPARALLEL: all







