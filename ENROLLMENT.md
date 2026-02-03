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

Multi-sample enrollment

You can enroll with multiple samples (recommended) to improve verification accuracy:

```bash
# enroll with 5 samples
make enroll-multi ENROLL_NAME="Your Name" SAMPLES=5

# or use the script directly
python3 src/speaker_recognizer.py enroll "Your Name" 5
```
