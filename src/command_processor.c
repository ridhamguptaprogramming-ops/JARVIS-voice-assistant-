#include "../include/command_processor.h"
#include "../include/search.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

// Forward declarations for new developer tools
void execute_dev_command(const char* command, char* response, int response_size);
void execute_dev_search(const char* command, char* response, int response_size);
void execute_project_management(const char* command, char* response, int response_size);

static int run_command_capture(const char* shell_cmd, char* response, int response_size, int max_lines);
static void execute_daily_workflow_command(const char* command, char* response, int response_size);
static void execute_c_workflow_command(const char* command, char* response, int response_size);
static void execute_code_navigation_command(const char* command, char* response, int response_size);
static int extract_identifier_after_keyword(const char* command, const char* keyword, char* out, size_t out_size);
static int create_c_module_scaffold(const char* module_name, char* response, int response_size);
static int update_makefile_for_module(const char* module_name, char* error_message, int error_message_size);

/**
 * Processes a voice command and executes appropriate action
 */
char* process_command(const char* command) {
    if (!command || strlen(command) == 0) {
        char* response = (char*)malloc(256);
        strcpy(response, "I didn't catch that. Please say it again.");
        return response;
    }

    char* lower_cmd = to_lowercase(command);
    const int response_size = 1024;
    char* response = (char*)malloc(response_size);

    if (!lower_cmd || !response) {
        free(lower_cmd);
        free(response);
        return NULL;
    }

    // Time-related commands
    if (command_contains(lower_cmd, "time")) {
        time_t now = time(NULL);
        struct tm* timeinfo = localtime(&now);
        strftime(response, response_size, "The current time is %I:%M %p", timeinfo);
    }
    // Greeting commands
    else if (command_contains(lower_cmd, "hello") ||
             command_contains(lower_cmd, "hi") ||
             command_contains(lower_cmd, "hey")) {
        strcpy(response, "Hello sir. I am JARVIS. How may I assist you today?");
    }
    // Help command
    else if (command_contains(lower_cmd, "help")) {
        strcpy(response, "I can handle daily C development tasks: build project, run tests, check warnings, "
                        "find symbol <name>, create c module <name>, git status, and web search.");
    }
    // System info command
    else if (command_contains(lower_cmd, "system info") ||
             command_contains(lower_cmd, "system status") ||
             command_contains(lower_cmd, "system information") ||
             strcmp(lower_cmd, "info") == 0) {
        snprintf(response, response_size, "System information requested. JARVIS version 2.0.0 with voice control is running smoothly on macOS.");
    }
    // YouTube search commands
    else if (command_contains(lower_cmd, "youtube") ||
             command_contains(lower_cmd, "video") ||
             command_contains(lower_cmd, "watch")) {
        execute_youtube_command(lower_cmd, response, response_size);
    }
    // Web page opening commands
    else if (command_contains(lower_cmd, "open website") ||
             (command_contains(lower_cmd, "go to") && !command_contains(lower_cmd, "folder")) ||
             command_contains(lower_cmd, "visit")) {
        execute_webpage_command(lower_cmd, response, response_size);
    }
    // Open application commands
    else if (command_contains(lower_cmd, "open")) {
        execute_open_command(lower_cmd, response, response_size);
    }
    // C development workflow commands
    else if (command_contains(lower_cmd, "build project") ||
             command_contains(lower_cmd, "compile project") ||
             command_contains(lower_cmd, "rebuild project") ||
             command_contains(lower_cmd, "clean build") ||
             command_contains(lower_cmd, "run tests") ||
             command_contains(lower_cmd, "test project") ||
             command_contains(lower_cmd, "check warnings") ||
             command_contains(lower_cmd, "show warnings") ||
             command_contains(lower_cmd, "create c module") ||
             command_contains(lower_cmd, "scaffold module")) {
        execute_c_workflow_command(lower_cmd, response, response_size);
    }
    // Code navigation and coding support
    else if (command_contains(lower_cmd, "find symbol") ||
             command_contains(lower_cmd, "find function") ||
             command_contains(lower_cmd, "where is function") ||
             command_contains(lower_cmd, "search code") ||
             command_contains(lower_cmd, "show todo") ||
             command_contains(lower_cmd, "list todo") ||
             command_contains(lower_cmd, "fixme")) {
        execute_code_navigation_command(lower_cmd, response, response_size);
    }
    // Daily workflow automation
    else if (command_contains(lower_cmd, "daily workflow") ||
             command_contains(lower_cmd, "daily status") ||
             command_contains(lower_cmd, "morning sync") ||
             command_contains(lower_cmd, "review changes") ||
             command_contains(lower_cmd, "git status") ||
             command_contains(lower_cmd, "git pull") ||
             command_contains(lower_cmd, "git push")) {
        execute_daily_workflow_command(lower_cmd, response, response_size);
    }
    // Developer Workflow Automation
    else if (command_contains(lower_cmd, "git") ||
             command_contains(lower_cmd, "build") ||
             command_contains(lower_cmd, "deploy") ||
             command_contains(lower_cmd, "make")) {
        execute_dev_command(lower_cmd, response, response_size);
    }
    // Project Navigation & Management
    else if (command_contains(lower_cmd, "change directory") ||
             command_contains(lower_cmd, "go to folder") ||
             command_contains(lower_cmd, "where am i") ||
             command_contains(lower_cmd, "list files") ||
             command_contains(lower_cmd, "create file")) {
        execute_project_management(lower_cmd, response, response_size);
    }
    // Developer Search (Stack Overflow/GitHub)
    else if (command_contains(lower_cmd, "stack overflow") ||
             command_contains(lower_cmd, "github")) {
        execute_dev_search(lower_cmd, response, response_size);
    }
    // System Control
    else if (command_contains(lower_cmd, "lock screen")) {
        system("pmset displaysleepnow");
        strcpy(response, "Locking screen.");
    }
    // Joke command
    else if (command_contains(lower_cmd, "joke")) {
        strcpy(response, "Why do programmers prefer dark mode? "
                        "Because light attracts bugs!");
    }
    // Weather command
    else if (command_contains(lower_cmd, "weather")) {
        strcpy(response, "I apologize, but I don't have access to real-time weather data. "
                        "Please check a weather service for accurate information.");
    }
    // Shutdown command
    else if (command_contains(lower_cmd, "shutdown") || 
             command_contains(lower_cmd, "exit") ||
             command_contains(lower_cmd, "quit")) {
        strcpy(response, "Shutting down. Goodbye sir.");
    }
    // Search commands - only if explicitly asked
    else if (command_contains(lower_cmd, "search") ||
             command_contains(lower_cmd, "find ") ||
             command_contains(lower_cmd, "look for") ||
             command_contains(lower_cmd, "show me") ||
             command_contains(lower_cmd, "tell me about") ||
             command_contains(lower_cmd, "what is") ||
             command_contains(lower_cmd, "who is") ||
             command_contains(lower_cmd, "how to") ||
             command_contains(lower_cmd, "how do i")) {
        const char* query = extract_search_query(command);
        char* search_result = general_search(query);
        
        if (search_result) {
            strncpy(response, search_result, (size_t)response_size - 1);
            response[response_size - 1] = '\0';
            free(search_result);
        } else {
            strcpy(response, "Search query processed. Please try a different search term.");
        }
    }
    // Reset AI Memory
    else if (command_contains(lower_cmd, "reset ai") || 
             command_contains(lower_cmd, "clear memory") ||
             command_contains(lower_cmd, "forget everything")) {
        if (remove("src/chat_history.json") == 0) {
            strcpy(response, "AI memory has been wiped. I'm ready for a fresh start.");
        } else {
            strcpy(response, "My memory is already clean.");
        }
    }
    // Change Personality
    else if (command_contains(lower_cmd, "set mode") || 
             command_contains(lower_cmd, "change personality") ||
             command_contains(lower_cmd, "be sarcastic") ||
             command_contains(lower_cmd, "be pirate") ||
             command_contains(lower_cmd, "be polite")) {
        
        const char* mode = "default";
        if (command_contains(lower_cmd, "sarcastic")) mode = "sarcastic";
        else if (command_contains(lower_cmd, "pirate")) mode = "pirate";
        else if (command_contains(lower_cmd, "formal") || command_contains(lower_cmd, "polite")) mode = "formal";
        
        FILE* f = fopen("src/persona_mode.txt", "w");
        if (f) {
            fprintf(f, "%s", mode);
            fclose(f);
            snprintf(response, response_size, "I have switched to %s mode.", mode);
        } else {
            strcpy(response, "I couldn't switch my personality settings.");
        }
    }
    // AI Integration
    else if (command_contains(lower_cmd, "ask ai") || 
             command_contains(lower_cmd, "explain") ||
             command_contains(lower_cmd, "write") ||
             command_contains(lower_cmd, "generate")) {
        
        printf("[JARVIS] Thinking...\n");
        char sys_cmd[1024];
        snprintf(sys_cmd, sizeof(sys_cmd), "python3 src/ai_chat.py \"%s\"", command);
        
        FILE* fp = popen(sys_cmd, "r");
        if (fp == NULL) {
            strcpy(response, "I cannot access my AI brain right now.");
        } else {
            if (fgets(response, response_size, fp) == NULL) {
                strcpy(response, "I couldn't generate a response.");
            }
            // Remove trailing newline
            size_t len = strlen(response);
            if (len > 0 && response[len-1] == '\n') {
                response[len-1] = '\0';
            }
            pclose(fp);
        }
    }
    // Default response - ask for clarification instead of searching
    else {
        snprintf(response, response_size, "I didn't understand '%s'. Try: build project, run tests, "
                 "check warnings, find function <name>, daily status, search for <topic>, or quit.", command);
    }

    free(lower_cmd);
    return response;
}

