#!/usr/bin/env python3
"""
Simple speaker verification using MFCC averages.
Usage:
  # Enroll a speaker (creates speakers/<name>.npy)
  python3 src/speaker_recognizer.py enroll <name>

  # Verify speaker (prints name or UNKNOWN)
  python3 src/speaker_recognizer.py verify

Notes:
- Requires: sounddevice, numpy, librosa, scipy
- Records 2.5 seconds of audio from default microphone
"""
import sys
import os
import time

SPEAKERS_DIR = os.path.join(os.path.dirname(__file__), '..', 'speakers')
RECORD_SECONDS = 2.5
SR = 22050

try:
    import sounddevice as sd
    import numpy as np
    import librosa
    from scipy.spatial.distance import cosine
except Exception as e:
    print("ERROR: Missing Python dependencies: sounddevice, numpy, librosa, scipy", file=sys.stderr)
    sys.exit(2)


def record_audio(duration=RECORD_SECONDS, sr=SR):
    try:
        audio = sd.rec(int(duration * sr), samplerate=sr, channels=1, dtype='float32')
        sd.wait()
        return audio.flatten()
    except Exception as e:
        print(f"ERROR: Microphone record failed: {e}", file=sys.stderr)
        return None


def compute_embedding(y, sr=SR):
    # compute MFCC and take mean over time axis
    mfcc = librosa.feature.mfcc(y=y, sr=sr, n_mfcc=20)
    return np.mean(mfcc, axis=1)


def enroll(name):
    os.makedirs(SPEAKERS_DIR, exist_ok=True)
    print(f"Enrolling speaker: {name}", file=sys.stderr)
    print("Please speak after the beep...", file=sys.stderr)
    time.sleep(0.5)
    print("Beep! Recording...", file=sys.stderr)
    y = record_audio()
    if y is None:
        print("ERROR", file=sys.stderr)
        sys.exit(1)
    emb = compute_embedding(y)
    path = os.path.join(SPEAKERS_DIR, f"{name}.npy")
    np.save(path, emb)
    print(f"ENROLLED:{name}")


def verify():
    # load enrolled speaker embeddings
    if not os.path.isdir(SPEAKERS_DIR):
        print("UNKNOWN")
        return
    files = [f for f in os.listdir(SPEAKERS_DIR) if f.endswith('.npy')]
    if not files:
        print("UNKNOWN")
        return

    print("Please speak for verification after the beep...", file=sys.stderr)
    time.sleep(0.5)
    print("Beep! Recording...", file=sys.stderr)
    y = record_audio()
    if y is None:
        print("UNKNOWN")
        return
    emb = compute_embedding(y)

    best_name = None
    best_score = 1.0
    for f in files:
        name = os.path.splitext(f)[0]
        path = os.path.join(SPEAKERS_DIR, f)
        try:
            ref = np.load(path)
            score = cosine(ref, emb)
            if score < best_score:
                best_score = score
                best_name = name
        except Exception:
            continue

    # threshold chosen experimentally; lower is more similar
    THRESHOLD = 0.45
    if best_name and best_score < THRESHOLD:
        print(f"{best_name}")
    else:
        print("UNKNOWN")


if __name__ == '__main__':
    if len(sys.argv) >= 2 and sys.argv[1] == 'enroll' and len(sys.argv) == 3:
        enroll(sys.argv[2])
        sys.exit(0)
    else:
        verify()
        sys.exit(0)
