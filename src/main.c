#include "../include/jarvis.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * Main entry point for JARVIS Voice Assistant
 */
int main(int argc, char* argv[]) {
    (void)argc;  // Unused
    (void)argv;  // Unused
    
    // Initialize JARVIS
    if (!jarvis_init()) {
        fprintf(stderr, "Failed to initialize JARVIS\n");
        return EXIT_FAILURE;
    }
    
    // Run JARVIS
    jarvis_run();
    
    // Cleanup and exit
    jarvis_cleanup();
    
    return EXIT_SUCCESS;
}
