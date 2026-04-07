#include "../include/voice_input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define CLR_RESET  "\033[0m"
#define CLR_CYAN   "\033[1;36m"
#define CLR_YELLOW "\033[1;33m"
#define CLR_GREEN  "\033[1;32m"

static int env_flag_enabled(const char* value) {
    if (!value) return 0;
    return strcmp(value, "1") == 0 || strcmp(value, "true") == 0 ||
           strcmp(value, "TRUE") == 0 || strcmp(value, "yes") == 0 ||
           strcmp(value, "YES") == 0 || strcmp(value, "on") == 0;
}

/* Try one speech recognition attempt; returns heap string or NULL */
static char* try_speech(void) {
    printf(CLR_GREEN "  🎤  Listening...\n" CLR_RESET);
    fflush(stdout);

    FILE* pipe = popen("python3 src/speech_recognizer.py 2>/dev/null", "r");
    if (!pipe) return NULL;

    char buf[512] = "";
    if (fgets(buf, sizeof(buf), pipe) != NULL)
        buf[strcspn(buf, "\n")] = '\0';
    pclose(pipe);

    if (strlen(buf) == 0) return NULL;

    char* result = (char*)malloc(512);
    if (!result) return NULL;
    strncpy(result, buf, 511);
    result[511] = '\0';
    return result;
}

char* capture_voice_input(void) {
    char* combined = (char*)malloc(768);
    if (!combined) return NULL;
    combined[0] = '\0';

    /* ── Attempt 1 ── */
    char* text = try_speech();

    /* ── Retry once on timeout/empty ── */
    if (!text) {
        printf(CLR_YELLOW "  [JARVIS] No speech detected. Retrying...\n" CLR_RESET);
        text = try_speech();
    }

    if (text) {
        printf(CLR_CYAN "  🧠  Processing...\n" CLR_RESET);

        char speaker[128] = "UNKNOWN";
        if (env_flag_enabled(getenv("JARVIS_VERIFY_SPEAKER"))) {
            FILE* spipe = popen("python3 src/speaker_recognizer.py 2>/dev/null", "r");
            if (spipe) {
                if (fgets(speaker, sizeof(speaker), spipe) != NULL)
                    speaker[strcspn(speaker, "\n")] = '\0';
                pclose(spipe);
            }
        }
        if (speaker[0] == '\0') strcpy(speaker, "UNKNOWN");

        snprintf(combined, 768, "%s|%s", speaker, text);

        if (strcmp(speaker, "UNKNOWN") != 0)
            printf(CLR_GREEN "  💬  (%s) You said: \"%s\"\n" CLR_RESET, speaker, text);
        else
            printf(CLR_GREEN "  💬  You said: \"%s\"\n" CLR_RESET, text);

        free(text);
        return combined;
    }

    /* ── Keyboard fallback ── */
    printf(CLR_YELLOW
           "  [JARVIS] Microphone unavailable or no speech detected.\n"
           "           Switching to keyboard input.\n"
           CLR_RESET);
    printf(CLR_CYAN "  ❯ Type your command: " CLR_RESET);
    fflush(stdout);

    char kb_buf[512] = "";
    if (fgets(kb_buf, sizeof(kb_buf), stdin) != NULL) {
        kb_buf[strcspn(kb_buf, "\n")] = '\0';
        if (strlen(kb_buf) > 0) {
            snprintf(combined, 768, "KEYBOARD|%s", kb_buf);
            return combined;
        }
    }

    free(combined);
    return NULL;
}

int record_audio(const char* filename) {
    (void)filename;
    printf(CLR_CYAN "  [JARVIS] Voice input ready.\n" CLR_RESET);
    return 1;
}

char* speech_to_text(const char* audio_file) {
    (void)audio_file;
    char* s = (char*)malloc(256);
    if (!s) return NULL;
    strcpy(s, "");
    return s;
}
