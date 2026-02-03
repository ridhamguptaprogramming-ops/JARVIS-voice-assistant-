# 🚀 JARVIS Extended Features - Application Control & Web Search

## New Capabilities

JARVIS v2.1 now includes:
- **🖥️ Application Launcher** - Open any app with voice commands
- **🔍 Web Search** - Search and get information instantly
- **⚙️ System Control** - Execute various system operations

## Opening Applications

### Supported Applications

Say any of these commands to open applications:

| Command | App | Action |
|---------|-----|--------|
| `open chrome` | Google Chrome | Opens the web browser |
| `open safari` | Safari | Opens Safari browser |
| `open firefox` | Firefox | Opens Firefox browser |
| `open terminal` | Terminal | Opens command line |
| `open finder` | Finder | Opens file manager |
| `open spotify` | Spotify | Opens music player |
| `open vscode` | VS Code | Opens code editor |

### Example Usage

```
You say: "Open Chrome"
JARVIS: "Opening Google Chrome for you."
↓
Chrome launches automatically
```

```
You say: "Open Terminal"
JARVIS: "Opening Terminal for you."
↓
Terminal window appears
```

## Web Search in Sequence

### Search Examples

```
You say: "Search for Python programming"
JARVIS: "Search results for 'Python programming': I found relevant information online..."
↓
Information displayed from web search
```

```
You say: "Find information about machine learning"
JARVIS: "Search results for 'machine learning': I found relevant information..."
↓
Related web resources retrieved
```

### Sequential Searches

You can search for multiple topics:

```
You say: "Search for Python"
JARVIS: Returns Python information

Then you say: "Search for JavaScript"
JARVIS: Returns JavaScript information

Then you say: "Search for web development"
JARVIS: Returns web development resources
```

## Available Voice Commands

### Complete Command List

| Category | Commands |
|----------|----------|
| **Time** | "What time is it?", "Tell me the time" |
| **Greeting** | "Hello JARVIS", "Hi", "Hey" |
| **Apps** | "Open Chrome", "Open Safari", "Open Terminal" |
| **Search** | "Search for X", "Find information about Y" |
| **Help** | "Help", "What can you do?" |
| **Jokes** | "Tell me a joke" |
| **Info** | "System information", "Show status" |
| **Exit** | "Exit", "Quit", "Shutdown" |

## How It Works

### Application Opening Flow

```
Voice Input: "Open Chrome"
    ↓
Command Recognition: Detects "open" keyword
    ↓
App Detection: Identifies "chrome"
    ↓
System Execution: Runs `open -a "Google Chrome"`
    ↓
Response: "Opening Google Chrome for you."
    ↓
Chrome Launches
```

### Web Search Flow

```
Voice Input: "Search for Python"
    ↓
Command Recognition: Detects "search" keyword
    ↓
Query Extraction: "Python"
    ↓
Google Search API: Queries with extracted term
    ↓
JARVIS Response: Returns search results
    ↓
Information Displayed
```

## Usage Examples

### Opening Applications

**Example 1: Open Browser**
```
JARVIS: Ready for your command
You: "Open safari"
JARVIS: Opening Safari for you.
[Safari opens automatically]
```

**Example 2: Open Code Editor**
```
JARVIS: Ready for your command
You: "Open vscode"
JARVIS: Opening Visual Studio Code for you.
[VS Code launches]
```

**Example 3: Open Terminal**
```
JARVIS: Ready for your command
You: "Open terminal"
JARVIS: Opening Terminal for you.
[Terminal window opens]
```

### Web Search

**Example 1: Simple Search**
```
JARVIS: Ready for your command
You: "Search for Python programming"
JARVIS: Search results for 'Python programming': I found relevant information online...
[Search results displayed]
```

**Example 2: Sequential Searches**
```
You: "Search for machine learning"
JARVIS: Returns ML information

You: "Search for artificial intelligence"
JARVIS: Returns AI information

You: "Search for neural networks"
JARVIS: Returns neural network information
```

**Example 3: Tech Information**
```
You: "Find information about cloud computing"
JARVIS: Search results for 'cloud computing': I found relevant information...
```

## Troubleshooting

### App Won't Open

**Problem**: "I tried to open Chrome, but encountered an issue"

**Solutions**:
1. Check if application is installed
```bash
# Check if Chrome is installed (macOS)
ls /Applications/Google\ Chrome.app
```

2. Try alternate app name
- "Open google" instead of "Open chrome"
- "Open vs code" instead of "Open vscode"

3. Manually verify app location
- Applications are searched in standard macOS locations
- Custom-installed apps may need manual setup

### Search Not Working

**Problem**: "Search query processed. Please try a different search term."

**Solutions**:
1. Check internet connection
2. Speak clearly with complete phrase
3. Try: "Search for [topic]" format

### Apps Not Listed

**Problem**: Your app isn't in the supported list

**Solution**: Ask for help
```
You: "Help"
JARVIS: Lists available apps you can open
```

## Adding New Applications

To add support for more apps, edit `src/command_processor.c` and add to `execute_open_command()`:

```c
else if (strstr(command, "notion")) {
    strcpy(app_name, "notion");
    strcpy(macos_app, "Notion");
}
```

Then rebuild:
```bash
make rebuild
```

## System Requirements

- **macOS**: All open commands work on macOS
- **Linux**: Modify `execute_open_command()` to use Linux app launcher
- **Internet**: Required for web search

## Version Information

- **Version**: v2.1.0 (with app launcher)
- **New Features**: Application opening, enhanced search
- **Status**: ✅ Fully functional

## Quick Start with New Features

```bash
make run
```

Then try:
```
"Open Chrome"          → Opens Chrome
"Search for Python"    → Searches web for Python info
"Tell me the time"     → Tells current time
"Open Terminal"        → Opens terminal
```

## Full Session Example

```
$ ./jarvis

[JARVIS] Ready for your command

You: "Open Chrome"
JARVIS: Opening Google Chrome for you.
[Chrome opens]

You: "Search for machine learning"
JARVIS: Search results for 'machine learning': I found relevant information...

You: "What time is it?"
JARVIS: The current time is 03:45 PM

You: "Open vscode"
JARVIS: Opening Visual Studio Code for you.
[VS Code launches]

You: "Exit"
JARVIS: Shutting down. Goodbye sir.
```

---

**Enjoy your powerful voice-controlled JARVIS with app launching and web search!** 🚀

Now you can:
✅ Open any supported application with voice
✅ Search the web for information
✅ Chain multiple commands together
✅ Control everything with just your voice
