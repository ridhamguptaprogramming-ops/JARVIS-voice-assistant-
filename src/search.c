#include "../include/search.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static void trim_whitespace_inplace(char* text) {
    if (!text) return;

    size_t start = 0;
    size_t len = strlen(text);
    while (start < len && isspace((unsigned char)text[start])) {
        start++;
    }

    if (start > 0) {
        memmove(text, text + start, len - start + 1);
        len = strlen(text);
    }

    while (len > 0 && isspace((unsigned char)text[len - 1])) {
        text[len - 1] = '\0';
        len--;
    }
}

static const char* normalize_search_query(const char* query) {
    static char cleaned[512];
    if (!query) {
        cleaned[0] = '\0';
        return cleaned;
    }

    strncpy(cleaned, query, sizeof(cleaned) - 1);
    cleaned[sizeof(cleaned) - 1] = '\0';
    trim_whitespace_inplace(cleaned);

    const char* fillers[] = {
        "karo ",
        "kar do ",
        "kardo ",
        "please ",
        "jarvis ",
        "zara ",
        "bajake ",
        "baja ke "
    };

    int changed = 1;
    while (changed && cleaned[0] != '\0') {
        changed = 0;
        char lowered[512];
        strncpy(lowered, cleaned, sizeof(lowered) - 1);
        lowered[sizeof(lowered) - 1] = '\0';
        for (size_t i = 0; lowered[i]; i++) {
            lowered[i] = (char)tolower((unsigned char)lowered[i]);
        }

        for (size_t i = 0; i < sizeof(fillers) / sizeof(fillers[0]); i++) {
            size_t f_len = strlen(fillers[i]);
            if (strncmp(lowered, fillers[i], f_len) == 0) {
                memmove(cleaned, cleaned + f_len, strlen(cleaned + f_len) + 1);
                trim_whitespace_inplace(cleaned);
                changed = 1;
                break;
            }
        }
    }

    return cleaned;
}

static void url_encode_query(const char* input, char* output, size_t output_size) {
    if (!input || !output || output_size == 0) {
        return;
    }

    size_t out = 0;
    for (size_t i = 0; input[i] != '\0' && out < output_size - 1; i++) {
        unsigned char ch = (unsigned char)input[i];
        if (isalnum(ch) || ch == '-' || ch == '_' || ch == '.' || ch == '~') {
            output[out++] = (char)ch;
        } else if (ch == ' ') {
            output[out++] = '+';
        } else {
            if (out + 3 >= output_size) {
                break;
            }
            snprintf(output + out, output_size - out, "%%%02X", ch);
            out += 3;
        }
    }
    output[out] = '\0';
}

/**
 * Performs a web search using command-line tools
 */
char* web_search(const char* query) {
    if (!query || strlen(query) == 0) {
        return NULL;
    }
    
    char* result = (char*)malloc(1024);
    if (!result) return NULL;
    
    // Simulate web search with curl and DuckDuckGo
    char command[768];
    char encoded_query[512];
    FILE* fp;

    url_encode_query(query, encoded_query, sizeof(encoded_query));
    
    snprintf(command, sizeof(command), 
            "curl -s 'https://duckduckgo.com/?q=%s&format=json' 2>/dev/null | head -c 500", 
            encoded_query);
    
    fp = popen(command, "r");
    if (fp) {
        char buffer[1024];
        if (fgets(buffer, sizeof(buffer), fp) != NULL) {
            // Extract a meaningful response
            if (strstr(buffer, "error") == NULL && strlen(buffer) > 10) {
                snprintf(result, 1024, 
                        "Search results for '%s': I found relevant information online. "
                        "The query has been processed. Please check online resources for detailed results.", 
                        query);
            } else {
                snprintf(result, 1024, 
                        "Searching for '%s' online... Internet search completed. "
                        "For detailed results, please visit a search engine directly.", 
                        query);
            }
            pclose(fp);
            return result;
        }
        pclose(fp);
    }
    
    // Fallback: simulate search results
    snprintf(result, 1024, 
            "Web search initiated for: '%s'. Processing search query through online resources. "
            "Please note: For real-time results, visit DuckDuckGo or Google directly.", 
            query);
    
    return result;
}

