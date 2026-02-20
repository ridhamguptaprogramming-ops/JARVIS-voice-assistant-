#include "../include/command_processor.h"
#include "../include/search.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

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
static void execute_ai_project_setup_command(const char* command, char* response, int response_size);
static int contains_word(const char* text, const char* word);
static void detect_project_language(const char* command, char* language, size_t language_size);
static int extract_project_name_from_command(const char* command, char* project_name, size_t project_name_size);
static int write_file_content(const char* file_path, const char* content);
static int create_project_files(const char* project_name, const char* language, char* entry_file, size_t entry_file_size,
                                char* error_message, int error_message_size);
static int open_project_in_vscode(const char* project_name, const char* entry_file);
static int open_file_in_vscode(const char* file_path);
static int save_last_project_name(const char* project_name);
static int load_last_project_name(char* project_name, size_t project_name_size);
static int is_project_creation_request(const char* command);
static void execute_open_project_command(const char* command, char* response, int response_size);
static void execute_open_last_project_command(char* response, int response_size);
static void sanitize_prompt_for_shell(const char* input, char* output, size_t output_size);
static void run_ai_mode_command(const char* command, const char* mode, char* response, int response_size);
static int sanitize_relative_path(const char* input, char* output, size_t output_size);
static int create_directory_recursive(const char* path);
static const char* starter_template_for_path(const char* file_path);
static int create_file_with_optional_template(const char* file_path, int use_template, char* response, int response_size);
static int extract_file_path_for_codegen(const char* command, char* file_path, size_t file_path_size);
static int extract_codegen_prompt(const char* command, char* prompt, size_t prompt_size);
static void strip_markdown_code_fences(char* text);
static void execute_ai_code_file_command(const char* command, char* response, int response_size);
static void launch_jarvis_ui_command(char* response, int response_size);
static int is_ai_brain_request(const char* command);
static void execute_ai_brain_command(const char* command, char* response, int response_size);

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
                        "find symbol <name>, create c module <name>, create project <name> in python, "
                        "open project <name> in VS Code, open last project, create folder <name>, "
                        "open file <path>, create code file <name>, git status, AI summary, AI ideas, "
                        "AI plan mode, AI task engine (website, app, legal, problem solving), and web search.");
    }
    // System info command
    else if (command_contains(lower_cmd, "system info") ||
             command_contains(lower_cmd, "system status") ||
             command_contains(lower_cmd, "system information") ||
             strcmp(lower_cmd, "info") == 0) {
        snprintf(response, response_size, "System information requested. JARVIS version 2.0.0 with voice control is running smoothly on macOS.");
    }
    // AI-like project setup (create project and open in VS Code)
    else if (is_project_creation_request(lower_cmd)) {
        execute_ai_project_setup_command(lower_cmd, response, response_size);
    }
    // Open an existing project in VS Code
    else if (command_contains(lower_cmd, "open project")) {
        execute_open_project_command(lower_cmd, response, response_size);
    }
    // Open last created project in VS Code
    else if (command_contains(lower_cmd, "open last project") ||
             command_contains(lower_cmd, "open recent project")) {
        execute_open_last_project_command(response, response_size);
    }
    // Open file in VS Code
    else if (command_contains(lower_cmd, "open file")) {
        execute_project_management(lower_cmd, response, response_size);
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
    // Open JARVIS AI UI
    else if ((command_contains(lower_cmd, "open") || command_contains(lower_cmd, "launch")) &&
             (command_contains(lower_cmd, "jarvis ui") ||
              command_contains(lower_cmd, "ai ui") ||
              command_contains(lower_cmd, "ai window"))) {
        launch_jarvis_ui_command(response, response_size);
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
    // AI Brain task orchestration
    else if (is_ai_brain_request(lower_cmd)) {
        execute_ai_brain_command(command, response, response_size);
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
    // AI code generation directly into file
    else if (command_contains(lower_cmd, "generate code file") ||
             command_contains(lower_cmd, "create code file") ||
             command_contains(lower_cmd, "write code file")) {
        execute_ai_code_file_command(lower_cmd, response, response_size);
    }
    // Project Navigation & Management
    else if (command_contains(lower_cmd, "change directory") ||
             command_contains(lower_cmd, "go to folder") ||
             command_contains(lower_cmd, "create folder") ||
             command_contains(lower_cmd, "new folder") ||
             command_contains(lower_cmd, "where am i") ||
             command_contains(lower_cmd, "list files") ||
             command_contains(lower_cmd, "create file") ||
             command_contains(lower_cmd, "new file") ||
             command_contains(lower_cmd, "open file")) {
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
             command_contains(lower_cmd, "be polite") ||
             command_contains(lower_cmd, "developer mode") ||
             command_contains(lower_cmd, "automation mode") ||
             command_contains(lower_cmd, "ceo mode") ||
             command_contains(lower_cmd, "research mode") ||
             command_contains(lower_cmd, "security mode")) {
        
        const char* mode = "default";
        if (command_contains(lower_cmd, "sarcastic")) mode = "sarcastic";
        else if (command_contains(lower_cmd, "pirate")) mode = "pirate";
        else if (command_contains(lower_cmd, "formal") || command_contains(lower_cmd, "polite")) mode = "formal";
        else if (command_contains(lower_cmd, "developer")) mode = "developer";
        else if (command_contains(lower_cmd, "automation")) mode = "automation";
        else if (command_contains(lower_cmd, "ceo")) mode = "ceo";
        else if (command_contains(lower_cmd, "research")) mode = "research";
        else if (command_contains(lower_cmd, "security")) mode = "security";
        
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
             command_contains(lower_cmd, "generate") ||
             command_contains(lower_cmd, "summarize") ||
             command_contains(lower_cmd, "summary") ||
             command_contains(lower_cmd, "brainstorm") ||
             command_contains(lower_cmd, "ideas") ||
             command_contains(lower_cmd, "plan") ||
             command_contains(lower_cmd, "roadmap")) {
        const char* ai_mode = "chat";
        if (command_contains(lower_cmd, "summarize") || command_contains(lower_cmd, "summary")) {
            ai_mode = "summary";
        } else if (command_contains(lower_cmd, "brainstorm") || command_contains(lower_cmd, "ideas")) {
            ai_mode = "ideas";
        } else if (command_contains(lower_cmd, "plan") || command_contains(lower_cmd, "roadmap")) {
            ai_mode = "plan";
        }
        run_ai_mode_command(command, ai_mode, response, response_size);
    }
    // Default response - ask for clarification instead of searching
    else {
        snprintf(response, response_size, "I didn't understand '%s'. Try: build project, run tests, "
                "check warnings, find function <name>, create project <name>, open project <name>, "
                "open last project, create folder <name>, open file <path>, create file <name>, "
                "generate code file <name> for <task>, make website <idea>, build app <idea>, "
                "draft court/legal document <topic>, solve problem <topic>, search for <topic>, or quit.", command);
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

static int is_project_creation_request(const char* command) {
    if (!command) {
        return 0;
    }

    if (strstr(command, "build project") || strstr(command, "rebuild project") ||
        strstr(command, "compile project") || strstr(command, "test project")) {
        return 0;
    }

    if (strstr(command, "create project") || strstr(command, "new project") ||
        strstr(command, "start project") || strstr(command, "project named") ||
        strstr(command, "project called") || strstr(command, "make project") ||
        strstr(command, "make a project") || strstr(command, "make an project")) {
        return 1;
    }

    if (strstr(command, "project") && strstr(command, "make ")) {
        return 1;
    }

    return 0;
}

static int sanitize_relative_path(const char* input, char* output, size_t output_size) {
    if (!input || !output || output_size == 0) {
        return 0;
    }

    while (*input && isspace((unsigned char)*input)) {
        input++;
    }

    if (*input == '\0' || *input == '/' || *input == '~') {
        return 0;
    }

    size_t out_len = 0;
    for (size_t i = 0; input[i] != '\0' && out_len < output_size - 1; i++) {
        char ch = input[i];
        if (isspace((unsigned char)ch)) {
            break;
        }
        if (!(isalnum((unsigned char)ch) || ch == '_' || ch == '-' || ch == '.' || ch == '/')) {
            return 0;
        }
        output[out_len++] = ch;
    }

    while (out_len > 0 &&
           (output[out_len - 1] == '.' || output[out_len - 1] == '!' || output[out_len - 1] == '?' || output[out_len - 1] == ',')) {
        out_len--;
    }
    output[out_len] = '\0';

    if (out_len == 0 || strstr(output, "..") != NULL || strstr(output, "//") != NULL) {
        return 0;
    }

    return 1;
}

static int create_directory_recursive(const char* path) {
    if (!path || strlen(path) == 0) {
        return 0;
    }

    char temp[512];
    snprintf(temp, sizeof(temp), "%s", path);

    size_t len = strlen(temp);
    if (len == 0 || len >= sizeof(temp)) {
        return 0;
    }

    if (temp[len - 1] == '/') {
        temp[len - 1] = '\0';
    }

    for (char* p = temp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            if (mkdir(temp, 0755) != 0 && errno != EEXIST) {
                return 0;
            }
            *p = '/';
        }
    }

    if (mkdir(temp, 0755) != 0 && errno != EEXIST) {
        return 0;
    }

    return 1;
}

static const char* starter_template_for_path(const char* file_path) {
    if (!file_path) {
        return NULL;
    }

    const char* ext = strrchr(file_path, '.');
    if (!ext) {
        return NULL;
    }

    if (strcmp(ext, ".py") == 0) {
        return "def main() -> None:\n    print(\"Hello from JARVIS\")\n\n\nif __name__ == \"__main__\":\n    main()\n";
    }
    if (strcmp(ext, ".js") == 0) {
        return "function main() {\n  console.log(\"Hello from JARVIS\");\n}\n\nmain();\n";
    }
    if (strcmp(ext, ".ts") == 0) {
        return "function main(): void {\n  console.log(\"Hello from JARVIS\");\n}\n\nmain();\n";
    }
    if (strcmp(ext, ".c") == 0) {
        return "#include <stdio.h>\n\nint main(void) {\n    printf(\"Hello from JARVIS!\\n\");\n    return 0;\n}\n";
    }
    if (strcmp(ext, ".h") == 0) {
        return "#pragma once\n\n";
    }
    if (strcmp(ext, ".html") == 0) {
        return "<!doctype html>\n<html>\n  <head>\n    <meta charset=\"utf-8\" />\n    <title>JARVIS App</title>\n  </head>\n  <body>\n    <h1>Hello from JARVIS</h1>\n  </body>\n</html>\n";
    }
    if (strcmp(ext, ".css") == 0) {
        return "body {\n  margin: 0;\n  font-family: sans-serif;\n}\n";
    }
    if (strcmp(ext, ".md") == 0) {
        return "# New Notes\n\nCreated by JARVIS.\n";
    }
    if (strcmp(ext, ".json") == 0) {
        return "{\n  \"name\": \"jarvis-project\"\n}\n";
    }

    return NULL;
}

static int create_file_with_optional_template(const char* file_path, int use_template, char* response, int response_size) {
    if (!file_path || !response || response_size <= 0) {
        return 0;
    }

    if (access(file_path, F_OK) == 0) {
        snprintf(response, response_size, "File already exists: %s", file_path);
        return 0;
    }

    const char* slash = strrchr(file_path, '/');
    if (slash) {
        char parent_dir[512] = {0};
        size_t parent_len = (size_t)(slash - file_path);
        if (parent_len >= sizeof(parent_dir)) {
            snprintf(response, response_size, "Path is too long: %s", file_path);
            return 0;
        }
        strncpy(parent_dir, file_path, parent_len);
        parent_dir[parent_len] = '\0';
        if (strlen(parent_dir) > 0 && !create_directory_recursive(parent_dir)) {
            snprintf(response, response_size, "Failed to create parent folder %s.", parent_dir);
            return 0;
        }
    }

    FILE* file = fopen(file_path, "w");
    if (!file) {
        snprintf(response, response_size, "Failed to create file %s.", file_path);
        return 0;
    }

    if (use_template) {
        const char* template_content = starter_template_for_path(file_path);
        if (template_content) {
            fputs(template_content, file);
            fclose(file);
            snprintf(response, response_size, "Created %s with starter code.", file_path);
            return 1;
        }
    }

    fclose(file);
    snprintf(response, response_size, "Created file %s.", file_path);
    return 1;
}

static void execute_open_project_command(const char* command, char* response, int response_size) {
    if (!command || !response || response_size <= 0) {
        return;
    }

    const char* start = strstr(command, "open project");
    if (!start) {
        snprintf(response, response_size, "Please specify a project to open.");
        return;
    }

    start += strlen("open project");
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }
    if (strncmp(start, "named ", 6) == 0) {
        start += 6;
    } else if (strncmp(start, "called ", 7) == 0) {
        start += 7;
    }

    char project_name[256] = {0};
    if (!sanitize_relative_path(start, project_name, sizeof(project_name))) {
        snprintf(response, response_size, "Please say: open project <project_name>.");
        return;
    }

    struct stat st;
    if (stat(project_name, &st) != 0 || !S_ISDIR(st.st_mode)) {
        snprintf(response, response_size, "I couldn't find project folder '%s'.", project_name);
        return;
    }

    int opened = open_project_in_vscode(project_name, "");
    if (opened) {
        snprintf(response, response_size, "Opening project '%s' in VS Code.", project_name);
    } else {
        snprintf(response, response_size, "Project '%s' is ready, but I couldn't open VS Code automatically.", project_name);
    }
}

static void execute_open_last_project_command(char* response, int response_size) {
    if (!response || response_size <= 0) {
        return;
    }

    char project_name[256] = {0};
    if (!load_last_project_name(project_name, sizeof(project_name))) {
        snprintf(response, response_size, "I don't have a recent project yet. Create one first.");
        return;
    }

    struct stat st;
    if (stat(project_name, &st) != 0 || !S_ISDIR(st.st_mode)) {
        snprintf(response, response_size, "Last project '%s' is not available anymore.", project_name);
        return;
    }

    int opened = open_project_in_vscode(project_name, "");
    if (opened) {
        snprintf(response, response_size, "Opening your last project '%s' in VS Code.", project_name);
    } else {
        snprintf(response, response_size, "Found last project '%s', but I couldn't open VS Code automatically.", project_name);
    }
}

static void sanitize_prompt_for_shell(const char* input, char* output, size_t output_size) {
    if (!output || output_size == 0) {
        return;
    }

    if (!input) {
        output[0] = '\0';
        return;
    }

    size_t j = 0;
    for (size_t i = 0; input[i] != '\0' && j < output_size - 1; i++) {
        unsigned char ch = (unsigned char)input[i];
        if (ch == '\n' || ch == '\r') {
            output[j++] = ' ';
            continue;
        }
        if (ch == '"' || ch == '\'' || ch == '`' || ch == '\\' || ch == '$') {
            output[j++] = ' ';
            continue;
        }
        if (iscntrl(ch)) {
            continue;
        }
        output[j++] = (char)ch;
    }
    output[j] = '\0';
}

static void run_ai_mode_command(const char* command, const char* mode, char* response, int response_size) {
    if (!command || !mode || !response || response_size <= 0) {
        return;
    }

    printf("[JARVIS] Thinking...\n");

    char safe_prompt[768];
    sanitize_prompt_for_shell(command, safe_prompt, sizeof(safe_prompt));

    if (strlen(safe_prompt) == 0) {
        snprintf(response, response_size, "Please provide a prompt for AI.");
        return;
    }

    char sys_cmd[1024];
    snprintf(sys_cmd, sizeof(sys_cmd), "python3 src/ai_chat.py --mode %s \"%s\"", mode, safe_prompt);

    FILE* fp = popen(sys_cmd, "r");
    if (fp == NULL) {
        snprintf(response, response_size, "I cannot access my AI brain right now.");
        return;
    }

    response[0] = '\0';
    char line[256];
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strlen(response) + strlen(line) + 1 >= (size_t)response_size) {
            break;
        }
        strcat(response, line);
    }

    if (strlen(response) == 0) {
        snprintf(response, response_size, "I couldn't generate a response.");
    }

    size_t len = strlen(response);
    while (len > 0 && (response[len - 1] == '\n' || response[len - 1] == '\r')) {
        response[len - 1] = '\0';
        len--;
    }

    pclose(fp);
}

