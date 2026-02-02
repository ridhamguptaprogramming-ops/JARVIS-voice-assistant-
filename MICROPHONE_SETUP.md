# JARVIS Microphone Setup Guide 🎤

## Overview

JARVIS now supports **real-time microphone input** with automatic speech-to-text conversion! Speak your commands naturally and JARVIS will listen, understand, and respond.

## Quick Setup (1 minute)

### Step 1: Install Dependencies
```bash
make setup
```

This installs Python libraries for speech recognition:
- `SpeechRecognition` - Converts speech to text
- `pydub` - Audio processing

### Step 2: Run JARVIS
```bash
make run
```

### Step 3: Start Speaking!
When you see:
```
[JARVIS] 🎤 Listening... (speak now)
[JARVIS] Waiting for voice from microphone...
```

Simply **speak your command naturally**! Examples:
- "Hello JARVIS"
- "What time is it?"
- "Tell me a joke"
- "Search for Python"
- "Exit"

JARVIS will recognize your speech and respond with voice output! 🎉

## How It Works

1. **Listening** - When JARVIS asks for input, your microphone activates
2. **Processing** - Your speech is converted to text using Google Speech Recognition
3. **Understanding** - JARVIS processes your command
4. **Responding** - JARVIS speaks the response back to you using text-to-speech

## System Requirements

### macOS
- **Microphone** - Built-in or external USB microphone
- **Python 3** - For speech recognition
- **Internet connection** - For Google Speech Recognition API

Install Python if you don't have it:
```bash
brew install python3
```

### Linux
- **Microphone** - Built-in or external USB microphone
- **Python 3** - For speech recognition
- **Internet connection** - For Google Speech Recognition API
- **Audio tools** - `alsa-utils` (usually pre-installed)

Install Python if you don't have it:
```bash
sudo apt-get install python3 python3-pip
```

## Testing Your Microphone

### Test 1: Check Microphone Hardware
```bash
# macOS
# Go to System Preferences > Sound > Input and select your microphone

# Linux
alsamixer
```

### Test 2: Check Python Installation
```bash
python3 --version
pip3 --version
```

Should show Python 3.6+

### Test 3: Verify Speech Recognition Library
```bash
python3 -c "import speech_recognition; print('✓ Speech Recognition installed')"
```

If you see an error, run:
```bash
make setup
```

### Test 4: Test Direct Speech Recognition
```bash
python3 src/speech_recognizer.py
```

Speak something and it should output your text.

## Fallback to Keyboard

If your microphone isn't available or speech recognition fails, JARVIS **automatically falls back to keyboard input**:

```
[JARVIS] Microphone not available. Using keyboard input instead.
[JARVIS] Type your command: _
```

Just type your command and press Enter. Everything still works! ⌨️

## Microphone Troubleshooting

### Problem: "Microphone not available"
**Solution 1**: Check microphone hardware
```bash
# macOS
System Preferences > Sound > Input
# Select your microphone

# Linux
alsamixer
# Ensure volume is not muted
```

**Solution 2**: Reinstall speech recognition library
```bash
pip3 uninstall SpeechRecognition
make setup
```

**Solution 3**: Use keyboard fallback temporarily
JARVIS automatically falls back to keyboard input, so you can still use all features!

### Problem: "Google Speech API error"
**Causes**: No internet connection or API rate limiting

**Solutions**:
1. Check internet connection
2. Wait 30 seconds and try again (Google rate limits)
3. Use keyboard input as fallback

### Problem: "Python not found"
**Solution**: Install Python 3
```bash
# macOS
brew install python3

# Linux
sudo apt-get install python3 python3-pip
```

Then run:
```bash
make setup
```

### Problem: "Module not found: speech_recognition"
**Solution**: Reinstall dependencies
```bash
pip3 install --upgrade SpeechRecognition pydub
# or
make setup
```

### Problem: Microphone not detected on Linux
**Solution**: Check and fix ALSA settings
```bash
# List audio devices
arecord -l

# If muted, unmute
alsamixer
# Press F3 to show playback, F4 for capture
# Navigate with arrows and press M to unmute
```

## Advanced Configuration

### Increase Microphone Sensitivity
Edit `src/speech_recognizer.py` and modify the audio adjustment:
```python
# Change 0.5 to higher value for more sensitivity
recognizer.adjust_for_ambient_noise(source, duration=1.0)
```

### Increase Listening Timeout
Edit `src/speech_recognizer.py`:
```python
# Change 10 to higher value (in seconds)
audio = recognizer.listen(source, timeout=15, phrase_time_limit=5)
```

### Change Speech Recognition Engine
The default uses Google Speech Recognition. For alternatives:
```python
# Microsoft Bing Speech Recognition
text = recognizer.recognize_bing(audio)

# IBM Watson Speech to Text
text = recognizer.recognize_ibm(audio, username='...', password='...')
```

## Privacy & Security

### How Your Voice is Handled
- **Local Processing**: Audio is captured locally on your computer
- **Sent to Google**: Speech recognition requires sending audio to Google's API
- **Not Stored**: JARVIS doesn't permanently store audio or text
- **Encrypted**: Communication with Google API is encrypted

### Disable Cloud Speech Recognition
If you prefer no internet transmission, JARVIS falls back to keyboard input automatically when speech recognition fails.

## Performance Tips

1. **Speak clearly** - Distinct speech recognition works better
2. **Reduce background noise** - Quiet environment = better accuracy
3. **Use microphone close to mouth** - ~15cm (6 inches) optimal
4. **Complete sentences** - "What is the time?" vs "time" = better recognition
5. **Normal speaking pace** - Not too fast, not too slow

## Voice Input Examples

| Command | JARVIS Response |
|---------|-----------------|
| "hello jarvis" | Greeting with introduction |
| "what time is it" | Current time |
| "tell me a joke" | Programming joke |
| "help" | List of available commands |
| "search for python" | Web search for python |
| "system information" | Computer details |
| "exit" / "quit" | Shut down JARVIS |

## Features Enabled by Microphone

✅ **Voice Commands** - Speak instead of type
✅ **Natural Language** - Conversational input
✅ **Real-time Processing** - Immediate response
✅ **Voice Output** - JARVIS speaks responses
✅ **Keyboard Fallback** - Works without microphone

## Architecture

```
Microphone
    ↓
[speech_recognizer.py] (Python Speech Recognition)
    ↓
[voice_input.c] (C program receives text)
    ↓
[command_processor.c] (Process command)
    ↓
[voice_output.c] (Speak response)
    ↓
Speaker/Headphones
```

## Version Information

- **Microphone Support**: v2.0.0
- **Speech Recognition**: Google Speech Recognition API
- **Fallback**: Keyboard input
- **Platforms**: macOS, Linux
- **Language**: C with Python helper

## Support & Debugging

### Enable Detailed Logging
Run JARVIS and observe the output:
```bash
./jarvis
```

Look for these messages:
- `[Listening...]` - Microphone is active
- `You said: "..."` - Your recognized speech
- `Microphone not available` - Using keyboard fallback

### Report Issues
Check that you have:
- ✅ Python 3 installed
- ✅ SpeechRecognition library installed (`make setup`)
- ✅ Working microphone
- ✅ Internet connection
- ✅ GCC/Clang compiler

---

**Ready to experience voice-controlled JARVIS?** 🎤

```bash
make setup && make run
```

Speak your first command!
