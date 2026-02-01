#ifndef VOICE_OUTPUT_H
#define VOICE_OUTPUT_H

#include <stdlib.h>

/**
 * Speaks out the given text using text-to-speech
 * @param text The text to be spoken
 * @return 1 on success, 0 on failure
 */
int speak(const char* text);

/**
 * Initialize voice output system
 * @return 1 on success, 0 on failure
 */
int voice_output_init(void);

#endif // VOICE_OUTPUT_H