/**
 * Converts command string to lowercase for case-insensitive matching
 */
char* to_lowercase(const char* str) {
    if (!str) return NULL;
    
    char* lower = (char*)malloc(strlen(str) + 1);
    if (!lower) return NULL;
    
    for (int i = 0; str[i]; i++) {
        lower[i] = tolower((unsigned char)str[i]);
    }
    lower[strlen(str)] = '\0';
    
    return lower;
}

/**
 * Checks if a command contains a specific keyword
 */
int command_contains(const char* command, const char* keyword) {
    if (!command || !keyword) return 0;
    
    return strstr(command, keyword) != NULL ? 1 : 0;
}

/**
 * Opens applications based on voice commands
 * Examples: "open chrome", "open safari", "open terminal"
 */
void execute_open_command(const char* command, char* response, int response_size) {
    if (!command) {
        snprintf(response, response_size, "I need an application name.");
        return;
    }

    char* lower_cmd = to_lowercase(command);
    char macos_app[256] = {0};

    if (!lower_cmd) {
        snprintf(response, response_size, "I couldn't process the application name.");
        return;
    }

    // Extract application name
    if (strstr(lower_cmd, "chrome") || strstr(lower_cmd, "google")) {
        strcpy(macos_app, "Google Chrome");
    }
    else if (strstr(lower_cmd, "safari")) {
        strcpy(macos_app, "Safari");
    }
    else if (strstr(lower_cmd, "firefox")) {
        strcpy(macos_app, "Firefox");
    }
    else if (strstr(lower_cmd, "terminal")) {
        strcpy(macos_app, "Terminal");
    }
    else if (strstr(lower_cmd, "finder") || strstr(lower_cmd, "files")) {
        strcpy(macos_app, "Finder");
    }
    else if (strstr(lower_cmd, "spotify") || strstr(lower_cmd, "music")) {
        strcpy(macos_app, "Spotify");
    }
    else if (strstr(lower_cmd, "vscode") || strstr(lower_cmd, "code") || strstr(lower_cmd, "visual studio")) {
        strcpy(macos_app, "Visual Studio Code");
    }
    else if (strstr(lower_cmd, "xcode")) {
        strcpy(macos_app, "Xcode");
    }
    else if (strstr(lower_cmd, "docker")) {
        strcpy(macos_app, "Docker");
    }
    else if (strstr(lower_cmd, "postman")) {
        strcpy(macos_app, "Postman");
    }
    else {
        snprintf(response, response_size, "Which application would you like to open? "
                 "Try: Chrome, Safari, Firefox, Terminal, Finder, Spotify, or VS Code.");
        free(lower_cmd);
        return;
    }
    
    // Execute the open command on macOS
    char open_cmd[512];
    snprintf(open_cmd, sizeof(open_cmd), "open -a \"%s\" &", macos_app);
    
    int result = system(open_cmd);
    
    if (result == 0) {
        snprintf(response, response_size, "Opening %s for you.", macos_app);
    } else {
        snprintf(response, response_size, "I tried to open %s, but encountered an issue. "
                 "Please check if %s is installed.", macos_app, macos_app);
    }

    free(lower_cmd);
}

