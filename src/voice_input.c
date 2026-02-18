#include "../include/voice_input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

static int env_flag_enabled(const char* value) {
    if (!value) {
        return 0;
    }
    if (strcmp(value, "1") == 0 || strcmp(value, "true") == 0 || strcmp(value, "TRUE") == 0 ||
        strcmp(value, "yes") == 0 || strcmp(value, "YES") == 0 || strcmp(value, "on") == 0) {
        return 1;
    }
    return 0;
}

/**
 * Captures voice input from the microphone using Python speech recognition
 */
char* capture_voice_input(void) {
    printf("\n[JARVIS] 🎤 Listening... (speak now)\n");
    fflush(stdout);

    // Prioritize fast speech-to-text for smoother voice chat.
    FILE* pipe = popen("python3 src/speech_recognizer.py 2>/dev/null", "r");

    // We'll return a combined string: "SPEAKER|TEXT" where SPEAKER may be UNKNOWN or KEYBOARD
    char* combined = (char*)malloc(768);
    if (!combined) {
        if (pipe) pclose(pipe);
        return NULL;
    }
    combined[0] = '\0';

    if (pipe) {
        char text_buf[512] = "";
        if (fgets(text_buf, sizeof(text_buf), pipe) != NULL) {
            text_buf[strcspn(text_buf, "\n")] = 0;
        }
        pclose(pipe);

        if (strlen(text_buf) > 0) {
            char speaker[128] = "UNKNOWN";

            // Optional: enable speaker verification only when requested.
            const char* verify_speaker = getenv("JARVIS_VERIFY_SPEAKER");
            if (env_flag_enabled(verify_speaker)) {
                FILE* spipe = popen("python3 src/speaker_recognizer.py 2>/dev/null", "r");
                if (spipe) {
                    if (fgets(speaker, sizeof(speaker), spipe) != NULL) {
                        speaker[strcspn(speaker, "\n")] = 0;
                    }
                    pclose(spipe);
                }
            }

            if (speaker[0] == '\0') {
                strcpy(speaker, "UNKNOWN");
            }

            snprintf(combined, 768, "%s|%s", speaker, text_buf);
            if (strcmp(speaker, "UNKNOWN") != 0) {
                printf("[JARVIS] (%s) You said: \"%s\"\n", speaker, text_buf);
            } else {
                printf("[JARVIS] You said: \"%s\"\n", text_buf);
                if (env_flag_enabled(verify_speaker)) {
                    printf("[JARVIS] Speaker not verified (unknown)\n");
                }
            }
            return combined;
        }
    }

    // Fallback to keyboard input if Python script fails
    printf("[JARVIS] Microphone not available or empty input. Using keyboard input instead.\n");
    printf("[JARVIS] Type your command: ");
    fflush(stdout);

    char kb_buf[512] = "";
    if (fgets(kb_buf, sizeof(kb_buf), stdin) != NULL) {
        kb_buf[strcspn(kb_buf, "\n")] = 0;
        if (strlen(kb_buf) > 0) {
            snprintf(combined, 768, "KEYBOARD|%s", kb_buf);
            return combined;
        }
    }

    free(combined);
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