static int is_ai_brain_request(const char* command) {
    if (!command) {
        return 0;
    }

    if (strstr(command, "generate code file") ||
        strstr(command, "create code file") ||
        strstr(command, "write code file")) {
        return 0;
    }

    int has_creation_verb = contains_word(command, "create") ||
                            contains_word(command, "make") ||
                            contains_word(command, "build") ||
                            contains_word(command, "generate") ||
                            contains_word(command, "draft") ||
                            contains_word(command, "design");

    int website_task = strstr(command, "website") != NULL ||
                       strstr(command, "web app") != NULL ||
                       strstr(command, "webpage") != NULL ||
                       strstr(command, "landing page") != NULL ||
                       strstr(command, "frontend") != NULL;

    int app_task = contains_word(command, "app") ||
                   contains_word(command, "application") ||
                   contains_word(command, "software");

    int legal_task = strstr(command, "court") != NULL ||
                     strstr(command, "legal") != NULL ||
                     strstr(command, "contract") != NULL ||
                     strstr(command, "agreement") != NULL ||
                     strstr(command, "notice") != NULL ||
                     strstr(command, "case") != NULL;

    int problem_task = strstr(command, "solve problem") != NULL ||
                       strstr(command, "solve this") != NULL ||
                       strstr(command, "problem solving") != NULL ||
                       strstr(command, "root cause") != NULL ||
                       strstr(command, "debug") != NULL ||
                       strstr(command, "fix issue") != NULL;

    if (contains_word(command, "ai") && contains_word(command, "task")) {
        return 1;
    }

    if (legal_task || problem_task) {
        return 1;
    }

    if (has_creation_verb && (website_task || app_task)) {
        return 1;
    }

    return 0;
}