/**
 * Opens YouTube and searches for a topic
 */
void execute_youtube_command(const char* command, char* response, int response_size) {
    char search_term[512] = {0};
    char url[1024] = {0};
    
    // Extract search term from command
    const char* start = NULL;
    if ((start = strstr(command, "search")) || (start = strstr(command, "find")) || 
        (start = strstr(command, "watch")) || (start = strstr(command, "youtube"))) {
        
        // Skip the keyword
        while (*start && !isspace(*start)) start++;
        while (*start && isspace(*start)) start++;
        
        // Get remaining text
        strncpy(search_term, start, sizeof(search_term) - 1);
        search_term[sizeof(search_term) - 1] = '\0';
        
        if (strlen(search_term) > 0) {
            // Remove trailing punctuation
            int len = strlen(search_term) - 1;
            while (len >= 0 && (search_term[len] == '.' || search_term[len] == '?' || search_term[len] == '!')) {
                search_term[len] = '\0';
                len--;
            }
            
            // Build YouTube search URL (replace spaces with +)
            snprintf(url, sizeof(url), "open 'https://www.youtube.com/results?search_query=");
            for (int i = 0; search_term[i]; i++) {
                if (search_term[i] == ' ') {
                    strcat(url, "+");
                } else {
                    char ch[2] = {search_term[i], '\0'};
                    strcat(url, ch);
                }
            }
            strcat(url, "' &");
            
            system(url);
            snprintf(response, response_size, "Searching YouTube for %s. Opening in your browser.", search_term);
        } else {
            system("open 'https://www.youtube.com' &");
            snprintf(response, response_size, "Opening YouTube for you.");
        }
    } else {
        system("open 'https://www.youtube.com' &");
        snprintf(response, response_size, "Opening YouTube.");
    }
}

