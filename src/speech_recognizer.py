#!/usr/bin/env python3
"""
Real-time Speech Recognition Module for JARVIS
Converts microphone audio to text using Google Speech Recognition API
"""

import sys
import os
import argparse
from typing import Optional

try:
    from speech_recognition import Recognizer, Microphone, UnknownValueError, RequestError
except ImportError:
    Recognizer = None
    Microphone = None
    UnknownValueError = Exception
    RequestError = Exception

DEFAULT_TIMEOUT = float(os.getenv("JARVIS_LISTEN_TIMEOUT", "10"))
DEFAULT_PHRASE_LIMIT = float(os.getenv("JARVIS_PHRASE_LIMIT", "6"))
DEFAULT_NOISE_CALIBRATION = float(os.getenv("JARVIS_NOISE_CALIBRATION", "0.8"))
DEFAULT_RETRIES = int(os.getenv("JARVIS_LISTEN_RETRIES", "2"))
DEFAULT_LANGUAGE = os.getenv("JARVIS_SPEECH_LANGUAGE", "en-US")

def parse_bool(value: Optional[str]) -> bool:
    if not value:
        return False
    return value.strip().lower() in {"1", "true", "yes", "on"}

def resolve_microphone_index() -> Optional[int]:
    if Microphone is None:
        return None

    mic_index_env = os.getenv("JARVIS_MIC_INDEX")
    mic_name_env = os.getenv("JARVIS_MIC_NAME")

    if mic_index_env:
        try:
            return int(mic_index_env)
        except ValueError:
            print(f"WARNING: Invalid JARVIS_MIC_INDEX '{mic_index_env}'. Using default mic.", file=sys.stderr)

    if mic_name_env:
        names = Microphone.list_microphone_names()
        match_name = mic_name_env.strip().lower()
        for idx, name in enumerate(names):
            if match_name in str(name).lower():
                return idx
        print(f"WARNING: Microphone containing '{mic_name_env}' not found. Using default mic.", file=sys.stderr)

    return None

def list_microphones() -> int:
    if Microphone is None:
        print("ERROR: speech_recognition library not installed", file=sys.stderr)
        print("Install with: pip3 install SpeechRecognition pydub", file=sys.stderr)
        return 1

    try:
        names = Microphone.list_microphone_names()
    except Exception as exc:
        print(f"ERROR: Unable to list microphones: {exc}", file=sys.stderr)
        return 1

    if not names:
        print("No microphones detected.")
        return 0

    for idx, name in enumerate(names):
        print(f"{idx}: {name}")
    return 0

def recognize_speech():
    """
    Listens to microphone input and converts speech to text.
    Returns the recognized text or None if recognition fails.
    """
    if Recognizer is None or Microphone is None:
        print("ERROR: speech_recognition library not installed", file=sys.stderr)
        print("Install with: pip3 install SpeechRecognition pydub", file=sys.stderr)
        return None

    recognizer = Recognizer()
    recognizer.dynamic_energy_threshold = True
    recognizer.pause_threshold = 0.7
    recognizer.non_speaking_duration = 0.4
    recognizer.operation_timeout = 10

    mic_index = resolve_microphone_index()
    attempts = max(1, DEFAULT_RETRIES)
    
    for attempt in range(1, attempts + 1):
        try:
            with Microphone(device_index=mic_index) as source:
                recognizer.adjust_for_ambient_noise(source, duration=DEFAULT_NOISE_CALIBRATION)
                print(
                    f"[Listening... attempt {attempt}/{attempts}]",
                    file=sys.stderr,
                    flush=True,
                )
                audio = recognizer.listen(
                    source,
                    timeout=DEFAULT_TIMEOUT,
                    phrase_time_limit=DEFAULT_PHRASE_LIMIT,
                )

            text = recognizer.recognize_google(audio, language=DEFAULT_LANGUAGE)
            if text:
                return text
        except UnknownValueError:
            if attempt == attempts:
                return None
        except RequestError as exc:
            print(f"ERROR: Google Speech API error: {exc}", file=sys.stderr)
            return None
        except Exception as exc:
            print(f"ERROR: Microphone error: {exc}", file=sys.stderr)
            if attempt == attempts:
                return None

    return None

def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="JARVIS speech recognition bridge")
    parser.add_argument("--list-mics", action="store_true", help="List available microphones and exit")
    return parser.parse_args()

if __name__ == "__main__":
    args = parse_args()
    if args.list_mics:
        sys.exit(list_microphones())

    if parse_bool(os.getenv("JARVIS_DEBUG_MIC")):
        print("Tip: Use `python3 src/speech_recognizer.py --list-mics` to pick a microphone.", file=sys.stderr)

    result = recognize_speech()
    
    if result:
        # Print the recognized text to stdout for C program to capture
        print(result)
        sys.exit(0)
    else:
        # Return empty line if recognition failed
        print("")
        sys.exit(1)