static void execute_ai_brain_command(const char* command, char* response, int response_size) {
    if (!command || !response || response_size <= 0) {
        return;
    }

    printf("[JARVIS] AI brain orchestrator engaged...\n");

    char safe_prompt[768];
    sanitize_prompt_for_shell(command, safe_prompt, sizeof(safe_prompt));

    if (strlen(safe_prompt) == 0) {
        snprintf(response, response_size, "Please provide a task for the AI brain.");
        return;
    }

    char sys_cmd[1200];
    snprintf(sys_cmd, sizeof(sys_cmd), "python3 src/ai_brain.py --request \"%s\"", safe_prompt);

    FILE* fp = popen(sys_cmd, "r");
    if (!fp) {
        snprintf(response, response_size, "I cannot access the AI brain orchestrator right now.");
        return;
    }

    response[0] = '\0';
    char line[256];
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strlen(response) + strlen(line) + 1 >= (size_t)response_size) {
            break;
        }
        strcat(response, line);
    }
    pclose(fp);

    if (strlen(response) == 0) {
        snprintf(response, response_size, "AI brain task execution returned no output.");
        return;
    }

    size_t len = strlen(response);
    while (len > 0 && (response[len - 1] == '\n' || response[len - 1] == '\r')) {
        response[len - 1] = '\0';
        len--;
    }
}

