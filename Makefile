# Compiler and flags
CC := gcc

# Detect operating system
OS := $(shell uname -s)
ifeq ($(OS), Linux)
    FFMPEG_LIB_PATH := /usr/lib
    FFMPEG_INCLUDE_PATH := /usr/include
    LDFLAGS := -lz -lm -llzma -lpthread
else ifeq ($(OS), Darwin) # macOS
    FFMPEG_LIB_PATH := /usr/local/lib
    FFMPEG_INCLUDE_PATH := /usr/local/include
    LDFLAGS := -lz -lm -lpthread
else ifeq ($(OS), Windows_NT)
    # Assuming Windows cross-compilation with MinGW
    CC := x86_64-w64-mingw32-gcc
    FFMPEG_LIB_PATH := ./modules/lib
    FFMPEG_INCLUDE_PATH := ./modules/include
    LDFLAGS := -lz -lm -lws2_32
else
    $(error Unsupported platform: $(OS))
endif

# FFmpeg libraries to link dynamically
LIBS := -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lswresample -lswscale

# Include directories
INCLUDES := \
    -I$(FFMPEG_INCLUDE_PATH)/libavcodec \
    -I$(FFMPEG_INCLUDE_PATH)/libavdevice \
    -I$(FFMPEG_INCLUDE_PATH)/libavfilter \
    -I$(FFMPEG_INCLUDE_PATH)/libavformat \
    -I$(FFMPEG_INCLUDE_PATH)/libavutil \
    -I$(FFMPEG_INCLUDE_PATH)/libswresample \
    -I$(FFMPEG_INCLUDE_PATH)/libswscale

# Compiler and linker flags
CFLAGS := $(INCLUDES) -L$(FFMPEG_LIB_PATH)

# Directories
BUILD_DIR := build
SRC_DIR := src

# Source and object files
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC_FILES))

# Output binary name
TARGET := $(BUILD_DIR)/asciivideo

# Default rule: build the program
all: $(BUILD_DIR) $(TARGET)

# Link all object files into the final binary
$(TARGET): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $@ $(CFLAGS) $(LIBS) $(LDFLAGS)

# Compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Add a "phony" rule to avoid conflicts with files named "all" or "clean"
.PHONY: all clean run
