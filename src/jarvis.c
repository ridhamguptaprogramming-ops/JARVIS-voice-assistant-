#include "../include/jarvis.h"
#include "../include/voice_input.h"
#include "../include/voice_output.h"
#include "../include/command_processor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

/**
 * Gets the current time as a formatted string
 */
char* get_current_time(void) {
    char* time_str = (char*)malloc(100);
    if (!time_str) return NULL;
    
    time_t now = time(NULL);
    struct tm* timeinfo = localtime(&now);
    strftime(time_str, 100, "%Y-%m-%d %H:%M:%S", timeinfo);
    
    return time_str;
}

/**
 * Gets system information
 */
char* get_system_info(void) {
    char* info = (char*)malloc(256);
    if (!info) return NULL;
    
    snprintf(info, 256, "JARVIS v%s - Active on %s", JARVIS_VERSION, get_current_time());
    
    return info;
}

/**
 * Initializes the JARVIS voice assistant
 */
int jarvis_init(void) {
    printf("\n");
    printf("╔════════════════════════════════════════╗\n");
    printf("║          JARVIS Voice Assistant        ║\n");
    printf("║              v%s                      ║\n", JARVIS_VERSION);
    printf("╚════════════════════════════════════════╝\n");
    printf("\n");
    
    printf("[JARVIS] Initializing systems...\n");
    printf("[JARVIS] Loading voice recognition module...\n");
    printf("[JARVIS] Loading voice synthesis module...\n");
    printf("[JARVIS] Loading command processor...\n");
    printf("[JARVIS] All systems online. Ready to assist.\n\n");
    
    if (!voice_output_init()) {
        fprintf(stderr, "[JARVIS] Error: Failed to initialize voice output\n");
        return 0;
    }
    
    return 1;
}

/**
 * Runs the main event loop for JARVIS
 */
void jarvis_run(void) {
    int running = 1;
    
    speak("JARVIS is now online. How may I assist you?");
    
    while (running) {
        printf("\n");
        printf("═══════════════════════════════════════\n");
        printf("[JARVIS] Ready for your command\n");
        printf("═══════════════════════════════════════\n");
        printf("Try: hello, time, joke, help, search for [query], or quit\n");

        // Use microphone/capture input (returns "SPEAKER|TEXT" or NULL)
        char* combined = capture_voice_input();
        if (!combined) {
            printf("[JARVIS] I didn't get any input. Please try again.\n");
            continue;
        }

        // Split speaker and text
        char* sep = strchr(combined, '|');
        char speaker[128] = "";
        char command_text[512] = "";

        if (sep) {
            size_t s_len = sep - combined;
            if (s_len >= sizeof(speaker)) s_len = sizeof(speaker)-1;
            strncpy(speaker, combined, s_len);
            speaker[s_len] = '\0';
            strncpy(command_text, sep+1, sizeof(command_text)-1);
            command_text[sizeof(command_text)-1] = '\0';
        } else {
            // fallback: treat entire combined as command
            strncpy(command_text, combined, sizeof(command_text)-1);
            command_text[sizeof(command_text)-1] = '\0';
            strcpy(speaker, "UNKNOWN");
        }

        // If speaker is UNKNOWN, do not execute voice commands automatically
        if (strcmp(speaker, "UNKNOWN") == 0) {
            printf("[JARVIS] Speaker not recognized — command will not be executed.\n");
            printf("[JARVIS] To register your voice, run: python3 src/speaker_recognizer.py enroll \"Your Name\"\n");
            free(combined);
            continue;
        }

        if (strlen(command_text) == 0) {
            printf("[JARVIS] I didn't hear a command. Please try again.\n");
            free(combined);
            continue;
        }

        printf("\n[JARVIS] Processing: '%s' (from %s)\n", command_text, speaker);

        // Process the command
        char* response = process_command(command_text);

        if (!response) {
            fprintf(stderr, "[JARVIS] Error processing command\n");
            free(combined);
            continue;
        }
        
        printf("[JARVIS] %s\n", response);
        speak(response);
        
        // Check for exit commands
        char* lower_input = (char*)malloc(strlen(command_text) + 1);
        if (lower_input) {
            for (int i = 0; command_text[i]; i++) {
                lower_input[i] = tolower((unsigned char)command_text[i]);
            }
            lower_input[strlen(command_text)] = '\0';

            if (strstr(lower_input, "quit") || strstr(lower_input, "exit") || 
                strstr(lower_input, "shutdown")) {
                running = 0;
            }
            free(lower_input);
        }
        
        free(response);
        free(combined);
    }
}

/**
 * Cleans up and shuts down JARVIS
 */
void jarvis_cleanup(void) {
    printf("\n[JARVIS] Shutting down...\n");
    printf("[JARVIS] All systems offline.\n");
    printf("[JARVIS] Goodbye sir.\n\n");
}
