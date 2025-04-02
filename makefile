ARCH ?= x64
OPTIMISED ?= 0

CC_64 = gcc
CC_32 = i686-w64-mingw32-gcc

CFLAGS = -Wall -Wextra
CFLAGS_OPT = -s -O2 -ffunction-sections -fdata-sections -Wl,--gc-sections
LIBS = -lwlanapi
SRC = wifistr.c
TARGET = wifistr.exe

ifeq ($(ARCH),x64)
    CC = $(CC_64)
    OUTPUT_DIR = bin/x64
else ifeq ($(ARCH),x32)
    CC = $(CC_32)
    OUTPUT_DIR = bin/x32
else
    $(error Unsupported ARCH: $(ARCH). Use 'x64' or 'x32')
endif

ifeq ($(OPTIMISED),1)
    CFLAGS += $(CFLAGS_OPT)
endif

# Create output directory
$(shell if not exist "$(OUTPUT_DIR)" mkdir "$(OUTPUT_DIR)")

all: $(OUTPUT_DIR) $(OUTPUT_DIR)/$(TARGET)

$(OUTPUT_DIR)/$(TARGET): $(SRC)
	$(CC) $(SRC) -o $@ $(LIBS) $(CFLAGS)

clean:
	if exist "$(OUTPUT_DIR)\$(TARGET)" del /q "$(OUTPUT_DIR)\$(TARGET)"

.PHONY: all clean
