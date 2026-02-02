# 🎤 JARVIS Voice Assistant - Quick Reference Card

## Setup (One-Time)
```bash
cd "/Users/ridhamgupta805gmail.com/Documents/Todo Assignment/JARVIS-voice-assistant-"
make setup    # Install Python speech recognition
make run      # Build and run JARVIS
```

## Using JARVIS

### Voice Commands (Speak These)
| Command | Result |
|---------|--------|
| `hello jarvis` | Greeting |
| `what time is it` | Current time |
| `tell me a joke` | Programming joke |
| `help` | Available commands |
| `search for [query]` | Web search |
| `system info` | System details |
| `exit` / `quit` | Shut down |

### Build Commands
```bash
make              # Build JARVIS
make run          # Build and run with microphone
make setup        # Install Python dependencies
make debug        # Build with debug symbols
make clean        # Remove build files
make rebuild      # Clean and rebuild
make help         # Show all commands
```

## Microphone Features

✅ **Real-time voice input** - Speak naturally  
✅ **Google Speech API** - Accurate recognition  
✅ **Automatic fallback** - Works without microphone (keyboard)  
✅ **Voice output** - JARVIS speaks responses  

## If Microphone Doesn't Work

### Quick Fixes
```bash
# Check Python is installed
python3 --version

# Install speech recognition
pip3 install SpeechRecognition pydub

# Or use make setup
make setup
```

### Fallback
- If microphone fails, JARVIS automatically uses keyboard input
- Type commands instead of speaking
- All features still work!

## File Locations
```
src/
  ├── speech_recognizer.py   ← Speech-to-text engine
  ├── voice_input.c           ← Listens to microphone
  ├── voice_output.c          ← Speaks responses
  ├── command_processor.c     ← Understands commands
  └── jarvis.c                ← Main logic

Documentation:
  ├── QUICKSTART.md           ← Start here
  ├── MICROPHONE_SETUP.md     ← Detailed microphone guide
  ├── MICROPHONE_ENABLED.md   ← What's new
  └── README.md               ← Full documentation
```

## How It Works

```
You speak → Microphone → Python script → Google API 
   → Text → JARVIS understands → Speaks response
```

## Troubleshooting

| Problem | Solution |
|---------|----------|
| "Microphone not available" | Check microphone hardware, reinstall Python |
| "Google Speech API error" | Check internet, try again (rate limiting) |
| "Python not found" | Install Python 3: `brew install python3` |
| "Module not found" | Run `make setup` |

## Example Session

```
$ ./jarvis

[JARVIS] Ready for your command
[JARVIS] 🎤 Listening... (speak now)

You: "What time is it?"
You said: "What time is it?"

[JARVIS] The current time is 02:45 PM
[JARVIS] Speaking: The current time is 02:45 PM

[JARVIS] Ready for your command
[JARVIS] 🎤 Listening... (speak now)

You: "Exit"
You said: "Exit"

[JARVIS] Shutting down. Goodbye sir.
```

## Pro Tips

1. **Speak clearly** - Better recognition with distinct speech
2. **Reduce noise** - Quiet environment = better accuracy
3. **Hold microphone close** - ~15cm (6 inches) from mouth
4. **Complete sentences** - "What is the time?" > "time"
5. **Normal pace** - Not too fast, not too slow

## Version Info
- **Version**: 2.0.0 (with microphone)
- **Language**: C with Python helper
- **Platform**: macOS, Linux
- **Status**: ✅ Fully functional

## One-Line Start
```bash
cd "/Users/ridhamgupta805gmail.com/Documents/Todo Assignment/JARVIS-voice-assistant-" && make setup && make run
```

---

**Enjoy your voice-controlled JARVIS! 🎤🤖**
