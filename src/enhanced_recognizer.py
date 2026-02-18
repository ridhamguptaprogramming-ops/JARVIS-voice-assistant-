#!/usr/bin/env python3
"""
Enhanced Speech Recognition with Speaker Verification
Recognizes WHO is speaking and WHAT they asked
"""

import sys
import os
import json
import hashlib
from datetime import datetime

try:
    from speech_recognition import Recognizer, Microphone, UnknownValueError, RequestError # type: ignore
except ImportError:
    print("ERROR: speech_recognition library not installed", file=sys.stderr)
    print("Install with: pip3 install SpeechRecognition pydub", file=sys.stderr)
    sys.exit(1)

# Speaker profiles stored as voice fingerprints
SPEAKERS_DB = "config/speakers.json"

def create_voice_fingerprint(audio):
    """Create a simple voice fingerprint from audio characteristics"""
    # Get basic audio stats as fingerprint
    audio_data = audio.get_raw_data()
    fingerprint = hashlib.md5(audio_data[:1000]).hexdigest()[:16]
    return fingerprint

def register_speaker(name):
    """Register a new speaker's voice (delegates to MFCC enroll)"""
    try:
        import src.speaker_recognizer as sr
    except Exception:
        try:
            import speaker_recognizer as sr
        except Exception as e:
            print(f"[JARVIS] Enrollment unavailable: {e}", file=sys.stderr)
            return False

    print(f"[JARVIS] Registering speaker: {name}", file=sys.stderr)
    # Default to 3 samples for enrollment
    try:
        sr.enroll(name, samples=3)
        print(f"[JARVIS] Speaker '{name}' registered successfully!", file=sys.stderr)
        return True
    except Exception as e:
        print(f"[JARVIS] Registration failed: {e}", file=sys.stderr)
        return False

def recognize_speaker():
    """Identify who is speaking using the MFCC speaker recognizer if available"""
    # Prefer the speaker_recognizer MFCC-based verifier
    try:
        import src.speaker_recognizer as sr
    except Exception:
        try:
            import speaker_recognizer as sr
        except Exception:
            sr = None

    if sr is not None:
        try:
            name = sr.verify()
            if name:
                return name
        except Exception:
            pass

    # Fallback to simple fingerprint method
    recognizer = Recognizer()
    if not os.path.exists(SPEAKERS_DB):
        return "Unknown"
    try:
        with Microphone() as source:
            recognizer.adjust_for_ambient_noise(source, duration=0.3)
            audio = recognizer.listen(source, timeout=5)

        fingerprint = create_voice_fingerprint(audio)

        # Load speakers database
        with open(SPEAKERS_DB, 'r') as f:
            speakers = json.load(f)

        # Simple matching - in production use ML/voice recognition
        for name, data in speakers.items():
            if data['fingerprint'] == fingerprint:
                return name

        return "Unknown"
    except:
        return "Unknown"

def recognize_speech_and_speaker():
    """
    Listens to microphone and returns:
    1. Speaker name (who is speaking)
    2. Recognized text (what they said)
    """
    recognizer = Recognizer()
    
    try:
        # Step 1: Identify speaker
        speaker = recognize_speaker()
        
        # Step 2: Recognize speech
        with Microphone() as source:
            recognizer.adjust_for_ambient_noise(source, duration=0.5)
            print("[Listening...]", file=sys.stderr, flush=True)
            audio = recognizer.listen(source, timeout=10, phrase_time_limit=5)
        
        try:
            text = recognizer.recognize_google(audio)
            # Return both speaker and text
            return f"{speaker}|{text}"
        except UnknownValueError:
            return f"{speaker}|"
        except RequestError as e:
            print(f"ERROR: Google Speech API error: {e}", file=sys.stderr)
            return f"{speaker}|"
            
    except Exception as e:
        print(f"ERROR: Microphone error: {e}", file=sys.stderr)
        return "Unknown|"

if __name__ == "__main__":
    # Check for speaker registration mode
    if len(sys.argv) > 1 and sys.argv[1] in ("register", "enroll"):
        if len(sys.argv) > 2:
            speaker_name = sys.argv[2]
            # Optional samples argument
            samples = 3
            if len(sys.argv) > 3:
                try:
                    samples = int(sys.argv[3])
                except Exception:
                    samples = 3
            # Delegate to speaker_recognizer if available
            try:
                import src.speaker_recognizer as sr
            except Exception:
                try:
                    import speaker_recognizer as sr
                except Exception:
                    sr = None

            if sr is not None:
                sr.enroll(speaker_name, samples=samples)
            else:
                register_speaker(speaker_name)
        else:
            print("Usage: python3 enhanced_recognizer.py register [name] [samples]")
    else:
        # Normal recognition mode
        result = recognize_speech_and_speaker()
        print(result)
