#ifndef VOICE_INPUT_H
#define VOICE_INPUT_H

#include <stdlib.h>
#include <string.h>

/**
 * Captures voice input from the user and converts it to text
 * @return Dynamically allocated string containing recognized text, or NULL on failure
 */
char* capture_voice_input(void);

/**
 * Records audio to a temporary file
 * @param filename Path to save the audio file
 * @return 1 on success, 0 on failure
 */
int record_audio(const char* filename);

/**
 * Converts speech to text using system tools
 * @param audio_file Path to the audio file
 * @return Dynamically allocated string with recognized text, or NULL on failure
 */
char* speech_to_text(const char* audio_file);

#endif // VOICE_INPUT_H