/**
 * Opens web pages and searches
 */
void execute_webpage_command(const char* command, char* response, int response_size) {
    char search_term[512] = {0};
    char url[1024] = {0};
    
    // Extract search term from command
    const char* start = NULL;
    if ((start = strstr(command, "visit")) || (start = strstr(command, "go to")) || 
        (start = strstr(command, "open website"))) {
        
        // Skip the keyword
        while (*start && !isspace(*start)) start++;
        while (*start && isspace(*start)) start++;
        
        // Get remaining text
        strncpy(search_term, start, sizeof(search_term) - 1);
        search_term[sizeof(search_term) - 1] = '\0';
        
        if (strlen(search_term) > 0) {
            // Remove trailing punctuation
            int len = strlen(search_term) - 1;
            while (len >= 0 && (search_term[len] == '.' || search_term[len] == '?' || search_term[len] == '!')) {
                search_term[len] = '\0';
                len--;
            }
            
            // Build Google search URL
            snprintf(url, sizeof(url), "open 'https://www.google.com/search?q=");
            for (int i = 0; search_term[i]; i++) {
                if (search_term[i] == ' ') {
                    strcat(url, "+");
                } else {
                    char ch[2] = {search_term[i], '\0'};
                    strcat(url, ch);
                }
            }
            strcat(url, "' &");
            
            system(url);
            snprintf(response, response_size, "Searching the web for %s. Opening in your browser.", search_term);
        } else {
            system("open 'https://www.google.com' &");
            snprintf(response, response_size, "Opening Google for you.");
        }
    } else {
        system("open 'https://www.google.com' &");
        snprintf(response, response_size, "Opening web browser.");
    }
}

