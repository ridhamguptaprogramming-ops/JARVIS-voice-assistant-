#include "../include/voice_input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * Captures voice input from the microphone using Python speech recognition
 */
char* capture_voice_input(void) {
    printf("\n[JARVIS] 🎤 Listening... (speak now)\n");
    printf("[JARVIS] Waiting for voice from microphone...\n");
    fflush(stdout);

    // First verify speaker (who's speaking)
    FILE* spipe = popen("python3 src/speaker_recognizer.py 2>/dev/null", "r");
    char speaker[128] = "";
    if (spipe) {
        if (fgets(speaker, sizeof(speaker), spipe) != NULL) {
            // strip newline
            speaker[strcspn(speaker, "\n")] = 0;
        }
        pclose(spipe);
    }

    if (speaker[0] != '\0' && strcmp(speaker, "UNKNOWN") != 0) {
        printf("[JARVIS] Verified speaker: %s\n", speaker);
    } else {
        printf("[JARVIS] Speaker not verified (unknown)\n");
    }

    // Then run speech-to-text
    FILE* pipe = popen("python3 src/speech_recognizer.py 2>/dev/null", "r");

    if (pipe) {
        char* user_input = (char*)malloc(512);
        if (!user_input) {
            pclose(pipe);
            return NULL;
        }

        // Read the output from Python script
        if (fgets(user_input, 512, pipe) != NULL) {
            pclose(pipe);

            // Remove trailing newline
            user_input[strcspn(user_input, "\n")] = 0;

            // Return if input is not empty
            if (strlen(user_input) > 0) {
                if (speaker[0] != '\0' && strcmp(speaker, "UNKNOWN") != 0) {
                    printf("[JARVIS] (%s) You said: \"%s\"\n", speaker, user_input);
                } else {
                    printf("[JARVIS] You said: \"%s\"\n", user_input);
                }
                return user_input;
            }
        }

        pclose(pipe);
        free(user_input);
    }
    
    // Fallback to keyboard input if Python script fails
    printf("[JARVIS] Microphone not available. Using keyboard input instead.\n");
    printf("[JARVIS] Type your command: ");
    fflush(stdout);
    
    char* user_input = (char*)malloc(512);
    if (!user_input) return NULL;
    
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
