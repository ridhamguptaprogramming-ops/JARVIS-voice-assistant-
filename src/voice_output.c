#include "../include/voice_output.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define CLR_RESET  "\033[0m"
#define CLR_YELLOW "\033[1;33m"
#define CLR_GREEN  "\033[1;32m"

static int run_process_wait(const char* program, char* const argv[]) {
    pid_t pid = fork();
    if (pid < 0) return 0;
    if (pid == 0) { execvp(program, argv); _exit(127); }
    int status = 0;
    if (waitpid(pid, &status, 0) < 0) return 0;
    return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}

/* Check whether a command exists on PATH */
static int command_exists(const char* cmd) {
    char check[128];
    snprintf(check, sizeof(check), "command -v %s >/dev/null 2>&1", cmd);
    return system(check) == 0;
}

static int g_tts_available = -1;   /* -1 = not yet probed */
static char g_tts_engine[32] = ""; /* "say", "espeak", "festival", or "" */

static void probe_tts(void) {
    if (g_tts_available != -1) return;

#ifdef __APPLE__
    if (command_exists("say")) {
        g_tts_available = 1;
        strcpy(g_tts_engine, "say");
        return;
    }
#else
    if (command_exists("espeak")) {
        g_tts_available = 1;
        strcpy(g_tts_engine, "espeak");
        return;
    }
    if (command_exists("festival")) {
        g_tts_available = 1;
        strcpy(g_tts_engine, "festival");
        return;
    }
#endif
    g_tts_available = 0;
    g_tts_engine[0] = '\0';
}

int speak(const char* text) {
    if (!text || strlen(text) == 0) return 0;

    probe_tts();

    if (!g_tts_available) {
        printf(CLR_YELLOW
               "  [JARVIS] Voice output unavailable. Switching to text mode.\n"
               CLR_RESET);
        /* Text already printed by jarvis_run — nothing more needed */
        return 1;
    }

    int result = 0;
    if (strcmp(g_tts_engine, "say") == 0) {
        char* args[] = {"say", "-v", "Alex", (char*)text, NULL};
        result = run_process_wait("say", args);
    } else if (strcmp(g_tts_engine, "espeak") == 0) {
        char* args[] = {"espeak", (char*)text, NULL};
        result = run_process_wait("espeak", args);
    } else if (strcmp(g_tts_engine, "festival") == 0) {
        /* festival reads from stdin */
        char cmd[2048];
        snprintf(cmd, sizeof(cmd), "echo '%s' | festival --tts 2>/dev/null", text);
        result = (system(cmd) == 0);
    }

    if (!result) {
        printf(CLR_YELLOW
               "  [JARVIS] Voice output unavailable. Switching to text mode.\n"
               CLR_RESET);
    }
    return 1;
}

int notify_desktop(const char* title, const char* message) {
    if (!title)   title   = "JARVIS";
    if (!message) message = "";

#ifdef __APPLE__
    const char* en = getenv("JARVIS_ENABLE_NOTIFICATIONS");
    if (!(en && (strcmp(en, "1") == 0 || strcmp(en, "true") == 0))) return 1;
    char* args[] = {
        "osascript",
        "-e", "on run argv",
        "-e", "set msg to item 1 of argv",
        "-e", "set ttl to item 2 of argv",
        "-e", "display dialog msg with title ttl giving up after 1",
        "-e", "end run",
        "--",
        (char*)message, (char*)title, NULL
    };
    return run_process_wait("osascript", args);
#else
    char* args[] = {"notify-send", (char*)title, (char*)message, NULL};
    return run_process_wait("notify-send", args);
#endif
}

int voice_output_init(void) {
    probe_tts();
    if (g_tts_available) {
        printf(CLR_GREEN "  [JARVIS] Voice output ready (engine: %s)\n" CLR_RESET, g_tts_engine);
    } else {
        printf(CLR_YELLOW
               "  [JARVIS] Voice output unavailable. Text-only mode active.\n"
               CLR_RESET);
    }
    return 1;
}
