#include "../include/jarvis.h"
#include "../include/voice_input.h"
#include "../include/voice_output.h"
#include "../include/command_processor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>

/* ── ANSI colour macros ─────────────────────────────────────────────────── */
#define CLR_RESET  "\033[0m"
#define CLR_CYAN   "\033[1;36m"
#define CLR_GREEN  "\033[1;32m"
#define CLR_YELLOW "\033[1;33m"
#define CLR_RED    "\033[1;31m"
#define CLR_BOLD   "\033[1m"

/* ── Context memory (last 5 commands) ───────────────────────────────────── */
#define MEMORY_SIZE 5
static char g_memory[MEMORY_SIZE][512];
static int  g_memory_count = 0;

void memory_push(const char* cmd) {
    if (!cmd || strlen(cmd) == 0) return;
    if (g_memory_count < MEMORY_SIZE) {
        strncpy(g_memory[g_memory_count], cmd, 511);
        g_memory[g_memory_count][511] = '\0';
        g_memory_count++;
    } else {
        /* shift left */
        for (int i = 0; i < MEMORY_SIZE - 1; i++)
            memcpy(g_memory[i], g_memory[i + 1], 512);
        strncpy(g_memory[MEMORY_SIZE - 1], cmd, 511);
        g_memory[MEMORY_SIZE - 1][511] = '\0';
    }
}

const char* memory_last(void) {
    if (g_memory_count == 0) return NULL;
    return g_memory[g_memory_count - 1];
}

/* Expose memory to command_processor via weak linkage alternative */
int  jarvis_memory_count(void)              { return g_memory_count; }
const char* jarvis_memory_get(int i)        { return (i >= 0 && i < g_memory_count) ? g_memory[i] : NULL; }

/* ── Timestamp helper ───────────────────────────────────────────────────── */
static void print_ts(const char* colour, const char* tag, const char* msg) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    char ts[16];
    strftime(ts, sizeof(ts), "%H:%M:%S", t);
    printf("%s[%s]%s %s%s%s %s\n", CLR_CYAN, ts, CLR_RESET, colour, tag, CLR_RESET, msg);
}

/* ── Loading bar animation ──────────────────────────────────────────────── */
static void loading_bar(const char* label, int steps, int delay_ms) {
    printf("%s  %-36s [", CLR_YELLOW, label);
    fflush(stdout);
    for (int i = 0; i < steps; i++) {
        printf("█");
        fflush(stdout);
        usleep((unsigned int)(delay_ms * 1000));
    }
    printf("]%s\n", CLR_RESET);
}

/* ── env flag helper ────────────────────────────────────────────────────── */
static int env_flag_enabled(const char* value) {
    if (!value) return 0;
    return strcmp(value, "1") == 0 || strcmp(value, "true") == 0 ||
           strcmp(value, "TRUE") == 0 || strcmp(value, "yes") == 0 ||
           strcmp(value, "YES") == 0 || strcmp(value, "on") == 0;
}

static void launch_jarvis_ui_if_enabled(void) {
    if (env_flag_enabled(getenv("JARVIS_DISABLE_UI"))) return;
    if (env_flag_enabled(getenv("JARVIS_NO_GUI")))     return;
    int status = system("python3 src/jarvis_ui.py >/dev/null 2>&1 &");
    if (status == 0)
        print_ts(CLR_GREEN, "[OK]", "UI window launched.");
    else
        print_ts(CLR_YELLOW, "[WARN]", "UI window unavailable (GUI may be disabled).");
}

/* ── Public API ─────────────────────────────────────────────────────────── */
char* get_current_time(void) {
    char* s = (char*)malloc(100);
    if (!s) return NULL;
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    strftime(s, 100, "%Y-%m-%d %H:%M:%S", t);
    return s;
}

char* get_system_info(void) {
    char* info = (char*)malloc(256);
    if (!info) return NULL;
    char* ts = get_current_time();
    snprintf(info, 256, "JARVIS v%s - Active on %s", JARVIS_VERSION, ts ? ts : "unknown");
    free(ts);
    return info;
}

