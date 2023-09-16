ifeq (,$(CROSS_COMPILE))
$(error missing CROSS_COMPILE for this toolchain)
endif

TARGET1 = rapid-splash
TARGET2 = gif-deconstructor

CC = $(CROSS_COMPILE)gcc
CFLAGS = -marm -mtune=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -march=armv7ve -I./include
LDFLAGS = -Wl,-rpath-link=/opt/miyoomini-toolchain/arm-linux-gnueabihf/libc/usr/lib -L./lib
LIBS_SDL = -L./lib -lSDL -lSDL_image
LIBS_IMG = -L./lib -lgif -lpng

SRC_DIR = ./src

all: $(TARGET1) $(TARGET2)

$(TARGET1): $(SRC_DIR)/main.c 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET1) $(SRC_DIR)/main.c $(LIBS_SDL)

$(TARGET2): $(SRC_DIR)/deconstructor.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET2) $(SRC_DIR)/deconstructor.c $(LIBS_IMG)

clean:
	rm -f $(TARGET1) $(TARGET2)