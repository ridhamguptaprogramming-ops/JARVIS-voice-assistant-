# JARVIS Voice Assistant

A sophisticated voice-controlled assistant written in C, inspired by the AI from Iron Man.

## Features

- **Voice Input**: Capture and process voice commands (with fallback to text input)
- **Voice Output**: Text-to-speech responses using system audio
- **Command Processing**: Natural language command recognition and execution
- **Modular Architecture**: Separate modules for input, output, and command processing
- **Generative AI**: Integration with Gemini API for intelligent responses (See [AI_UPGRADE.md](AI_UPGRADE.md))

## System Requirements

- **OS**: macOS or Linux
- **Compiler**: GCC or Clang
- **Additional Tools**:
  - macOS: `say` command (built-in), optional `ffmpeg` for audio recording
  - Linux: `espeak` for text-to-speech, `arecord` for audio recording

## Building

### Quick Build
```bash
make
```

### Build and Run
```bash
make run
```

### Debug Build
```bash
make debug
```

### Clean Build Artifacts
```bash
make clean
```

## Usage

### Start JARVIS
```bash
./jarvis
```

### Available Commands

- **Time**: "What time is it?" - JARVIS will tell you the current time
- **Greeting**: "Hello JARVIS", "Hi", "Hey" - JARVIS will greet you
- **Help**: "Help" - Get list of available commands
- **System Info**: "System information", "Info" - Get system details
- **Joke**: "Tell me a joke" - JARVIS will tell a joke
- **Weather**: "What's the weather?" - JARVIS will explain weather access limitations
- **Shutdown**: "Exit", "Quit", "Shutdown" - Exit the application

## Project Structure

```
JARVIS-voice-assistant/
├── src/
│   ├── main.c                 # Entry point
│   ├── jarvis.c               # Main JARVIS logic
│   ├── voice_input.c          # Voice input handling
│   ├── voice_output.c         # Voice output handling
│   └── command_processor.c    # Command processing engine
├── include/
│   ├── jarvis.h
│   ├── voice_input.h
│   ├── voice_output.h
│   └── command_processor.h
├── build/                      # Build artifacts (generated)
├── Makefile                    # Build configuration
└── README.md                   # This file
```

## Code Architecture

### Main Components

1. **voice_input.c**: Handles audio recording and speech-to-text conversion
   - `capture_voice_input()` - Main function to get voice input
   - `record_audio()` - Records audio from microphone
   - `speech_to_text()` - Converts audio to text

2. **voice_output.c**: Handles text-to-speech synthesis
   - `speak()` - Converts text to speech using system TTS
   - `voice_output_init()` - Initializes the TTS system

3. **command_processor.c**: Processes and responds to commands
   - `process_command()` - Main command processing logic
   - `to_lowercase()` - Helper for case-insensitive matching
   - `command_contains()` - Keyword matching function

4. **jarvis.c**: Core assistant logic and event loop
   - `jarvis_init()` - Initializes all systems
   - `jarvis_run()` - Main event loop
   - `jarvis_cleanup()` - Shutdown procedures

## Building Options

### Compile Only (No Run)
```bash
make
```

### Full Build with Execution
```bash
make run
```

### Build with Debug Information
```bash
make debug
./jarvis
```

### Clean Everything
```bash
make clean
make rebuild  # Clean and rebuild from scratch
```

## Advanced Usage

### Custom Compiler
```bash
CC=clang make
```

### Custom Flags
```bash
CFLAGS="-Wall -Wextra -std=c99" make
```

## Extending JARVIS

To add new commands:

1. Open [src/command_processor.c](src/command_processor.c)
2. Add a new `else if` clause in the `process_command()` function
3. Implement your custom logic
4. Rebuild with `make rebuild`

Example:
```c
else if (command_contains(lower_cmd, "your keyword")) {
    strcpy(response, "Your custom response here");
}
```

## Troubleshooting

### Build Issues
- Ensure GCC or Clang is installed: `gcc --version`
- Check all source files are in `src/` directory
- Run `make clean` then `make rebuild`

### Audio Issues (macOS)
- Text-to-speech uses the `say` command (built-in)
- For voice recording, install FFmpeg: `brew install ffmpeg`

### Audio Issues (Linux)
- For text-to-speech: `sudo apt install espeak`
- For voice recording: `sudo apt install alsa-utils`

## Version History

- **v1.0.0** - Initial release with basic voice I/O and command processing
- **v1.1.0** - Added Generative AI capabilities via Python bridge

## License

This project is provided as-is for educational purposes.

## Author

JARVIS Voice Assistant - A learning project in C programming
