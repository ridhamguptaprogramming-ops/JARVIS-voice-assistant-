#include "../include/jarvis.h"
#include "../include/voice_input.h"
#include "../include/voice_output.h"
#include "../include/command_processor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

static int env_flag_enabled(const char* value) {
    if (!value) {
        return 0;
    }
    return strcmp(value, "1") == 0 ||
           strcmp(value, "true") == 0 ||
           strcmp(value, "TRUE") == 0 ||
           strcmp(value, "yes") == 0 ||
           strcmp(value, "YES") == 0 ||
           strcmp(value, "on") == 0;
}

static void launch_jarvis_ui_if_enabled(void) {
    const char* disable_ui = getenv("JARVIS_DISABLE_UI");
    if (env_flag_enabled(disable_ui)) {
        return;
    }

    const char* no_gui = getenv("JARVIS_NO_GUI");
    if (env_flag_enabled(no_gui)) {
        return;
    }

    int status = system("python3 src/jarvis_ui.py >/dev/null 2>&1 &");
    if (status == 0) {
        printf("[JARVIS] UI window launched.\n");
    } else {
        printf("[JARVIS] UI window was not launched (GUI may be unavailable).\n");
    }
}

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

    launch_jarvis_ui_if_enabled();
    // Notify system that JARVIS is online
    notify_desktop("JARVIS", "JARVIS is now online") ;
    
    return 1;
}

/**
 * Runs the main event loop for JARVIS
 */
void jarvis_run(void) {
    int running = 1;
    int strict_speaker_mode = env_flag_enabled(getenv("JARVIS_STRICT_SPEAKER"));
    
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

        // If speaker is unknown, only force confirmation in strict mode.
        if (strict_speaker_mode && strcmp(speaker, "UNKNOWN") == 0) {
            printf("[JARVIS] Speaker not recognized. Asking for confirmation to execute the command...\n");

            int confirmed = 0;
            // Try a quick voice confirmation
            FILE* cpipe = popen("python3 src/speech_recognizer.py 2>/dev/null", "r");
            if (cpipe) {
                char conf_buf[256] = "";
                if (fgets(conf_buf, sizeof(conf_buf), cpipe) != NULL) {
                    conf_buf[strcspn(conf_buf, "\n")] = 0;
                    // check for yes-like responses
                    for (char* p = conf_buf; *p; ++p) *p = (char)tolower((unsigned char)*p);
                    if (strstr(conf_buf, "yes") || strstr(conf_buf, "confirm") || strstr(conf_buf, "ok") || strstr(conf_buf, "execute") || strstr(conf_buf, "run")) {
                        confirmed = 1;
                    }
                }
                pclose(cpipe);
            }

            // If not confirmed by voice, ask keyboard (fallback)
            if (!confirmed) {
                printf("[JARVIS] Confirm execution? (yes/no): ");
                fflush(stdout);
                char kb[32] = "";
                if (fgets(kb, sizeof(kb), stdin) != NULL) {
                    kb[strcspn(kb, "\n")] = 0;
                    for (char* p = kb; *p; ++p) *p = (char)tolower((unsigned char)*p);
                    if (strcmp(kb, "yes") == 0 || strcmp(kb, "y") == 0) confirmed = 1;
                }
            }

            if (!confirmed) {
                printf("[JARVIS] Command cancelled (no confirmation).\n");
                printf("[JARVIS] To register your voice, run: python3 src/speaker_recognizer.py enroll \"Your Name\"\n");
                free(combined);
                continue;
            }

            printf("[JARVIS] Confirmation received from unverified source — proceeding with caution.\n");
        } else if (strcmp(speaker, "UNKNOWN") == 0) {
            strcpy(speaker, "GUEST");
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
        // Show desktop notification for the result
        notify_desktop("JARVIS", response);
        
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
