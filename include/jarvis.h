#ifndef JARVIS_H
#define JARVIS_H

#include <time.h>

#define JARVIS_VERSION "1.0.0"
#define JARVIS_NAME "JARVIS"

/**
 * Gets the current time as a formatted string
 * @return Dynamically allocated time string
 */
char* get_current_time(void);

/**
 * Gets system information
 * @return Dynamically allocated system info string
 */
char* get_system_info(void);

/**
 * Initializes the JARVIS voice assistant
 * @return 1 on success, 0 on failure
 */
int jarvis_init(void);

/**
 * Runs the main event loop for JARVIS
 */
void jarvis_run(void);

/**
 * Cleans up and shuts down JARVIS
 */
void jarvis_cleanup(void);

#endif // JARVIS_H