static int run_command_capture(const char* shell_cmd, char* response, int response_size, int max_lines) {
    if (!shell_cmd || !response || response_size <= 0) {
        return -1;
    }

    FILE* fp = popen(shell_cmd, "r");
    if (!fp) {
        snprintf(response, response_size, "Failed to execute command.");
        return -1;
    }

    response[0] = '\0';
    char buffer[256];
    int lines = 0;

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        if (max_lines > 0 && lines >= max_lines) {
            break;
        }

        buffer[strcspn(buffer, "\n")] = '\0';
        if (strlen(buffer) == 0) {
            continue;
        }

        if (strlen(response) + strlen(buffer) + 2 >= (size_t)response_size) {
            break;
        }

        strcat(response, buffer);
        strcat(response, "\n");
        lines++;
    }

    int status = pclose(fp);
    size_t len = strlen(response);
    if (len > 0 && response[len - 1] == '\n') {
        response[len - 1] = '\0';
    }

    if (strlen(response) == 0) {
        if (status == 0) {
            snprintf(response, response_size, "Command finished with no output.");
        } else {
            snprintf(response, response_size, "Command finished, but no readable output was captured.");
        }
    }

    return status;
}

static int extract_identifier_after_keyword(const char* command, const char* keyword, char* out, size_t out_size) {
    if (!command || !keyword || !out || out_size == 0) {
        return 0;
    }

    const char* start = strstr(command, keyword);
    if (!start) {
        return 0;
    }

    start += strlen(keyword);
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }

    if (strncmp(start, "named ", 6) == 0) {
        start += 6;
    }
    if (strncmp(start, "for ", 4) == 0) {
        start += 4;
    }

    while (*start && isspace((unsigned char)*start)) {
        start++;
    }

    if (!(*start == '_' || isalpha((unsigned char)*start))) {
        return 0;
    }

    size_t i = 0;
    while ((*start == '_' || isalnum((unsigned char)*start)) && i < out_size - 1) {
        out[i++] = *start++;
    }
    out[i] = '\0';

    return i > 0 ? 1 : 0;
}

static int update_makefile_for_module(const char* module_name, char* error_message, int error_message_size) {
    if (!module_name || strlen(module_name) == 0) {
        snprintf(error_message, error_message_size, "Missing module name for Makefile update.");
        return 0;
    }

    const char* makefile_path = "Makefile";
    const char* temp_path = "Makefile.jarvis.tmp";
    char source_token[256];
    char object_token[256];
    snprintf(source_token, sizeof(source_token), "$(SRC_DIR)/%s.c", module_name);
    snprintf(object_token, sizeof(object_token), "$(BUILD_DIR)/%s.o", module_name);

    FILE* check = fopen(makefile_path, "r");
    if (!check) {
        snprintf(error_message, error_message_size, "Cannot open Makefile for reading.");
        return 0;
    }

    int source_exists = 0;
    int object_exists = 0;
    char line[4096];
    while (fgets(line, sizeof(line), check) != NULL) {
        if (strstr(line, source_token) != NULL) {
            source_exists = 1;
        }
        if (strstr(line, object_token) != NULL) {
            object_exists = 1;
        }
    }
    fclose(check);

    if (source_exists && object_exists) {
        return 1;
    }

    FILE* in = fopen(makefile_path, "r");
    FILE* out = fopen(temp_path, "w");
    if (!in || !out) {
        if (in) fclose(in);
        if (out) fclose(out);
        remove(temp_path);
        snprintf(error_message, error_message_size, "Cannot prepare temporary Makefile update.");
        return 0;
    }

    while (fgets(line, sizeof(line), in) != NULL) {
        if (!source_exists && strncmp(line, "SOURCES =", 9) == 0) {
            line[strcspn(line, "\n")] = '\0';
            fprintf(out, "%s %s\n", line, source_token);
        } else if (!object_exists && strncmp(line, "OBJECTS =", 9) == 0) {
            line[strcspn(line, "\n")] = '\0';
            fprintf(out, "%s %s\n", line, object_token);
        } else {
            fputs(line, out);
        }
    }

    fclose(in);
    fclose(out);

    if (rename(temp_path, makefile_path) != 0) {
        remove(temp_path);
        snprintf(error_message, error_message_size, "Failed to update Makefile: %s", strerror(errno));
        return 0;
    }

    return 1;
}

