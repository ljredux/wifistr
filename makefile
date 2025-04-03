# Default MinGW paths
MINGW64_PATH ?= C:/dev/mingw64/bin
MINGW32_PATH ?= C:/dev/mingw32/bin

# Default build configuration
ARCH ?= x64
OPTIMISED ?= 0

# Executable name
TARGET = wifistr.exe

# Set toolchain paths and output directory based on architecture
ifeq ($(ARCH),x64)
	MINGW_PATH := $(MINGW64_PATH)
	OUTPUT_DIR = bin/x64
else ifeq ($(ARCH),x32)
	MINGW_PATH := $(MINGW32_PATH)
	OUTPUT_DIR = bin/x32
else
	$(error Unsupported ARCH: $(ARCH). Use 'x64' or 'x32')
endif

# Toolchain configuration
CC = $(MINGW_PATH)/gcc
WINDRES = $(MINGW_PATH)/windres

# Compiler flags & libraries
CFLAGS = -Wall -Wextra
CFLAGS_OPT = -s -O2 -fno-asynchronous-unwind-tables -ffunction-sections -fdata-sections -Wl,--gc-sections
LIBS = -lwlanapi

# Source files
SRC = wifistr.c
RC_FILE = version.rc
RES_FILE = version.res

# Add optimisation flags, if requested
ifeq ($(OPTIMISED),1)
	CFLAGS += $(CFLAGS_OPT)
endif

# Create output directory if it doesn't exist
$(shell if not exist "$(OUTPUT_DIR)" mkdir "$(OUTPUT_DIR)")

# Main build target
all: $(OUTPUT_DIR) $(OUTPUT_DIR)/$(TARGET)

# Rule to compile the resource file
$(OUTPUT_DIR)/$(RES_FILE): $(RC_FILE)
	$(WINDRES) $(RC_FILE) -O coff -o $(OUTPUT_DIR)/$(RES_FILE)

# Rule to compile the main executable
$(OUTPUT_DIR)/$(TARGET): $(SRC) $(OUTPUT_DIR)/$(RES_FILE)
	$(CC) $(SRC) $(OUTPUT_DIR)/$(RES_FILE) -o $@ $(LIBS) $(CFLAGS)

clean:
	if exist "$(OUTPUT_DIR)\$(TARGET)" del /q "$(OUTPUT_DIR)\$(TARGET)"
	if exist "$(OUTPUT_DIR)\$(RES_FILE)" del /q "$(OUTPUT_DIR)\$(RES_FILE)"

.PHONY: all clean