int jarvis_init(void) {
    /* ── Banner ── */
    printf("\n");
    printf(CLR_CYAN "  ╔══════════════════════════════════════════╗\n" CLR_RESET);
    printf(CLR_CYAN "  ║" CLR_BOLD "        J.A.R.V.I.S  SYSTEM  v%-6s      " CLR_RESET CLR_CYAN "║\n" CLR_RESET, JARVIS_VERSION);
    printf(CLR_CYAN "  ║     Just A Rather Very Intelligent System  ║\n" CLR_RESET);
    printf(CLR_CYAN "  ╚══════════════════════════════════════════╝\n\n" CLR_RESET);

    /* ── Boot sequence ── */
    print_ts(CLR_CYAN,   "[BOOT]", "Initializing AI core...");
    loading_bar("Loading neural modules",    10, 40);

    print_ts(CLR_CYAN,   "[BOOT]", "Loading voice recognition module...");
    loading_bar("Voice recognition",         10, 30);

    print_ts(CLR_CYAN,   "[BOOT]", "Loading voice synthesis module...");
    loading_bar("Voice synthesis",           10, 30);

    print_ts(CLR_CYAN,   "[BOOT]", "Loading command processor...");
    loading_bar("Command processor",         10, 20);

    print_ts(CLR_CYAN,   "[BOOT]", "Loading context memory...");
    loading_bar("Context memory (5 slots)",  10, 20);

    if (!voice_output_init()) {
        print_ts(CLR_RED, "[ERROR]", "Failed to initialize voice output.");
        return 0;
    }

    print_ts(CLR_GREEN, "[OK]", "All systems online. JARVIS is ready.");
    printf("\n");

    launch_jarvis_ui_if_enabled();
    notify_desktop("JARVIS", "JARVIS v2.0 is now online");
    return 1;
}