static int create_c_module_scaffold(const char* module_name, char* response, int response_size) {
    if (!module_name || strlen(module_name) == 0) {
        snprintf(response, response_size, "Please provide a module name.");
        return 0;
    }

    char header_path[256];
    char source_path[256];
    snprintf(header_path, sizeof(header_path), "include/%s.h", module_name);
    snprintf(source_path, sizeof(source_path), "src/%s.c", module_name);

    if (access(header_path, F_OK) == 0 || access(source_path, F_OK) == 0) {
        snprintf(response, response_size, "Module '%s' already exists.", module_name);
        return 0;
    }

    char guard[128] = {0};
    size_t guard_len = strlen(module_name);
    if (guard_len + 3 >= sizeof(guard)) {
        snprintf(response, response_size, "Module name is too long.");
        return 0;
    }

    for (size_t i = 0; i < guard_len; i++) {
        guard[i] = (char)toupper((unsigned char)module_name[i]);
    }
    strcat(guard, "_H");

    FILE* header = fopen(header_path, "w");
    if (!header) {
        snprintf(response, response_size, "Failed to create %s.", header_path);
        return 0;
    }
    fprintf(header, "#ifndef %s\n#define %s\n\nvoid %s_init(void);\n\n#endif // %s\n",
            guard, guard, module_name, guard);
    fclose(header);

    FILE* source = fopen(source_path, "w");
    if (!source) {
        remove(header_path);
        snprintf(response, response_size, "Failed to create %s.", source_path);
        return 0;
    }
    fprintf(source, "#include \"../include/%s.h\"\n\nvoid %s_init(void) {\n    // TODO: implement module logic\n}\n",
            module_name, module_name);
    fclose(source);

    char makefile_error[256] = {0};
    if (update_makefile_for_module(module_name, makefile_error, sizeof(makefile_error))) {
        snprintf(response, response_size,
                 "Created module '%s' (%s, %s) and updated Makefile.",
                 module_name, header_path, source_path);
    } else {
        snprintf(response, response_size,
                 "Created module '%s' (%s, %s), but Makefile update failed: %s",
                 module_name, header_path, source_path,
                 strlen(makefile_error) > 0 ? makefile_error : "unknown error");
    }
    return 1;
}

static void execute_daily_workflow_command(const char* command, char* response, int response_size) {
    if (strstr(command, "git pull")) {
        run_command_capture("git pull 2>&1", response, response_size, 12);
        return;
    }

    if (strstr(command, "git push")) {
        run_command_capture("git push 2>&1", response, response_size, 12);
        return;
    }

    if (strstr(command, "git status") || strstr(command, "review changes")) {
        run_command_capture("git status --short -b 2>&1", response, response_size, 12);
        return;
    }

    run_command_capture(
        "git rev-parse --is-inside-work-tree >/dev/null 2>&1 && "
        "{ echo \"Branch:\"; git branch --show-current; "
        "echo \"Changes:\"; git status --short | head -8; "
        "echo \"Last commit:\"; git log -1 --oneline; } || "
        "echo \"Not a git repository.\"",
        response, response_size, 20
    );
}

