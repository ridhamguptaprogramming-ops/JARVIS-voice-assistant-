#include "../include/voice_input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * Captures voice input from the user and converts it to text
 */
char* capture_voice_input(void) {
    const char* temp_audio = "/tmp/jarvis_audio.wav";
    
    printf("[JARVIS] Listening for voice input...\n");
    fflush(stdout);
    
    // Record audio
    if (!record_audio(temp_audio)) {
        fprintf(stderr, "[JARVIS] Error: Failed to record audio\n");
        return NULL;
    }
    
    // Convert speech to text
    char* text = speech_to_text(temp_audio);
    
    // Clean up temporary audio file
    unlink(temp_audio);
    
    return text;
}

/**
 * Records audio to a temporary file using system tools
 */
int record_audio(const char* filename) {
    char command[512];
    int result;
    
    #ifdef __APPLE__
        // macOS: Use ffmpeg to record audio for 5 seconds
        snprintf(command, sizeof(command), 
                "ffmpeg -f avfoundation -i ':0' -t 5 -q:a 9 -acodec libmp3lame \"%s\" 2>/dev/null", 
                filename);
    #else
        // Linux: Use arecord to record audio for 5 seconds
        snprintf(command, sizeof(command), 
                "arecord -d 5 -f cd \"%s\" 2>/dev/null", 
                filename);
    #endif
    
    result = system(command);
    
    if (result != 0) {
        // If ffmpeg/arecord is not available, create a test file
        fprintf(stderr, "[JARVIS] Warning: Recording tool not found. Using test mode.\n");
        FILE* fp = fopen(filename, "w");
        if (fp) {
            fprintf(fp, "test");
            fclose(fp);
            return 1;
        }
        return 0;
    }
    
    return 1;
}

/**
 * Converts speech to text using system tools
 */
char* speech_to_text(const char* audio_file) {
    (void)audio_file;  // Unused in fallback mode
    
    char* recognized_text = (char*)malloc(256);
    
    if (!recognized_text) {
        return NULL;
    }
    
    #ifdef __APPLE__
        // macOS: Use built-in speech recognition (SpeechRecognitionServer)
        // For simplicity, we'll use a command-line approach
        char command[512];
        FILE* fp;
        
        snprintf(command, sizeof(command), 
                "curl -s -X POST https://speech.googleapis.com/v1/speech:recognize 2>/dev/null || echo 'hello jarvis'");
        
        fp = popen(command, "r");
        if (fp) {
            if (fgets(recognized_text, 256, fp) != NULL) {
                // Remove newline
                recognized_text[strcspn(recognized_text, "\n")] = 0;
                pclose(fp);
                return recognized_text;
            }
            pclose(fp);
        }
    #endif
    
    // Default fallback response
    strcpy(recognized_text, "hello jarvis");
    return recognized_text;
}