void jarvis_run(void) {
    int running = 1;
    int strict_speaker_mode = env_flag_enabled(getenv("JARVIS_STRICT_SPEAKER"));

    speak("JARVIS version 2.0 is now online. How may I assist you?");

    while (running) {
        printf("\n");
        printf(CLR_CYAN "  ═══════════════════════════════════════════\n" CLR_RESET);
        printf(CLR_BOLD "  JARVIS  ›  Ready for your command\n" CLR_RESET);
        printf(CLR_CYAN "  ═══════════════════════════════════════════\n" CLR_RESET);
        printf(CLR_YELLOW "  Try: hello · time · joke · help · search [query] · exit\n" CLR_RESET);
        printf("\n");

        /* ── Capture input ── */
        print_ts(CLR_GREEN, "🎤", "Listening...");
        char* combined = capture_voice_input();
        if (!combined) {
            print_ts(CLR_YELLOW, "[WARN]", "No input received. Please try again.");
            continue;
        }

        /* ── Split SPEAKER|TEXT ── */
        char* sep = strchr(combined, '|');
        char speaker[128]     = "";
        char command_text[512] = "";

        if (sep) {
            size_t s_len = (size_t)(sep - combined);
            if (s_len >= sizeof(speaker)) s_len = sizeof(speaker) - 1;
            strncpy(speaker, combined, s_len);
            speaker[s_len] = '\0';
            strncpy(command_text, sep + 1, sizeof(command_text) - 1);
            command_text[sizeof(command_text) - 1] = '\0';
        } else {
            strncpy(command_text, combined, sizeof(command_text) - 1);
            command_text[sizeof(command_text) - 1] = '\0';
            strcpy(speaker, "UNKNOWN");
        }

        /* ── Strict speaker check ── */
        if (strict_speaker_mode && strcmp(speaker, "UNKNOWN") == 0) {
            print_ts(CLR_YELLOW, "[WARN]", "Speaker not recognized. Requesting confirmation...");
            int confirmed = 0;
            FILE* cpipe = popen("python3 src/speech_recognizer.py 2>/dev/null", "r");
            if (cpipe) {
                char conf_buf[256] = "";
                if (fgets(conf_buf, sizeof(conf_buf), cpipe) != NULL) {
                    conf_buf[strcspn(conf_buf, "\n")] = 0;
                    for (char* p = conf_buf; *p; ++p) *p = (char)tolower((unsigned char)*p);
                    if (strstr(conf_buf, "yes") || strstr(conf_buf, "confirm") ||
                        strstr(conf_buf, "ok")  || strstr(conf_buf, "execute") ||
                        strstr(conf_buf, "run")) confirmed = 1;
                }
                pclose(cpipe);
            }
            if (!confirmed) {
                printf(CLR_YELLOW "  [JARVIS] Confirm execution? (yes/no): " CLR_RESET);
                fflush(stdout);
                char kb[32] = "";
                if (fgets(kb, sizeof(kb), stdin) != NULL) {
                    kb[strcspn(kb, "\n")] = 0;
                    for (char* p = kb; *p; ++p) *p = (char)tolower((unsigned char)*p);
                    if (strcmp(kb, "yes") == 0 || strcmp(kb, "y") == 0) confirmed = 1;
                }
            }
            if (!confirmed) {
                print_ts(CLR_YELLOW, "[WARN]", "Command cancelled — no confirmation.");
                free(combined);
                continue;
            }
            print_ts(CLR_YELLOW, "[WARN]", "Unverified source confirmed — proceeding with caution.");
        } else if (strcmp(speaker, "UNKNOWN") == 0) {
            strcpy(speaker, "GUEST");
        }

        if (strlen(command_text) == 0) {
            print_ts(CLR_YELLOW, "[WARN]", "Empty command. Please try again.");
            free(combined);
            continue;
        }

        /* ── Context memory: handle recall commands before processing ── */
        char lower_cmd_check[512];
        for (size_t i = 0; command_text[i] && i < sizeof(lower_cmd_check) - 1; i++)
            lower_cmd_check[i] = (char)tolower((unsigned char)command_text[i]);
        lower_cmd_check[strlen(command_text)] = '\0';

        if (strstr(lower_cmd_check, "repeat last") || strstr(lower_cmd_check, "last command")) {
            const char* last = memory_last();
            if (last) {
                print_ts(CLR_CYAN, "🧠", "Recalling last command...");
                printf(CLR_CYAN "  Last command: \"%s\"\n" CLR_RESET, last);
                strncpy(command_text, last, sizeof(command_text) - 1);
                command_text[sizeof(command_text) - 1] = '\0';
            } else {
                print_ts(CLR_YELLOW, "[INFO]", "No previous commands in memory.");
                free(combined);
                continue;
            }
        } else if (strstr(lower_cmd_check, "what did i say") || strstr(lower_cmd_check, "history")) {
            printf(CLR_CYAN "\n  ── Command History ──────────────────────\n" CLR_RESET);
            if (g_memory_count == 0) {
                printf("  (empty)\n");
            } else {
                for (int i = g_memory_count - 1; i >= 0; i--)
                    printf(CLR_YELLOW "  [%d] %s\n" CLR_RESET, g_memory_count - i, g_memory[i]);
            }
            printf(CLR_CYAN "  ─────────────────────────────────────────\n\n" CLR_RESET);
            free(combined);
            continue;
        }

        /* ── Store in memory ── */
        memory_push(command_text);

        print_ts(CLR_CYAN, "🧠", "Processing...");
        printf(CLR_BOLD "  [%s] › %s\n" CLR_RESET, speaker, command_text);

        /* ── Process ── */
        char* response = process_command(command_text);
        if (!response) {
            print_ts(CLR_RED, "[ERROR]", "Command processing failed.");
            free(combined);
            continue;
        }

        print_ts(CLR_GREEN, "💬", "Responding...");
        printf(CLR_GREEN "  JARVIS › %s\n" CLR_RESET, response);
        speak(response);
        notify_desktop("JARVIS", response);

        /* ── Exit check ── */
        if (strstr(lower_cmd_check, "quit") || strstr(lower_cmd_check, "exit") ||
            strstr(lower_cmd_check, "shutdown")) {
            running = 0;
        }

        free(response);
        free(combined);
    }
}

void jarvis_cleanup(void) {
    printf("\n");
    print_ts(CLR_YELLOW, "[SHUTDOWN]", "Powering down subsystems...");
    print_ts(CLR_YELLOW, "[SHUTDOWN]", "All systems offline.");
    printf(CLR_CYAN "\n  Goodbye, sir.\n\n" CLR_RESET);
}