static void execute_c_workflow_command(const char* command, char* response, int response_size) {
    if (strstr(command, "create c module") || strstr(command, "scaffold module")) {
        char module_name[128] = {0};
        int parsed = extract_identifier_after_keyword(command, "create c module", module_name, sizeof(module_name));
        if (!parsed) {
            parsed = extract_identifier_after_keyword(command, "scaffold module", module_name, sizeof(module_name));
        }
        if (!parsed) {
            snprintf(response, response_size, "Please say: create c module <module_name>.");
            return;
        }

        create_c_module_scaffold(module_name, response, response_size);
        return;
    }

    if (strstr(command, "rebuild project") || strstr(command, "clean build")) {
        run_command_capture("make rebuild 2>&1", response, response_size, 12);
        return;
    }

    if (strstr(command, "run tests") || strstr(command, "test project") || strstr(command, "make test")) {
        run_command_capture("make test 2>&1", response, response_size, 12);
        if (strstr(response, "No rule to make target") != NULL) {
            snprintf(response, response_size, "No test target found. Add a 'test' target to the Makefile.");
        }
        return;
    }

    if (strstr(command, "check warnings") || strstr(command, "show warnings")) {
        char warning_output[1024] = {0};
        run_command_capture("(make clean && make 2>&1) | grep -i 'warning:' || true",
                            warning_output, sizeof(warning_output), 10);

        if (strcmp(warning_output, "Command finished with no output.") == 0) {
            snprintf(response, response_size, "Build completed with no compiler warnings.");
        } else {
            snprintf(response, response_size, "Compiler warnings:\n%s", warning_output);
        }
        return;
    }

    run_command_capture("make 2>&1", response, response_size, 12);
}

static void execute_code_navigation_command(const char* command, char* response, int response_size) {
    if (strstr(command, "todo") || strstr(command, "fixme")) {
        run_command_capture("rg -n \"TODO|FIXME\" src include 2>/dev/null | head -8",
                            response, response_size, 10);
        if (strcmp(response, "Command finished with no output.") == 0) {
            snprintf(response, response_size, "No TODO/FIXME items found in src/ or include/.");
        }
        return;
    }

    char symbol[128] = {0};
    int parsed = extract_identifier_after_keyword(command, "find symbol", symbol, sizeof(symbol));
    if (!parsed) {
        parsed = extract_identifier_after_keyword(command, "find function", symbol, sizeof(symbol));
    }
    if (!parsed) {
        parsed = extract_identifier_after_keyword(command, "where is function", symbol, sizeof(symbol));
    }
    if (!parsed) {
        parsed = extract_identifier_after_keyword(command, "search code", symbol, sizeof(symbol));
    }

    if (!parsed) {
        snprintf(response, response_size, "Please specify a symbol. Example: find function process_command.");
        return;
    }

    char search_cmd[512];
    snprintf(search_cmd, sizeof(search_cmd),
             "rg -n \"\\b%s\\b\" src include 2>/dev/null | head -8", symbol);
    run_command_capture(search_cmd, response, response_size, 10);

    if (strcmp(response, "Command finished with no output.") == 0) {
        snprintf(response, response_size, "No matches found for '%s' in src/ or include/.", symbol);
    }
}

/**
 * Executes developer workflow commands (Git, Build)
 */
void execute_dev_command(const char* command, char* response, int response_size) {
    if (strstr(command, "status")) {
        run_command_capture("git status 2>&1", response, response_size, 12);
    }
    else if (strstr(command, "pull")) {
        run_command_capture("git pull 2>&1", response, response_size, 12);
    }
    else if (strstr(command, "push")) {
        run_command_capture("git push 2>&1", response, response_size, 12);
    }
    else if (strstr(command, "build") || strstr(command, "make") || strstr(command, "compile")) {
        run_command_capture("make 2>&1", response, response_size, 12);
    }
    else {
        snprintf(response, response_size, "I can help with git status, pull, push, build project, and run tests.");
    }
}

/**
 * Performs developer-specific searches on Stack Overflow or GitHub
 */
