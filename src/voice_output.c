#include "../include/voice_output.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * Speaks out the given text using text-to-speech
 */
int speak(const char* text) {
    if (!text || strlen(text) == 0) {
        return 0;
    }
    
    char command[1024];
    int result;
    
    printf("[JARVIS] Speaking: %s\n", text);
    
    #ifdef __APPLE__
        // macOS: Use 'say' command for text-to-speech
        snprintf(command, sizeof(command), "say -v Alex '%s'", text);
    #else
        // Linux: Use espeak if available
        snprintf(command, sizeof(command), "espeak '%s' 2>/dev/null || echo 'TTS not available'", text);
    #endif
    
    result = system(command);
    
    if (result != 0) {
        fprintf(stderr, "[JARVIS] Warning: Text-to-speech not available\n");
        // Still consider it successful for demo purposes
        return 1;
    }
    
    return 1;
}

/**
 * Initialize voice output system
 */
int voice_output_init(void) {
    printf("[JARVIS] Voice output system initialized\n");
    return 1;
}
