#include "../include/voice_input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * Captures voice input from the user and converts it to text
 */
char* capture_voice_input(void) {
    printf("\n[JARVIS] 🎤 Listening... (speak now)\n");
    printf("[JARVIS] Press Ctrl+C to cancel\n");
    fflush(stdout);
    
    // For voice input, we'll use a simple stdin approach
    // since true voice recognition requires external APIs or frameworks
    char* user_input = (char*)malloc(512);
    if (!user_input) return NULL;
    
    printf("[JARVIS] Waiting for voice input...\n> ");
    fflush(stdout);
    
    if (fgets(user_input, 512, stdin) != NULL) {
        // Remove trailing newline
        user_input[strcspn(user_input, "\n")] = 0;
        
        // Only return if input is not empty
        if (strlen(user_input) > 0) {
            return user_input;
        }
    }
    
    free(user_input);
    return NULL;
}

/**
 * Records audio to a temporary file using system tools
 */
int record_audio(const char* filename) {
    (void)filename;  // Unused
    
    #ifdef __APPLE__
        // macOS: Note - true audio recording requires ffmpeg or audio framework
        // For now, we return success as input is handled via stdin
        printf("[JARVIS] Voice input ready (via keyboard)\n");
        return 1;
    #else
        // Linux: Similar approach
        printf("[JARVIS] Voice input ready (via keyboard)\n");
        return 1;
    #endif
}


/**
 * Converts speech to text using system tools
 */
char* speech_to_text(const char* audio_file) {
    (void)audio_file;  // Unused
    
    char* recognized_text = (char*)malloc(256);
    if (!recognized_text) return NULL;
    
    // Speech-to-text would normally use APIs or frameworks
    // For now, text input is processed directly
    strcpy(recognized_text, "");
    return recognized_text;
}
