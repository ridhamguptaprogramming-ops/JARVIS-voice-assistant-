#include "../include/command_processor.h"
#include "../include/search.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

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
    char* response = (char*)malloc(512);
    
    if (!response) {
        free(lower_cmd);
        return NULL;
    }
    
    // Time-related commands
    if (command_contains(lower_cmd, "time")) {
        time_t now = time(NULL);
        struct tm* timeinfo = localtime(&now);
        strftime(response, 512, "The current time is %I:%M %p", timeinfo);
    }
    // Greeting commands
    else if (command_contains(lower_cmd, "hello") || 
             command_contains(lower_cmd, "hi") ||
             command_contains(lower_cmd, "hey")) {
        strcpy(response, "Hello sir. I am JARVIS. How may I assist you today?");
    }
    // Help command
    else if (command_contains(lower_cmd, "help")) {
        strcpy(response, "I can: tell the time, open applications, search the web, tell jokes, "
                        "show system info, and execute commands. Try: 'open chrome', 'search for something', or 'time'.");
    }
    // System info command
    else if (command_contains(lower_cmd, "system") || 
             command_contains(lower_cmd, "info") ||
             command_contains(lower_cmd, "status")) {
        snprintf(response, 512, "System information requested. JARVIS version 2.0.0 with voice control is running smoothly on macOS.");
    }
    // Open application commands
    else if (command_contains(lower_cmd, "open")) {
        execute_open_command(command, response, 512);
    }
    // Joke command
    else if (command_contains(lower_cmd, "joke") ||
             command_contains(lower_cmd, "tell")) {
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
    // YouTube search commands
    else if (command_contains(lower_cmd, "youtube") || 
             command_contains(lower_cmd, "video") ||
             command_contains(lower_cmd, "watch")) {
        execute_youtube_command(command, response, 512);
    }
    // Web page opening commands
    else if (command_contains(lower_cmd, "open website") ||
             command_contains(lower_cmd, "go to") ||
             command_contains(lower_cmd, "visit")) {
        execute_webpage_command(command, response, 512);
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
            strncpy(response, search_result, 511);
            response[511] = '\0';
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
            snprintf(response, 512, "I have switched to %s mode.", mode);
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
            if (fgets(response, 512, fp) == NULL) {
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
        snprintf(response, 512, "I'm sorry, I didn't understand '%s'. "
                              "Try: time, joke, help, info, search for something, or quit.", command);
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
    const char* lower_cmd = command;
    char app_name[256] = {0};
    char macos_app[256] = {0};
    
    // Extract application name
    if (strstr(command, "chrome") || strstr(command, "google")) {
        strcpy(app_name, "chrome");
        strcpy(macos_app, "Google Chrome");
    } 
    else if (strstr(command, "safari")) {
        strcpy(app_name, "safari");
        strcpy(macos_app, "Safari");
    }
    else if (strstr(command, "firefox")) {
        strcpy(app_name, "firefox");
        strcpy(macos_app, "Firefox");
    }
    else if (strstr(command, "terminal") || strstr(command, "terminal")) {
        strcpy(app_name, "terminal");
        strcpy(macos_app, "Terminal");
    }
    else if (strstr(command, "finder") || strstr(command, "files")) {
        strcpy(app_name, "finder");
        strcpy(macos_app, "Finder");
    }
    else if (strstr(command, "spotify") || strstr(command, "music")) {
        strcpy(app_name, "spotify");
        strcpy(macos_app, "Spotify");
    }
    else if (strstr(command, "vscode") || strstr(command, "code") || strstr(command, "visual studio")) {
        strcpy(app_name, "vscode");
        strcpy(macos_app, "Visual Studio Code");
    }
    else {
        snprintf(response, response_size, "Which application would you like to open? "
                 "Try: Chrome, Safari, Firefox, Terminal, Finder, Spotify, or VS Code.");
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
