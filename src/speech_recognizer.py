#!/usr/bin/env python3
"""
Real-time Speech Recognition Module for JARVIS
Converts microphone audio to text using Google Speech Recognition API
"""

import sys
import os

try:
    from speech_recognition import Recognizer, Microphone, UnknownValueError, RequestError
except ImportError:
    print("ERROR: speech_recognition library not installed", file=sys.stderr)
    print("Install with: pip3 install SpeechRecognition pydub", file=sys.stderr)
    sys.exit(1)

def recognize_speech():
    """
    Listens to microphone input and converts speech to text.
    Returns the recognized text or None if recognition fails.
    """
    recognizer = Recognizer()
    
    try:
        # Use the default microphone as the audio source
        with Microphone() as source:
            # Adjust for ambient noise
            recognizer.adjust_for_ambient_noise(source, duration=0.5)
            
            # Listen for audio (timeout in seconds)
            print("[Listening...]", file=sys.stderr, flush=True)
            audio = recognizer.listen(source, timeout=10, phrase_time_limit=5)
        
        # Try Google Speech Recognition
        try:
            text = recognizer.recognize_google(audio)
            return text
        except UnknownValueError:
            return None  # Audio was not understood
        except RequestError as e:
            print(f"ERROR: Google Speech API error: {e}", file=sys.stderr)
            return None
            
    except Exception as e:
        print(f"ERROR: Microphone error: {e}", file=sys.stderr)
        return None

if __name__ == "__main__":
    result = recognize_speech()
    
    if result:
        # Print the recognized text to stdout for C program to capture
        print(result)
        sys.exit(0)
    else:
        # Return empty line if recognition failed
        print("")
        sys.exit(1)
