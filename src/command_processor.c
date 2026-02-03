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
    // Search commands - only if explicitly asked
    else if (command_contains(lower_cmd, "search") || 
             command_contains(lower_cmd, "find ") ||
             command_contains(lower_cmd, "look for") ||
             command_contains(lower_cmd, "show me") ||
             command_contains(lower_cmd, "tell me about")) {
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