void execute_dev_search(const char* command, char* response, int response_size) {
    char search_term[512] = {0};
    char url[1024] = {0};
    const char* base_url = "https://stackoverflow.com/search?q=";
    const char* platform = "Stack Overflow";

    if (strstr(command, "github")) {
        base_url = "https://github.com/search?q=";
        platform = "GitHub";
    }

    // Extract search term
    const char* start = NULL;
    if ((start = strstr(command, "search")) || (start = strstr(command, "for")) ||
        (start = strstr(command, "github")) || (start = strstr(command, "overflow"))) {

        // Skip the keyword
        while (*start && !isspace((unsigned char)*start)) start++;
        while (*start && isspace((unsigned char)*start)) start++;

        strncpy(search_term, start, sizeof(search_term) - 1);
        search_term[sizeof(search_term) - 1] = '\0';

        if (strlen(search_term) > 0) {
            snprintf(url, sizeof(url), "open '%s", base_url);
            for (int i = 0; search_term[i]; i++) {
                if (search_term[i] == ' ') {
                    strcat(url, "+");
                } else {
                    char ch[2] = {search_term[i], '\0'};
                    strcat(url, ch);
                }
            }
            strcat(url, "' &");

            system(url);
            snprintf(response, response_size, "Searching %s for %s.", platform, search_term);
        } else {
            snprintf(response, response_size, "What should I search for on %s?", platform);
        }
    } else {
        snprintf(response, response_size, "Opening developer search.");
    }
}

/**
 * Manages project directories and files
 */
void execute_project_management(const char* command, char* response, int response_size) {
    if (strstr(command, "where am i") || strstr(command, "current directory")) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            snprintf(response, response_size, "Current directory: %s", cwd);
        } else {
            snprintf(response, response_size, "Could not determine current directory.");
        }
    }
    else if (strstr(command, "list files") || strstr(command, "ls")) {
        FILE* fp = popen("ls -F", "r");
        if (fp) {
            char buffer[128];
            response[0] = '\0';
            strcat(response, "Files: ");
            int count = 0;
            while (fgets(buffer, sizeof(buffer), fp) != NULL && count < 6) {
                buffer[strcspn(buffer, "\n")] = '\0';
                if (strlen(response) + strlen(buffer) + 2 < (size_t)response_size) {
                    strcat(response, buffer);
                    strcat(response, ", ");
                }
                count++;
            }
            pclose(fp);

            size_t len = strlen(response);
            if (len > 2 && response[len - 2] == ',') {
                response[len - 2] = '\0';
            }
            if (count == 0) {
                strcat(response, "No files found.");
            }
        } else {
            snprintf(response, response_size, "Failed to list files.");
        }
    }
    else if (strstr(command, "change directory") || strstr(command, "go to folder")) {
        const char* target = NULL;
        if (strstr(command, "go to folder")) {
            target = strstr(command, "folder ");
            if (target) target += 7;
        } else {
            target = strstr(command, "to ");
            if (target) target += 3;
        }

        if (target) {
            while (*target && isspace((unsigned char)*target)) target++;

            char dir[256] = {0};
            strncpy(dir, target, sizeof(dir) - 1);
            size_t len = strlen(dir);
            while (len > 0 && (dir[len - 1] == '.' || dir[len - 1] == '!' || dir[len - 1] == '?')) {
                dir[len - 1] = '\0';
                len--;
            }

            if (chdir(dir) == 0) {
                snprintf(response, response_size, "Changed directory to %s.", dir);
            } else {
                snprintf(response, response_size, "Could not find directory %s.", dir);
            }
        } else {
            snprintf(response, response_size, "Please specify a directory.");
        }
    }
    else if (strstr(command, "create file")) {
        const char* start = strstr(command, "file ");
        if (start) {
            start += 5;
            while (*start && isspace((unsigned char)*start)) start++;

            char file_path[256] = {0};
            size_t i = 0;
            while (start[i] && !isspace((unsigned char)start[i]) && i < sizeof(file_path) - 1) {
                char ch = start[i];
                if (ch == ';' || ch == '&' || ch == '|' || ch == '>' || ch == '<' || ch == '$') {
                    snprintf(response, response_size, "File name contains invalid characters.");
                    return;
                }
                file_path[i] = ch;
                i++;
            }
            file_path[i] = '\0';

            if (strlen(file_path) == 0) {
                snprintf(response, response_size, "Please provide a file name.");
                return;
            }

            FILE* file = fopen(file_path, "a");
            if (!file) {
                snprintf(response, response_size, "Failed to create file %s.", file_path);
                return;
            }
            fclose(file);
            snprintf(response, response_size, "Created file %s.", file_path);
        } else {
            snprintf(response, response_size, "Please specify a file name.");
        }
    }
}