/**
 * Searches for files on the system
 */
char* file_search(const char* filename) {
    if (!filename || strlen(filename) == 0) {
        return NULL;
    }
    
    char* result = (char*)malloc(2048);
    if (!result) return NULL;
    
    char command[512];
    FILE* fp;
    int file_count = 0;
    
    // Use find command to search for files (limit search to home directory)
    snprintf(command, sizeof(command), 
            "find ~/ -name '*%s*' -type f 2>/dev/null | head -10 | wc -l", 
            filename);
    
    fp = popen(command, "r");
    if (fp) {
        fscanf(fp, "%d", &file_count);
        pclose(fp);
    }
    
    if (file_count > 0) {
        snprintf(result, 2048, 
                "File search for '%s': Found %d matching file(s) in your system. "
                "Locations include your home directory and subdirectories.", 
                filename, file_count);
    } else {
        snprintf(result, 2048, 
                "File search for '%s': No matching files found in your system directories.", 
                filename);
    }
    
    return result;
}

/**
 * Executes a command and returns its output
 */
char* execute_command_search(const char* cmd) {
    if (!cmd || strlen(cmd) == 0) {
        return NULL;
    }
    
    char* result = (char*)malloc(2048);
    if (!result) return NULL;
    
    FILE* fp;
    char output[1024] = {0};
    
    // Execute the command with output redirection
    fp = popen(cmd, "r");
    if (fp) {
        if (fgets(output, sizeof(output) - 1, fp) != NULL) {
            // Remove trailing newline
            output[strcspn(output, "\n")] = 0;
            snprintf(result, 2048, "Command output: %s", output);
            pclose(fp);
            return result;
        }
        pclose(fp);
    }
    
    snprintf(result, 2048, "Command executed. Output: No significant results returned.");
    return result;
}

/**
 * Extracts search query from user input
 */
const char* extract_search_query(const char* input) {
    if (!input) return NULL;
    
    // Skip common search keywords
    const char* keywords[] = {
        "search karo ",
        "search kar ",
        "search for ",
        "search ",
        "find ",
        "look for ",
        "show me ",
        "tell me about ",
        "what is ",
        "who is ",
        "where is ",
        "when is "
    };
    
    // Convert to lowercase for matching
    char lower_input[512];
    strncpy(lower_input, input, sizeof(lower_input) - 1);
    lower_input[sizeof(lower_input) - 1] = '\0';
    
    for (int i = 0; lower_input[i]; i++) {
        lower_input[i] = tolower((unsigned char)lower_input[i]);
    }
    
    // Find which keyword matches and return the rest
    for (size_t i = 0; i < sizeof(keywords)/sizeof(keywords[0]); i++) {
        const char* found = strstr(lower_input, keywords[i]);
        if (found) {
            // Return pointer to original input after the keyword
            const char* raw_query = input + (found - lower_input) + strlen(keywords[i]);
            return normalize_search_query(raw_query);
        }
    }
    
    return normalize_search_query(input); // Return cleaned input if no keyword found
}

/**
 * Performs a general search based on query type
 */
char* general_search(const char* query) {
    if (!query || strlen(query) == 0) {
        char* response = (char*)malloc(256);
        strcpy(response, "Please provide a search query.");
        return response;
    }
    
    char* result = NULL;
    
    // Check if query is asking for a file
    if (strstr(query, "file") || strstr(query, ".txt") || strstr(query, ".pdf") ||
        strstr(query, ".doc") || strstr(query, ".jpg") || strstr(query, ".png")) {
        result = file_search(query);
    }
    // Check if it's a command-like query
    else if (strstr(query, "users") || strstr(query, "list") || 
             strstr(query, "count") || strstr(query, "how many")) {
        char cmd[256];
        
        if (strstr(query, "users")) {
            strcpy(cmd, "who | wc -l");
        } else if (strstr(query, "list")) {
            strcpy(cmd, "ls -la");
        } else {
            strcpy(cmd, "echo 'Query processed'");
        }
        
        result = execute_command_search(cmd);
    }
    // Default: web search
    else {
        result = web_search(query);
    }
    
    return result;
}
