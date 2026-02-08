# 🧠 JARVIS AI Upgrade Guide

You have added the AI engine (`src/ai_chat.py`). Now follow these steps to connect it to JARVIS.

## Step 1: Install Python Library

Open your terminal and run:
```bash
pip install google-generativeai
```

## Step 2: Get a Free API Key

1. Go to Google AI Studio.
2. Create a new API Key.
3. Export it in your terminal (or add to your `.bashrc` / `.zshrc`):
   ```bash
   export GEMINI_API_KEY="your_api_key_here"
   ```

## Step 3: Update C Code

Open `src/command_processor.c` and find the `process_command` function. Add this logic to handle "Ask AI" commands.

Add this code block before the final `else` (unknown command) section:

```c
// AI Integration
else if (command_contains(lower_cmd, "ask ai") || 
         command_contains(lower_cmd, "jarvis") ||
         command_contains(lower_cmd, "tell me")) {
    
    printf("[JARVIS] Thinking...\n");
    
    // Prepare command to call Python script
    char sys_cmd[1024];
    // Escape double quotes in command if necessary, simple version here:
    snprintf(sys_cmd, sizeof(sys_cmd), "python3 src/ai_chat.py \"%s\"", command);
    
    FILE* fp = popen(sys_cmd, "r");
    if (fp == NULL) {
        strcpy(response, "I cannot access my AI brain right now.");
    } else {
        // Read the output from Python
        if (fgets(response, 512, fp) == NULL) {
            strcpy(response, "I couldn't generate a response.");
        }
        pclose(fp);
    }
}
```

## Step 4: Rebuild

```bash
make rebuild
```

## Step 5: Test

```bash
./jarvis
> ask ai what is the capital of mars
> jarvis tell me a short story
```