static int extract_file_path_for_codegen(const char* command, char* file_path, size_t file_path_size) {
    if (!command || !file_path || file_path_size == 0) {
        return 0;
    }

    const char* markers[] = {
        "generate code file ",
        "create code file ",
        "write code file "
    };

    const char* start = NULL;
    for (size_t i = 0; i < sizeof(markers) / sizeof(markers[0]); i++) {
        start = strstr(command, markers[i]);
        if (start) {
            start += strlen(markers[i]);
            break;
        }
    }

    if (!start) {
        return 0;
    }

    while (*start && isspace((unsigned char)*start)) {
        start++;
    }

    char raw_path[256] = {0};
    size_t i = 0;
    while (start[i] && !isspace((unsigned char)start[i]) && i < sizeof(raw_path) - 1) {
        raw_path[i] = start[i];
        i++;
    }
    raw_path[i] = '\0';

    return sanitize_relative_path(raw_path, file_path, file_path_size);
}

static int extract_codegen_prompt(const char* command, char* prompt, size_t prompt_size) {
    if (!command || !prompt || prompt_size == 0) {
        return 0;
    }

    const char* marker = strstr(command, " for ");
    if (!marker) {
        marker = strstr(command, " about ");
        if (!marker) {
            return 0;
        }
        marker += strlen(" about ");
    } else {
        marker += strlen(" for ");
    }

    while (*marker && isspace((unsigned char)*marker)) {
        marker++;
    }

    sanitize_prompt_for_shell(marker, prompt, prompt_size);
    return strlen(prompt) > 0;
}

static void strip_markdown_code_fences(char* text) {
    if (!text || strncmp(text, "```", 3) != 0) {
        return;
    }

    char* first_newline = strchr(text, '\n');
    if (!first_newline) {
        return;
    }

    char* content_start = first_newline + 1;
    char* last_fence = NULL;
    char* cursor = strstr(content_start, "```");
    while (cursor) {
        last_fence = cursor;
        cursor = strstr(cursor + 3, "```");
    }

    if (last_fence) {
        *last_fence = '\0';
    }

    memmove(text, content_start, strlen(content_start) + 1);
}

static void execute_ai_code_file_command(const char* command, char* response, int response_size) {
    char file_path[256] = {0};
    if (!extract_file_path_for_codegen(command, file_path, sizeof(file_path))) {
        snprintf(response, response_size, "Please say: generate code file <name> for <task>.");
        return;
    }

    char prompt[768] = {0};
    if (!extract_codegen_prompt(command, prompt, sizeof(prompt))) {
        snprintf(response, response_size, "Please include what code you want. Example: generate code file app.py for flask login api.");
        return;
    }

    char sys_cmd[1200];
    snprintf(sys_cmd, sizeof(sys_cmd), "python3 src/ai_chat.py --mode code \"%s\"", prompt);

    FILE* fp = popen(sys_cmd, "r");
    if (!fp) {
        snprintf(response, response_size, "I cannot access AI code generation right now.");
        return;
    }

    char ai_code[8192];
    ai_code[0] = '\0';
    char line[256];
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strlen(ai_code) + strlen(line) + 1 >= sizeof(ai_code)) {
            break;
        }
        strcat(ai_code, line);
    }
    pclose(fp);

    strip_markdown_code_fences(ai_code);

    size_t code_len = strlen(ai_code);
    while (code_len > 0 && (ai_code[code_len - 1] == '\n' || ai_code[code_len - 1] == '\r')) {
        ai_code[--code_len] = '\0';
    }

    if (code_len == 0) {
        snprintf(response, response_size, "AI did not return code content.");
        return;
    }

    if (strncmp(ai_code, "AI module missing", 17) == 0 ||
        strncmp(ai_code, "I need an API key", 17) == 0 ||
        strncmp(ai_code, "I cannot load an AI model", 24) == 0 ||
        strncmp(ai_code, "I am having trouble", 18) == 0) {
        snprintf(response, response_size, "%s", ai_code);
        return;
    }

    FILE* out = fopen(file_path, "w");
    if (!out) {
        snprintf(response, response_size, "I couldn't write code to %s. Ensure folder exists.", file_path);
        return;
    }
    fputs(ai_code, out);
    fputc('\n', out);
    fclose(out);

    snprintf(response, response_size, "Generated code in %s and saved it.", file_path);
}

