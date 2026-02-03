CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2
CFLAGS_DEBUG = -Wall -Wextra -std=c11 -g
LDFLAGS = -lm

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
BIN_DIR = .

# Source files
SOURCES = $(SRC_DIR)/main.c $(SRC_DIR)/jarvis.c $(SRC_DIR)/voice_input.c $(SRC_DIR)/voice_output.c $(SRC_DIR)/command_processor.c $(SRC_DIR)/search.c
OBJECTS = $(BUILD_DIR)/main.o $(BUILD_DIR)/jarvis.o $(BUILD_DIR)/voice_input.o $(BUILD_DIR)/voice_output.o $(BUILD_DIR)/command_processor.o $(BUILD_DIR)/search.o
TARGET = $(BIN_DIR)/jarvis

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(OBJECTS)
	@echo "Linking $@..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Build complete: $@"

# Object file rules
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

# Debug build
debug: CFLAGS = $(CFLAGS_DEBUG)
debug: clean $(TARGET)
	@echo "Debug build complete"

# Run the application
run: $(TARGET)
	@echo "Starting JARVIS Voice Assistant..."
	@./$(TARGET)

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)
	@rm -f $(TARGET)
	@echo "Clean complete"

# Rebuild from scratch
rebuild: clean all

# Help target
help:
	@echo "JARVIS Voice Assistant - Build System"
	@echo "===================================="
	@echo "Available targets:"
	@echo "  make              - Build the project"
	@echo "  make run          - Build and run JARVIS"
	@echo "  make debug        - Build with debug symbols"
	@echo "  make clean        - Remove build artifacts"
	@echo "  make rebuild      - Clean and rebuild"
	@echo "  make setup        - Install Python dependencies for voice recognition"
	@echo "  make help         - Show this help message"

# Setup Python dependencies for speech recognition
setup:
	@echo "Setting up speech recognition dependencies..."
	@pip3 install SpeechRecognition pydub librosa sounddevice numpy scipy --quiet || pip install SpeechRecognition pydub librosa sounddevice numpy scipy --quiet
	@echo "Setup complete! JARVIS now has microphone support."
	@echo "You can now run: make run"

.PHONY: all run debug clean rebuild help setup
