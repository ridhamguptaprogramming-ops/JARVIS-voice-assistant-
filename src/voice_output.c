#include "../include/voice_output.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

static int run_process_wait(const char* program, char* const argv[]) {
    pid_t pid = fork();
    if (pid < 0) {
        return 0;
    }

    if (pid == 0) {
        execvp(program, argv);
        _exit(127);
    }

    int status = 0;
    if (waitpid(pid, &status, 0) < 0) {
        return 0;
    }
    return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}

/**
 * Speaks out the given text using text-to-speech
 */
int speak(const char* text) {
    if (!text || strlen(text) == 0) {
        return 0;
    }

    printf("[JARVIS] Speaking: %s\n", text);
    int result = 0;

    #ifdef __APPLE__
        // Use direct argv invocation to avoid shell-quoting failures on apostrophes.
        char* args[] = {"say", "-v", "Alex", (char*)text, NULL};
        result = run_process_wait("say", args);
    #else
        char* args[] = {"espeak", (char*)text, NULL};
        result = run_process_wait("espeak", args);
    #endif

    if (result != 0) {
        fprintf(stderr, "[JARVIS] Warning: Text-to-speech not available\n");
        return 1;
    }

    return 1;
}

/**
 * Send a desktop notification (macOS or Linux)
 */
int notify_desktop(const char* title, const char* message) {
    if (!title) title = "JARVIS";
    if (!message) message = "";

#ifdef __APPLE__
    // Notification syntax support can vary by environment; keep it opt-in to avoid console noise.
    const char* enable_notifications = getenv("JARVIS_ENABLE_NOTIFICATIONS");
    if (!(enable_notifications &&
          (strcmp(enable_notifications, "1") == 0 || strcmp(enable_notifications, "true") == 0))) {
        return 1;
    }

    // Pass message/title as arguments when explicitly enabled.
    char* args[] = {
        "osascript",
        "-e", "on run argv",
        "-e", "set msg to item 1 of argv",
        "-e", "set ttl to item 2 of argv",
        "-e", "display dialog msg with title ttl giving up after 1",
        "-e", "end run",
        "--",
        (char*)message,
        (char*)title,
        NULL
    };
    return run_process_wait("osascript", args);
#else
    char* args[] = {"notify-send", (char*)title, (char*)message, NULL};
    return run_process_wait("notify-send", args);
#endif
}

/**
 * Initialize voice output system
 */
int voice_output_init(void) {
    printf("[JARVIS] Voice output system initialized\n");
    return 1;
}
