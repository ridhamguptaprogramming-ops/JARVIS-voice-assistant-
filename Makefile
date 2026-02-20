CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2
CFLAGS_DEBUG = -Wall -Wextra -std=c11 -g
LDFLAGS = -lm

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
BIN_DIR = .
TEST_DIR = tests
TEST_BUILD_DIR = $(BUILD_DIR)/tests

# Source files
SOURCES = $(SRC_DIR)/main.c $(SRC_DIR)/jarvis.c $(SRC_DIR)/voice_input.c $(SRC_DIR)/voice_output.c $(SRC_DIR)/command_processor.c $(SRC_DIR)/search.c
OBJECTS = $(BUILD_DIR)/main.o $(BUILD_DIR)/jarvis.o $(BUILD_DIR)/voice_input.o $(BUILD_DIR)/voice_output.o $(BUILD_DIR)/command_processor.o $(BUILD_DIR)/search.o
TARGET = $(BIN_DIR)/jarvis
TEST_TARGET = $(TEST_BUILD_DIR)/test_suite
TEST_SOURCES = $(TEST_DIR)/test_suite.c

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

# Run JARVIS in a new terminal window (GUI)
run-gui: $(TARGET)
	@echo "Launching JARVIS in a new terminal window..."
	@bash scripts/run_jarvis_gui.sh

# Run JARVIS desktop UI directly
run-ui:
	@echo "Launching JARVIS desktop UI..."
	@python3 src/jarvis_ui.py

# Run JARVIS browser UI directly from HTML
run-web-ui:
	@echo "Launching JARVIS browser UI..."
	@bash scripts/run_jarvis_web_ui.sh

# Build and run C tests
test: $(TEST_TARGET)
	@echo "Running C test suite..."
	@./$(TEST_TARGET)

# Run automated Jarvis feature demo-test
demo-test: $(TARGET)
	@echo "Running automated JARVIS demo-test..."
	@bash demo.sh

$(TEST_TARGET): $(TEST_SOURCES) $(SRC_DIR)/command_processor.c $(SRC_DIR)/search.c include/command_processor.h include/search.h
	@mkdir -p $(TEST_BUILD_DIR)
	@echo "Compiling test suite..."
	$(CC) $(CFLAGS) -I$(INC_DIR) $(TEST_SOURCES) $(SRC_DIR)/command_processor.c $(SRC_DIR)/search.c -o $(TEST_TARGET) $(LDFLAGS)

# Enroll a new speaker profile. Usage: make enroll ENROLL_NAME="Your Name"
enroll:
	@if [ -z "$(ENROLL_NAME)" ]; then \
		echo "Usage: make enroll ENROLL_NAME=\"Your Name\""; \
		exit 1; \
	fi
	@echo "Enrolling speaker: $(ENROLL_NAME)"
	@python3 src/speaker_recognizer.py enroll "$(ENROLL_NAME)"

# Multi-sample enrollment: make enroll-multi ENROLL_NAME="Name" SAMPLES=5
enroll-multi:
	@if [ -z "$(ENROLL_NAME)" ]; then \
		echo "Usage: make enroll-multi ENROLL_NAME=\"Your Name\" SAMPLES=3"; \
		exit 1; \
	fi
	@SAMPLES_VAL=$(SAMPLES); if [ -z "$$SAMPLES_VAL" ]; then SAMPLES_VAL=3; fi; \
	@echo "Enrolling speaker: $(ENROLL_NAME) (samples=$$SAMPLES_VAL)"; \
	@python3 src/speaker_recognizer.py enroll "$(ENROLL_NAME)" $$SAMPLES_VAL

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
	@echo "  make run-gui      - Launch JARVIS CLI in a new terminal window"
	@echo "  make run-ui       - Launch desktop Tkinter UI"
	@echo "  make run-web-ui   - Open browser HTML UI directly"
	@echo "  make test         - Build and run C test suite"
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

.PHONY: all run run-gui run-ui run-web-ui test demo-test debug clean rebuild help setup
