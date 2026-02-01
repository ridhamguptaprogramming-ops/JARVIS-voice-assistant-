# JARVIS - Custom Commands Guide

This guide shows you how to add new commands to JARVIS.

## Quick Add Command Example

### Step 1: Edit command_processor.c

Open [src/command_processor.c](src/command_processor.c) and find the `process_command()` function.

Add your new command before the final `else` clause:

```c
// Example: Add a calculator command
else if (command_contains(lower_cmd, "calculate") || 
         command_contains(lower_cmd, "add")) {
    strcpy(response, "I can help you with calculations. Please provide the numbers.");
}
```

### Step 2: Rebuild

```bash
make rebuild
```

### Step 3: Test

```bash
./jarvis
```

Then try your new command!

## Complete Command Adding Example

Let's add a "temperature" command:

### Add to command_processor.c

```c
// Find this section in process_command()
else if (command_contains(lower_cmd, "calculate") || 
         command_contains(lower_cmd, "what is")) {
    strcpy(response, "I can help with calculations. Please state your math problem.");
}

// Add this new command right after:
else if (command_contains(lower_cmd, "temperature") || 
         command_contains(lower_cmd, "celsius") ||
         command_contains(lower_cmd, "fahrenheit")) {
    strcpy(response, "Water boils at 100 degrees Celsius or 212 degrees Fahrenheit.");
}
```

Save and rebuild:
```bash
make rebuild
```

Test it:
```bash
./jarvis
> what is the temperature
[JARVIS] Water boils at 100 degrees Celsius or 212 degrees Fahrenheit.
```

## Advanced: Command with Multiple Keywords

```c
else if (command_contains(lower_cmd, "movie") ||
         command_contains(lower_cmd, "film") ||
         command_contains(lower_cmd, "cinema")) {
    
    if (command_contains(lower_cmd, "recommend")) {
        strcpy(response, "I recommend watching Inception or The Matrix.");
    } else {
        strcpy(response, "Movie suggestion: Check IMDb for top-rated films.");
    }
}
```

## Advanced: Command with Dynamic Response

```c
#include <stdio.h>  // Add at top if not already there

// Then in process_command():
else if (command_contains(lower_cmd, "random")) {
    int num = rand() % 100;
    snprintf(response, 512, "Your random number is %d", num);
}
```

## How to Add Helper Functions

If you need more complex logic:

### 1. Add function declaration in header:

Edit [include/command_processor.h](include/command_processor.h):

```c
/**
 * Gets a random greeting
 */
char* get_random_greeting(void);
```

### 2. Add function implementation:

Edit [src/command_processor.c](src/command_processor.c):

```c
char* get_random_greeting(void) {
    const char* greetings[] = {
        "Hello sir",
        "Good day",
        "Greetings",
        "How do you do"
    };
    
    int idx = rand() % 4;
    char* greeting = (char*)malloc(50);
    strcpy(greeting, greetings[idx]);
    return greeting;
}
```

### 3. Use in command:

```c
else if (command_contains(lower_cmd, "greet")) {
    char* greeting = get_random_greeting();
    strcpy(response, greeting);
    free(greeting);
}
```

## Built-in Commands Reference

These are already implemented:

| Keyword | Response |
|---------|----------|
| hello, hi, hey | Greeting message |
| time | Current time |
| help | List of available commands |
| system, info | System information |
| joke | Programming joke |
| weather | Weather disclaimer |
| calculate, what is | Math help |
| quit, exit, shutdown | Program termination |

## Important Notes

### Memory Management
- Always use `malloc()` for new strings
- Call `free()` when done with allocated memory
- Use `strcpy()` for fixed buffers in `process_command()`

### String Safety
- Response buffer is 512 bytes: `snprintf(response, 512, ...)`
- Use `snprintf()` instead of `sprintf()` to prevent overflow
- Keep responses under 500 characters

### Case Sensitivity
- Use `to_lowercase()` to convert input to lowercase
- Use `command_contains()` for keyword matching
- Example: `command_contains(lower_cmd, "keyword")`

## Testing Your New Commands

### Run in Interactive Mode
```bash
make run
```

### Use Demo Script
Create a test in [demo.sh](demo.sh):

```bash
(
echo "hello jarvis"
sleep 1
echo "your_new_command"  # Add your test command
sleep 1
echo "quit"
) | ./jarvis
```

## Troubleshooting

### Command Not Recognized
- Check spelling in keyword matching
- Ensure you're using `command_contains()` correctly
- Rebuild with `make rebuild`

### Response Not Showing
- Verify response string is set correctly
- Check that command matching condition is true
- Look for typos in if/else conditions

### Compilation Error
- Check for missing semicolons
- Verify proper parentheses matching
- Ensure malloc() is balanced with free()
- Run `make clean` then `make` again

## Example: Complete Custom Command

Here's a complete example to add a greeting with time:

```c
else if (command_contains(lower_cmd, "greet me")) {
    time_t now = time(NULL);
    struct tm* timeinfo = localtime(&now);
    
    if (timeinfo->tm_hour < 12) {
        strftime(response, 512, "Good morning! It's %I:%M AM", timeinfo);
    } else if (timeinfo->tm_hour < 18) {
        strftime(response, 512, "Good afternoon! It's %I:%M PM", timeinfo);
    } else {
        strftime(response, 512, "Good evening! It's %I:%M PM", timeinfo);
    }
}
```

## Building & Testing Workflow

1. **Edit** - Modify [src/command_processor.c](src/command_processor.c)
2. **Build** - Run `make` or `make rebuild`
3. **Test** - Execute `./jarvis` and try the command
4. **Refine** - Repeat until satisfied

## Need More Help?

- Read [README.md](README.md) for full documentation
- Check [QUICKSTART.md](QUICKSTART.md) for usage examples
- Review existing commands in [src/command_processor.c](src/command_processor.c)
- Study the header files in [include/](include/) directory

---

**Happy Coding!** 🚀

Remember: Test your changes and keep the code clean!