static void launch_jarvis_ui_command(char* response, int response_size) {
    const char* no_gui = getenv("JARVIS_NO_GUI");
    if (no_gui && (strcmp(no_gui, "1") == 0 || strcmp(no_gui, "true") == 0)) {
        snprintf(response, response_size, "UI launch is disabled in this environment.");
        return;
    }

    int status = system("python3 src/jarvis_ui.py >/dev/null 2>&1 &");
    if (status == 0) {
        snprintf(response, response_size, "Opening the JARVIS AI UI window.");
    } else {
        snprintf(response, response_size, "I couldn't open the JARVIS UI window.");
    }
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
    char app_label[256] = {0};
    const char* launch_cmds[8] = {0};
    int launch_count = 0;

    if (!lower_cmd) {
        snprintf(response, response_size, "I couldn't process the application name.");
        return;
    }

    // Extract application name
    if (contains_word(lower_cmd, "xcode")) {
        strcpy(app_label, "Xcode");
        launch_cmds[launch_count++] = "open -a \"Xcode\" >/dev/null 2>&1 &";
    }
    else if (contains_word(lower_cmd, "vscode") || strstr(lower_cmd, "vs code") ||
             strstr(lower_cmd, "visual studio") || contains_word(lower_cmd, "code")) {
        strcpy(app_label, "Visual Studio Code");
        launch_cmds[launch_count++] = "code -n >/dev/null 2>&1 &";
        launch_cmds[launch_count++] = "open -b com.microsoft.VSCode >/dev/null 2>&1 &";
        launch_cmds[launch_count++] = "open -b com.microsoft.VSCodeInsiders >/dev/null 2>&1 &";
        launch_cmds[launch_count++] = "open -a \"Visual Studio Code\" >/dev/null 2>&1 &";
        launch_cmds[launch_count++] = "open -a \"Visual Studio Code - Insiders\" >/dev/null 2>&1 &";
    }
    else if (contains_word(lower_cmd, "chrome") || contains_word(lower_cmd, "google")) {
        strcpy(app_label, "Google Chrome");
        launch_cmds[launch_count++] = "open -a \"Google Chrome\" >/dev/null 2>&1 &";
        launch_cmds[launch_count++] = "open -b com.google.Chrome >/dev/null 2>&1 &";
    }
    else if (contains_word(lower_cmd, "safari")) {
        strcpy(app_label, "Safari");
        launch_cmds[launch_count++] = "open -a \"Safari\" >/dev/null 2>&1 &";
    }
    else if (contains_word(lower_cmd, "firefox")) {
        strcpy(app_label, "Firefox");
        launch_cmds[launch_count++] = "open -a \"Firefox\" >/dev/null 2>&1 &";
        launch_cmds[launch_count++] = "open -b org.mozilla.firefox >/dev/null 2>&1 &";
    }
    else if (contains_word(lower_cmd, "terminal")) {
        strcpy(app_label, "Terminal");
        launch_cmds[launch_count++] = "open -a \"Terminal\" >/dev/null 2>&1 &";
    }
    else if (contains_word(lower_cmd, "finder") || contains_word(lower_cmd, "files")) {
        strcpy(app_label, "Finder");
        launch_cmds[launch_count++] = "open -a \"Finder\" >/dev/null 2>&1 &";
    }
    else if (contains_word(lower_cmd, "spotify") || contains_word(lower_cmd, "music")) {
        strcpy(app_label, "Spotify");
        launch_cmds[launch_count++] = "open -a \"Spotify\" >/dev/null 2>&1 &";
    }
    else if (contains_word(lower_cmd, "docker")) {
        strcpy(app_label, "Docker");
        launch_cmds[launch_count++] = "open -a \"Docker\" >/dev/null 2>&1 &";
    }
    else if (contains_word(lower_cmd, "postman")) {
        strcpy(app_label, "Postman");
        launch_cmds[launch_count++] = "open -a \"Postman\" >/dev/null 2>&1 &";
    }
    else {
        snprintf(response, response_size, "Which application would you like to open? "
                 "Try: Chrome, Safari, Firefox, Terminal, Finder, Spotify, or VS Code.");
        free(lower_cmd);
        return;
    }

    const char* no_gui = getenv("JARVIS_NO_GUI");
    if (no_gui && (strcmp(no_gui, "1") == 0 || strcmp(no_gui, "true") == 0)) {
        snprintf(response, response_size, "Opening %s for you. (GUI launch disabled in this environment.)", app_label);
        free(lower_cmd);
        return;
    }

    int launched = 0;
    for (int i = 0; i < launch_count; i++) {
        if (launch_cmds[i] && system(launch_cmds[i]) == 0) {
            launched = 1;
            break;
        }
    }

    if (launched) {
        snprintf(response, response_size, "Opening %s for you.", app_label);
    } else {
        snprintf(response, response_size, "I couldn't open %s automatically. "
                 "Please verify it is installed and that VS Code command line tools are enabled.", app_label);
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

static int contains_word(const char* text, const char* word) {
    if (!text || !word || strlen(word) == 0) {
        return 0;
    }

    size_t word_len = strlen(word);
    const char* cursor = text;
    while ((cursor = strstr(cursor, word)) != NULL) {
        char prev = (cursor == text) ? ' ' : cursor[-1];
        char next = cursor[word_len];
        int prev_ok = !(isalnum((unsigned char)prev) || prev == '_');
        int next_ok = !(isalnum((unsigned char)next) || next == '_');
        if (prev_ok && next_ok) {
            return 1;
        }
        cursor += word_len;
    }

    return 0;
}

static int is_project_stop_word(const char* token) {
    if (!token || strlen(token) == 0) {
        return 0;
    }

    const char* stop_words[] = {
        "in", "using", "with", "for", "on", "inside",
        "vscode", "code", "language", "as", "called", "and", "then", "open", "folder",
        "python", "javascript", "typescript", "java",
        "golang", "go", "rust", "html", "c", "cpp", "c++"
    };

    for (size_t i = 0; i < sizeof(stop_words) / sizeof(stop_words[0]); i++) {
        if (strcmp(token, stop_words[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

static void detect_project_language(const char* command, char* language, size_t language_size) {
    const char* detected = "c";

    if (strstr(command, "c++") || contains_word(command, "cpp")) {
        detected = "cpp";
    } else if (contains_word(command, "python")) {
        detected = "python";
    } else if (contains_word(command, "typescript")) {
        detected = "typescript";
    } else if (contains_word(command, "javascript") || contains_word(command, "node")) {
        detected = "javascript";
    } else if (contains_word(command, "java")) {
        detected = "java";
    } else if (contains_word(command, "golang") || strstr(command, " in go") != NULL) {
        detected = "go";
    } else if (contains_word(command, "rust")) {
        detected = "rust";
    } else if (contains_word(command, "html")) {
        detected = "html";
    }

    snprintf(language, language_size, "%s", detected);
}

static int extract_project_name_from_command(const char* command, char* project_name, size_t project_name_size) {
    if (!command || !project_name || project_name_size == 0) {
        return 0;
    }

    project_name[0] = '\0';
    const char* project_pos = strstr(command, "project");
    if (!project_pos) {
        return 0;
    }

    project_pos += strlen("project");
    while (*project_pos && isspace((unsigned char)*project_pos)) {
        project_pos++;
    }

    if (strncmp(project_pos, "named ", 6) == 0) {
        project_pos += 6;
    } else if (strncmp(project_pos, "called ", 7) == 0) {
        project_pos += 7;
    } else if (strncmp(project_pos, "name ", 5) == 0) {
        project_pos += 5;
    }

    size_t out_len = 0;
    while (*project_pos) {
        while (*project_pos && isspace((unsigned char)*project_pos)) {
            project_pos++;
        }
        while (*project_pos == '"' || *project_pos == '\'') {
            project_pos++;
        }
        if (!*project_pos) {
            break;
        }

        char token[64] = {0};
        size_t token_len = 0;
        while (project_pos[token_len] && !isspace((unsigned char)project_pos[token_len]) &&
               token_len < sizeof(token) - 1) {
            char ch = project_pos[token_len];
            if (ch == '"' || ch == '\'' || ch == '.' || ch == ',' || ch == '!' || ch == '?' || ch == ':') {
                break;
            }
            token[token_len] = ch;
            token_len++;
        }
        token[token_len] = '\0';

        project_pos += token_len;
        while (*project_pos && !isspace((unsigned char)*project_pos)) {
            project_pos++;
        }

        if (token[0] == '\0' || is_project_stop_word(token)) {
            break;
        }

        char normalized_token[64] = {0};
        size_t norm_len = 0;
        for (size_t i = 0; token[i] && norm_len < sizeof(normalized_token) - 1; i++) {
            char ch = token[i];
            if (isalnum((unsigned char)ch) || ch == '_' || ch == '-') {
                if (ch == '-') {
                    ch = '_';
                }
                normalized_token[norm_len++] = (char)tolower((unsigned char)ch);
            }
        }
        normalized_token[norm_len] = '\0';

        if (norm_len == 0) {
            continue;
        }

        if (out_len > 0 && out_len < project_name_size - 1) {
            project_name[out_len++] = '_';
        }

        for (size_t i = 0; normalized_token[i] && out_len < project_name_size - 1; i++) {
            project_name[out_len++] = normalized_token[i];
        }

        if (out_len >= project_name_size - 1) {
            break;
        }
    }

    project_name[out_len] = '\0';
    return out_len > 0;
}

static int write_file_content(const char* file_path, const char* content) {
    FILE* file = fopen(file_path, "w");
    if (!file) {
        return 0;
    }

    fputs(content, file);
    fclose(file);
    return 1;
}

static int create_project_files(const char* project_name, const char* language, char* entry_file, size_t entry_file_size,
                                char* error_message, int error_message_size) {
    struct stat st;
    if (stat(project_name, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            snprintf(error_message, error_message_size, "'%s' exists but is not a directory.", project_name);
            return 0;
        }
    } else {
        if (mkdir(project_name, 0755) != 0) {
            snprintf(error_message, error_message_size, "Failed to create project folder '%s'.", project_name);
            return 0;
        }
    }

    const char* starter_code = "#include <stdio.h>\n\nint main(void) {\n    printf(\"Hello from JARVIS!\\n\");\n    return 0;\n}\n";
    const char* entry_name = "main.c";

    if (strcmp(language, "cpp") == 0) {
        entry_name = "main.cpp";
        starter_code = "#include <iostream>\n\nint main() {\n    std::cout << \"Hello from JARVIS!\" << std::endl;\n    return 0;\n}\n";
    } else if (strcmp(language, "python") == 0) {
        entry_name = "main.py";
        starter_code = "def main() -> None:\n    print(\"Hello from JARVIS!\")\n\n\nif __name__ == \"__main__\":\n    main()\n";
    } else if (strcmp(language, "javascript") == 0) {
        entry_name = "index.js";
        starter_code = "function main() {\n  console.log(\"Hello from JARVIS!\");\n}\n\nmain();\n";
    } else if (strcmp(language, "typescript") == 0) {
        entry_name = "index.ts";
        starter_code = "function main(): void {\n  console.log(\"Hello from JARVIS!\");\n}\n\nmain();\n";
    } else if (strcmp(language, "java") == 0) {
        entry_name = "Main.java";
        starter_code = "public class Main {\n    public static void main(String[] args) {\n        System.out.println(\"Hello from JARVIS!\");\n    }\n}\n";
    } else if (strcmp(language, "go") == 0) {
        entry_name = "main.go";
        starter_code = "package main\n\nimport \"fmt\"\n\nfunc main() {\n    fmt.Println(\"Hello from JARVIS!\")\n}\n";
    } else if (strcmp(language, "rust") == 0) {
        entry_name = "main.rs";
        starter_code = "fn main() {\n    println!(\"Hello from JARVIS!\");\n}\n";
    } else if (strcmp(language, "html") == 0) {
        entry_name = "index.html";
        starter_code = "<!doctype html>\n<html>\n  <head>\n    <meta charset=\"utf-8\" />\n    <title>JARVIS Project</title>\n  </head>\n  <body>\n    <h1>Hello from JARVIS!</h1>\n  </body>\n</html>\n";
    }

    char entry_path[512];
    snprintf(entry_path, sizeof(entry_path), "%s/%s", project_name, entry_name);
    if (access(entry_path, F_OK) == 0) {
        snprintf(error_message, error_message_size, "Entry file already exists: %s", entry_path);
        return 0;
    }

    if (!write_file_content(entry_path, starter_code)) {
        snprintf(error_message, error_message_size, "Failed to write starter file: %s", entry_path);
        return 0;
    }

    char readme_path[512];
    snprintf(readme_path, sizeof(readme_path), "%s/README.md", project_name);
    if (access(readme_path, F_OK) != 0) {
        char readme_content[1024];
        snprintf(readme_content, sizeof(readme_content),
                 "# %s\n\nCreated automatically by JARVIS.\n\nLanguage: %s\nEntry file: %s\n",
                 project_name, language, entry_name);
        if (!write_file_content(readme_path, readme_content)) {
            snprintf(error_message, error_message_size, "Failed to write README.md.");
            return 0;
        }
    }

    if (strcmp(language, "c") == 0) {
        char makefile_path[512];
        snprintf(makefile_path, sizeof(makefile_path), "%s/Makefile", project_name);
        if (access(makefile_path, F_OK) != 0) {
            char c_makefile[1024];
            snprintf(c_makefile, sizeof(c_makefile),
                     "CC = gcc\n"
                     "CFLAGS = -Wall -Wextra -std=c11 -O2\n"
                     "TARGET = %s\n"
                     "SRC = main.c\n\n"
                     "all: $(TARGET)\n\n"
                     "$(TARGET): $(SRC)\n"
                     "\t$(CC) $(CFLAGS) -o $(TARGET) $(SRC)\n\n"
                     "clean:\n"
                     "\trm -f $(TARGET)\n",
                     project_name);
            if (!write_file_content(makefile_path, c_makefile)) {
                snprintf(error_message, error_message_size, "Failed to write project Makefile.");
                return 0;
            }
        }
    }

    snprintf(entry_file, entry_file_size, "%s", entry_name);
    return 1;
}

static int open_project_in_vscode(const char* project_name, const char* entry_file) {
    const char* no_gui = getenv("JARVIS_NO_GUI");
    if (no_gui && (strcmp(no_gui, "1") == 0 || strcmp(no_gui, "true") == 0)) {
        return 1;
    }

    if (system("command -v code >/dev/null 2>&1") == 0) {
        char code_cmd[768];
        if (entry_file && strlen(entry_file) > 0) {
            snprintf(code_cmd, sizeof(code_cmd),
                     "code -n \"%s\" -g \"%s/%s\" >/dev/null 2>&1 &",
                     project_name, project_name, entry_file);
        } else {
            snprintf(code_cmd, sizeof(code_cmd),
                     "code -n \"%s\" >/dev/null 2>&1 &",
                     project_name);
        }
        return system(code_cmd) == 0;
    }

#ifdef __APPLE__
    char mac_cmd[512];
    snprintf(mac_cmd, sizeof(mac_cmd),
             "open -a \"Visual Studio Code\" \"%s\" >/dev/null 2>&1 &",
             project_name);
    return system(mac_cmd) == 0;
#elif defined(__linux__)
    char linux_cmd[512];
    snprintf(linux_cmd, sizeof(linux_cmd), "xdg-open \"%s\" >/dev/null 2>&1 &", project_name);
    return system(linux_cmd) == 0;
#else
    (void)entry_file;
    return 0;
#endif
}

static int open_file_in_vscode(const char* file_path) {
    const char* no_gui = getenv("JARVIS_NO_GUI");
    if (no_gui && (strcmp(no_gui, "1") == 0 || strcmp(no_gui, "true") == 0)) {
        return 1;
    }

    if (system("command -v code >/dev/null 2>&1") == 0) {
        char code_cmd[768];
        snprintf(code_cmd, sizeof(code_cmd), "code -g \"%s\" >/dev/null 2>&1 &", file_path);
        return system(code_cmd) == 0;
    }

#ifdef __APPLE__
    char mac_cmd[768];
    snprintf(mac_cmd, sizeof(mac_cmd),
             "open -a \"Visual Studio Code\" \"%s\" >/dev/null 2>&1 &",
             file_path);
    return system(mac_cmd) == 0;
#elif defined(__linux__)
    char linux_cmd[768];
    snprintf(linux_cmd, sizeof(linux_cmd), "xdg-open \"%s\" >/dev/null 2>&1 &", file_path);
    return system(linux_cmd) == 0;
#else
    return 0;
#endif
}

static int save_last_project_name(const char* project_name) {
    if (!project_name || strlen(project_name) == 0) {
        return 0;
    }

    FILE* file = fopen(".jarvis_last_project", "w");
    if (!file) {
        return 0;
    }
    fprintf(file, "%s\n", project_name);
    fclose(file);
    return 1;
}

static int load_last_project_name(char* project_name, size_t project_name_size) {
    if (!project_name || project_name_size == 0) {
        return 0;
    }

    FILE* file = fopen(".jarvis_last_project", "r");
    if (!file) {
        return 0;
    }

    if (!fgets(project_name, (int)project_name_size, file)) {
        fclose(file);
        return 0;
    }
    fclose(file);

    project_name[strcspn(project_name, "\n")] = '\0';
    if (strlen(project_name) == 0) {
        return 0;
    }
    return 1;
}

static void execute_ai_project_setup_command(const char* command, char* response, int response_size) {
    char project_name[128] = {0};
    if (!extract_project_name_from_command(command, project_name, sizeof(project_name))) {
        snprintf(project_name, sizeof(project_name), "new_project");
    }

    char language[32] = {0};
    detect_project_language(command, language, sizeof(language));

    char entry_file[128] = {0};
    char error_message[256] = {0};
    if (!create_project_files(project_name, language, entry_file, sizeof(entry_file),
                              error_message, sizeof(error_message))) {
        snprintf(response, response_size, "I couldn't create the project: %s", error_message);
        return;
    }

    save_last_project_name(project_name);

    int opened = open_project_in_vscode(project_name, entry_file);
    if (opened) {
        snprintf(response, response_size,
                 "Created %s project '%s' with %s and opened it in VS Code. Ready for coding.",
                 language, project_name, entry_file);
    } else {
        snprintf(response, response_size,
                 "Created %s project '%s' with %s. I couldn't auto-open VS Code; please open the folder manually.",
                 language, project_name, entry_file);
    }
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
    else if (strstr(command, "create folder") || strstr(command, "new folder")) {
        const char* start = strstr(command, "create folder");
        if (start) {
            start += strlen("create folder");
        } else {
            start = strstr(command, "new folder");
            if (start) {
                start += strlen("new folder");
            }
        }

        if (!start) {
            snprintf(response, response_size, "Please specify a folder name.");
            return;
        }

        while (*start && isspace((unsigned char)*start)) {
            start++;
        }

        char raw_folder_path[256] = {0};
        size_t i = 0;
        while (start[i] && !isspace((unsigned char)start[i]) && i < sizeof(raw_folder_path) - 1) {
            raw_folder_path[i] = start[i];
            i++;
        }
        raw_folder_path[i] = '\0';

        char folder_path[256] = {0};
        if (!sanitize_relative_path(raw_folder_path, folder_path, sizeof(folder_path))) {
            snprintf(response, response_size, "Invalid folder name. Use something like src/utils or notes.");
            return;
        }

        char full_path[512] = {0};
        snprintf(full_path, sizeof(full_path), "%s", folder_path);

        const char* project_pos = strstr(command, " in project ");
        if (project_pos) {
            project_pos += strlen(" in project ");
            char project_name[128] = {0};
            if (sanitize_relative_path(project_pos, project_name, sizeof(project_name))) {
                snprintf(full_path, sizeof(full_path), "%s/%s", project_name, folder_path);
            }
        }

        struct stat st;
        if (stat(full_path, &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                snprintf(response, response_size, "Folder already exists: %s", full_path);
            } else {
                snprintf(response, response_size, "A file already exists at %s", full_path);
            }
            return;
        }

        if (create_directory_recursive(full_path)) {
            snprintf(response, response_size, "Created folder %s.", full_path);
        } else {
            snprintf(response, response_size, "Failed to create folder %s.", full_path);
        }
    }
    else if (strstr(command, "open file")) {
        const char* start = strstr(command, "open file");
        if (!start) {
            snprintf(response, response_size, "Please specify a file to open.");
            return;
        }
        start += strlen("open file");
        while (*start && isspace((unsigned char)*start)) {
            start++;
        }

        char raw_file_path[256] = {0};
        size_t i = 0;
        while (start[i] && !isspace((unsigned char)start[i]) && i < sizeof(raw_file_path) - 1) {
            raw_file_path[i] = start[i];
            i++;
        }
        raw_file_path[i] = '\0';

        char file_path[256] = {0};
        if (!sanitize_relative_path(raw_file_path, file_path, sizeof(file_path))) {
            snprintf(response, response_size, "Invalid file path. Use something like src/main.c");
            return;
        }

        struct stat st;
        if (stat(file_path, &st) != 0 || !S_ISREG(st.st_mode)) {
            snprintf(response, response_size, "I couldn't find file %s.", file_path);
            return;
        }

        if (open_file_in_vscode(file_path)) {
            snprintf(response, response_size, "Opening file %s in VS Code.", file_path);
        } else {
            snprintf(response, response_size, "File %s exists, but I couldn't open VS Code automatically.", file_path);
        }
    }
    else if (strstr(command, "create file") || strstr(command, "new file")) {
        const char* start = strstr(command, "create file");
        if (start) {
            start += strlen("create file");
        } else {
            start = strstr(command, "new file");
            if (start) {
                start += strlen("new file");
            }
        }

        if (!start) {
            snprintf(response, response_size, "Please specify a file name.");
            return;
        }

        while (*start && isspace((unsigned char)*start)) {
            start++;
        }

        char raw_file_path[256] = {0};
        size_t i = 0;
        while (start[i] && !isspace((unsigned char)start[i]) && i < sizeof(raw_file_path) - 1) {
            raw_file_path[i] = start[i];
            i++;
        }
        raw_file_path[i] = '\0';

        char file_path[256] = {0};
        if (!sanitize_relative_path(raw_file_path, file_path, sizeof(file_path))) {
            snprintf(response, response_size, "Invalid file name. Use something like notes.md or src/main.py");
            return;
        }

        char full_path[512] = {0};
        snprintf(full_path, sizeof(full_path), "%s", file_path);

        const char* project_pos = strstr(command, " in project ");
        if (project_pos) {
            project_pos += strlen(" in project ");
            char project_name[128] = {0};
            if (sanitize_relative_path(project_pos, project_name, sizeof(project_name))) {
                snprintf(full_path, sizeof(full_path), "%s/%s", project_name, file_path);
            }
        }

        int wants_template = strstr(command, "template") != NULL ||
                             strstr(command, "boilerplate") != NULL ||
                             strstr(command, "starter") != NULL ||
                             strstr(command, "code") != NULL;

        create_file_with_optional_template(full_path, wants_template, response, response_size);
    }
}
