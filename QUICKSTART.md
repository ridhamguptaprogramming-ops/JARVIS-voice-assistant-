# JARVIS Voice Assistant - Quick Start Guide

## Overview

JARVIS is a command-line voice assistant written in C programming language. It features natural language command processing, text-to-speech output, and modular architecture for easy extension.

## System Requirements

- **macOS** or **Linux**
- **GCC** or **Clang** C compiler
- **Optional**: ffmpeg (for audio recording on macOS)

## Quick Start

### Step 1: Install Speech Recognition Dependencies
To enable real-time microphone input, install Python dependencies:
```bash
make setup
```

This installs:
- `SpeechRecognition` - Speech-to-text library
- `pydub` - Audio processing library

**Troubleshooting**: If you don't have Python, JARVIS will automatically fall back to keyboard input.

### Step 2: Navigate to Project Directory
```bash
cd "/Users/ridhamgupta805gmail.com/Documents/Todo Assignment/JARVIS-voice-assistant-"
```

### Step 3: Build the Project
```bash
make
```

This will compile all source files and create the `jarvis` executable.

### Step 4: Run JARVIS
```bash
./jarvis
```

Or use the convenience command:
```bash
make run
```

**Now JARVIS listens to your microphone in real-time!** 🎤

## Command Examples

Once JARVIS is running, you can give it commands. The assistant accepts both voice and text input:

| Command | JARVIS Response |
|---------|-----------------|
| `hello jarvis` | Greets you and introduces itself |
| `what time is it` | Tells you the current time |
| `tell me a joke` | Shares a programming joke |
| `help` | Lists available commands |
| `system information` | Provides system status |
| `what's the weather` | Explains weather limitations |
| `quit` / `exit` / `shutdown` | Closes the application |

## Build Options

### Standard Build
```bash
make
```
Compiles the project with optimizations enabled.

### Build and Run Immediately
```bash
make run
```
Compiles and launches JARVIS with microphone support in one command.

### Setup Microphone Support
```bash
make setup
```
Installs Python dependencies for real-time speech recognition.

### Debug Build
```bash
make debug
```
Compiles with debug symbols for troubleshooting.

### Clean Build
```bash
make clean
```
Removes all compiled files and build artifacts.

### Full Rebuild
```bash
make rebuild
```
Cleans everything and rebuilds from scratch.

### Show Help
```bash
make help
```
Displays all available make targets.

## Run Demo

A demo script is included to test JARVIS with predefined commands:

```bash
bash demo.sh
```

This will automatically build and run JARVIS with test commands, showcasing its capabilities.

## Project Structure

```
JARVIS-voice-assistant/
│
├── src/                          # Source code files
│   ├── main.c                   # Program entry point
│   ├── jarvis.c                 # Core assistant logic
│   ├── voice_input.c            # Voice/audio input handling
│   ├── voice_output.c           # Text-to-speech output
│   └── command_processor.c      # Command parsing & execution
│
├── include/                      # Header files
│   ├── jarvis.h
│   ├── voice_input.h
│   ├── voice_output.h
│   └── command_processor.h
│
├── build/                        # Compiled object files (auto-generated)
├── jarvis                        # Executable binary (auto-generated)
├── Makefile                      # Build system configuration
├── README.md                     # Detailed documentation
├── QUICKSTART.md                # This file
└── demo.sh                       # Demo script
```

## Features Explained

### Voice Input Module
- Captures audio from your microphone
- Converts speech to text (with fallback text input)
- Handles audio file management

### Voice Output Module  
- Converts text responses to speech
- Uses macOS `say` command or Linux `espeak`
- Provides natural-sounding responses

### Command Processor
- Parses user input for keywords
- Case-insensitive command matching
- Extensible command system

### Main Assistant Logic
- Event loop for continuous operation
- System initialization and cleanup
- User interaction management

## Extending JARVIS

To add new commands, edit `src/command_processor.c` and modify the `process_command()` function:

```c
else if (command_contains(lower_cmd, "your_keyword")) {
    strcpy(response, "Your response here");
}
```

Then rebuild:
```bash
make rebuild
```

## Troubleshooting

### Build Fails
**Solution**: Ensure GCC is installed and all source files are present
```bash
gcc --version
make clean
make
```

### No Audio Output
**Solution (macOS)**: Test the `say` command manually
```bash
say "Hello JARVIS"
```

**Solution (Linux)**: Install espeak
```bash
sudo apt-get install espeak
```

### Compilation Warnings
These are usually safe and indicate style suggestions, not errors. The build will still succeed.

## Performance Tips

- Use `make` for production builds (includes optimizations)
- Use `make debug` for development with better error messages
- The executable is optimized with `-O2` compiler flag

## Version Information

- **Version**: 1.0.0
- **Language**: C (C11 standard)
- **Compiler**: GCC/Clang
- **License**: Educational use

## Next Steps

1. **Customize**: Modify commands in `command_processor.c`
2. **Integrate**: Add APIs or external services
3. **Enhance**: Implement real speech recognition
4. **Deploy**: Package for distribution

## Support

For detailed information about the code structure and API, see [README.md](README.md).

---

**Created**: February 2026  
**Project**: JARVIS Voice Assistant in C
