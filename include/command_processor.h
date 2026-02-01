#ifndef COMMAND_PROCESSOR_H
#define COMMAND_PROCESSOR_H

#include <stdlib.h>
#include <string.h>

/**
 * Processes a voice command and executes appropriate action
 * @param command The voice command string
 * @return Response message to be spoken to the user
 */
char* process_command(const char* command);

/**
 * Converts command string to lowercase for case-insensitive matching
 * @param str String to convert
 * @return Dynamically allocated lowercase string
 */
char* to_lowercase(const char* str);

/**
 * Checks if a command contains a specific keyword
 * @param command The command string
 * @param keyword The keyword to search for
 * @return 1 if found, 0 otherwise
 */
int command_contains(const char* command, const char* keyword);

#endif // COMMAND_PROCESSOR_H
