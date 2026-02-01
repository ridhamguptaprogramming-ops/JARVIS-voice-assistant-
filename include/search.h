#ifndef SEARCH_H
#define SEARCH_H

#include <stdlib.h>

/**
 * Performs a web search using DuckDuckGo API
 * @param query The search query string
 * @return Dynamically allocated string with search results
 */
char* web_search(const char* query);

/**
 * Searches for files on the system
 * @param filename Pattern to search for
 * @return Dynamically allocated string with file paths found
 */
char* file_search(const char* filename);

/**
 * Executes a command and returns its output
 * @param command The command to execute
 * @return Dynamically allocated string with command output
 */
char* execute_command_search(const char* command);

/**
 * Performs a general search based on query type
 * @param query The search query
 * @return Dynamically allocated string with search results
 */
char* general_search(const char* query);

/**
 * Extracts search query from user input
 * @param input The user input string
 * @return Search query (pointer to part of input string)
 */
const char* extract_search_query(const char* input);

#endif // SEARCH_H
