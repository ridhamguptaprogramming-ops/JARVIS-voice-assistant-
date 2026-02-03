# Speaker Enrollment

This project supports enrolling speaker profiles so JARVIS can verify who is speaking before executing voice commands.

Quick enroll:

```bash
make setup
make enroll ENROLL_NAME="Your Name"
```

Alternatively:

```bash
python3 src/speaker_recognizer.py enroll "Your Name"
```

Notes:
- Speak a short, clear phrase when prompted.
- The enrollment uses MFCC-based averaging; accuracy is prototype-level.
- For better results, enroll with multiple samples (future improvement).
