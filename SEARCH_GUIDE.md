# JARVIS Developer Workflow Guide (C-First)

## Goal
Use JARVIS as a local, voice-driven development assistant for daily coding, automation, and workflow tasks in C projects.

## Focus Areas
- Automation workflows: build/test/warning checks and git routines.
- Voice assistance: run commands hands-free with natural phrases.
- Coding support: code navigation, TODO scanning, and C module scaffolding.

## Quick Start
```bash
make
./jarvis
```

Speak commands naturally, for example:
- `build project`
- `run tests`
- `check warnings`
- `find function process_command`
- `create c module parser`
- `daily status`

## Command Reference

### 1. C Build and Test Automation
Use these commands for local C development workflows:

```text
build project
compile project
rebuild project
clean build
run tests
test project
check warnings
show warnings
```

Behavior:
- Build commands run `make`/`make rebuild`.
- Test commands run `make test`.
- Warning checks rebuild and return warning lines only.

### 2. Coding Support (No External AI Needed)
Use these commands for source navigation and structure:

```text
find symbol process_command
find function execute_open_command
where is function general_search
search code jarvis_run
show todo
list todo
```

Behavior:
- Symbol/function lookup searches `src/` and `include/` using `rg`.
- TODO view returns `TODO`/`FIXME` entries from C sources and headers.

### 3. C Module Scaffolding
Create C module boilerplate in one voice command:

```text
create c module parser
scaffold module memory_pool
```

Generated files:
- `include/<module>.h`
- `src/<module>.c`

The scaffold includes an init function and include guard.

### 4. Daily Workflow Automation
Run fast project status checks:

```text
daily workflow
daily status
morning sync
review changes
git status
git pull
git push
```

Behavior:
- `daily workflow` summary includes branch, short status, and last commit.
- Git commands execute directly and return terminal output.

### 5. Project Navigation
Manage your local workspace:

```text
where am i
list files
go to folder src
change directory to include
create file notes.txt
```

### 6. Web and Developer Search
Use web lookup only when needed:

```text
search for c language memory management
tell me about sockets in c
stack overflow search segmentation fault
github search jarvis command processor
```

### 7. AI-Style Project Bootstrap
Create a new project folder, generate starter code, and open it in VS Code:

```text
create project billing_api in python using vscode
create project low_level_tool in c
new project weather_dash in javascript
start project compiler_lab in cpp
```

Behavior:
- Detects language from the command (C, C++, Python, JavaScript, TypeScript, Java, Go, Rust, HTML).
- Creates a project directory with starter entry file and README.
- Opens the project in a new VS Code window when available.

## Suggested Daily Routine
1. `daily status`
2. `check warnings`
3. `run tests`
4. `find function <name>` while coding
5. `show todo` before wrap-up

## Voice Routing Notes
`process_command()` now prioritizes developer workflows before generic search, so coding and automation tasks are handled as first-class commands.

## Limitations
- `check warnings` depends on compiler warning output.
- Module scaffolding auto-updates `SOURCES` and `OBJECTS` when standard variable lines exist in `Makefile`.
- Auto-open depends on VS Code availability (`code` CLI or OS-level fallback).

## Implementation Pointers
- Command router: `src/command_processor.c`
- Search helpers: `src/search.c`
- Public interfaces: `include/command_processor.h`, `include/search.h`
