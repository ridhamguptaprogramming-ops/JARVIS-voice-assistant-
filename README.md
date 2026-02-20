# JARVIS Voice Assistant

A sophisticated voice-controlled assistant written in C, inspired by the AI from Iron Man.

## Features

- **Voice Input**: Capture and process voice commands (with fallback to text input)
- **Enhanced Microphone Handling**: Ambient-noise calibration, retries, and selectable microphone device
- **Voice Output**: Text-to-speech responses using system audio
- **Command Processing**: Natural language command recognition and execution
- **C Workflow Automation**: Build, rebuild, test, and warning checks via voice
- **Coding Support**: Symbol search, TODO/FIXME scan, and C module scaffolding
- **AI-Style Project Bootstrap**: Create a new project, generate starter code, and open VS Code automatically
- **AI Brain Task Engine**: Orchestrate big tasks like website generation, app scaffolding, legal/court draft prep, and problem-solving reports
- **Daily Dev Routines**: Git status/pull/push and morning workflow summaries
- **Modular Architecture**: Separate modules for input, output, and command processing
- **Generative AI**: Chat plus two built-in AI features (`summary` and `ideas`) (See [AI_UPGRADE.md](AI_UPGRADE.md))
- **Desktop UI**: Dedicated JARVIS AI window with text + voice chat controls
- **Browser UI**: Direct HTML webpage UI with chat, voice input, and quick actions

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

### Run Test Suite
```bash
make test
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

### Start Desktop UI Only
```bash
make run-ui
```

### Start Browser UI (Direct HTML)
```bash
make run-web-ui
```

### Available Commands

- **Time**: "What time is it?" - JARVIS will tell you the current time
- **Greeting**: "Hello JARVIS", "Hi", "Hey" - JARVIS will greet you
- **Help**: "Help" - Get list of available commands
- **System Info**: "System information", "Info" - Get system details
- **Build/Test (C)**: "Build project", "Rebuild project", "Run tests"
- **Compiler Checks**: "Check warnings", "Show warnings"
- **Code Navigation**: "Find function process_command", "Find symbol jarvis_run", "Show todo"
- **Module Scaffold**: "Create c module parser"
- **AI Project Creation**: "Create project billing_api in python using vscode"
- **Open Existing Project**: "Open project billing_api" - Opens that folder in VS Code
- **Open Last Project**: "Open last project" - Reopens your most recently created project
- **Workflow Automation**: "Daily status", "Morning sync", "Git status", "Git pull", "Git push"
- **Project Navigation**: "Where am I", "List files", "Go to folder src", "Create folder src/api", "Open file src/main.c", "Create file notes.txt", "Create file app.py with template"
- **Joke**: "Tell me a joke" - JARVIS will tell a joke
- **Weather**: "What's the weather?" - JARVIS will explain weather access limitations
- **Shutdown**: "Exit", "Quit", "Shutdown" - Exit the application
- **AI Chat**: "Ask AI [question]", "Explain [topic]", "Write [text]" - Generates intelligent responses
- **AI Operating Modes**: "Set mode developer", "Set mode automation", "Set mode ceo", "Set mode research", "Set mode security"
- **AI Code File**: "Generate code file app.py for flask auth API" - Writes generated code directly to file
- **AI Plan Mode**: "Plan a microservice architecture for ecommerce" - Returns implementation steps
- **AI Brain Website**: "Create a website called product_hub for my startup" - Generates a website scaffold in `ai_workspace/projects/`
- **AI Brain App**: "Build an app for inventory tracking" - Generates an app scaffold in `ai_workspace/projects/`
- **AI Brain Legal/Court**: "Draft a court preparation document for landlord dispute" - Creates a legal draft in `ai_workspace/legal/`
- **AI Brain Problem Solver**: "Solve this problem: API timeout under load" - Generates a structured solution report in `ai_workspace/solutions/`
- **AI Summary**: "Summarize [topic/text]" - Generates concise summary points
- **AI Ideas**: "Give ideas about [topic]" / "Brainstorm [topic]" - Generates practical idea list
- **Open UI**: "Open Jarvis UI" / "Open AI window" - Opens the desktop UI

For full developer workflow usage, see [SEARCH_GUIDE.md](SEARCH_GUIDE.md).

## Project Structure

```
JARVIS-voice-assistant/
├── src/
│   ├── main.c                 # Entry point
│   ├── jarvis.c               # Main JARVIS logic
│   ├── ai_chat.py             # AI Engine (Python)
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

- **v1.2.0** - Added desktop UI window, improved microphone reliability, and new AI summary/ideas modes
- **v1.0.0** - Initial release with basic voice I/O and command processing
- **v1.1.0** - Added Generative AI capabilities via Python bridge

## License

This project is provided as-is for educational purposes.

## Author

JARVIS Voice Assistant - A learning project in C programming
