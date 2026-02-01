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
        strcpy(response, "I can help you with: checking the time, system information, "
                        "telling jokes, and basic calculations. What would you like?");
    }
    // System info command
    else if (command_contains(lower_cmd, "system") || 
             command_contains(lower_cmd, "info")) {
        snprintf(response, 512, "System information requested. JARVIS version 1.0.0 is running smoothly on macOS.");
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
    // Calculate command
    else if (command_contains(lower_cmd, "calculate") || 
             command_contains(lower_cmd, "what is")) {
        strcpy(response, "I can help with calculations. Please state your math problem.");
    }
    // Search commands
    else if (command_contains(lower_cmd, "search") || 
             command_contains(lower_cmd, "find") ||
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
    // Default response
    else {
        // Check if it might be a search query (user just states what they want)
        if (strlen(command) > 3 && !command_contains(lower_cmd, "i ")) {
            // Treat as potential search query
            char* search_result = general_search(command);
            if (search_result) {
                strncpy(response, search_result, 511);
                response[511] = '\0';
                free(search_result);
            } else {
                snprintf(response, 512, "I understood: '%s'. I'm still learning about this command. "
                                      "Would you like me to help with something else?", command);
            }
        } else {
            snprintf(response, 512, "I understood: '%s'. I'm still learning about this command. "
                                  "Would you like me to help with something else?", command);
        }
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
