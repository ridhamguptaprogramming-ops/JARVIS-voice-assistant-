#!/usr/bin/env bash
set -e

# Run JARVIS in a new terminal window (macOS / common Linux terminals)
# Usage: scripts/run_jarvis_gui.sh

DIR="$(cd "$(dirname "$0")/.." && pwd)"
cd "$DIR"

# macOS - open Terminal and run
if [[ "$OSTYPE" == "darwin"* ]]; then
    # Prefer iTerm if installed
    if [ -d "/Applications/iTerm.app" ] || [ -d "/Applications/iTerm2.app" ]; then
        /usr/bin/osascript <<EOF
tell application "iTerm"
    activate
    try
        set newWindow to (create window with default profile)
    on error
        tell current session of current window
        end tell
    end try
    tell the first session of current window
        write text "cd '$DIR' && ./jarvis"
    end tell
end tell
EOF
    else
        /usr/bin/osascript <<EOF
tell application "Terminal"
    activate
    do script "cd '$DIR' && ./jarvis"
    -- wait briefly for Terminal to create the window
    delay 1
    try
        tell application "Finder"
            set screen_bounds to bounds of window of desktop
        end tell
        set {x1, y1, x2, y2} to screen_bounds
        set bounds of front window to {0, 22, x2, y2}
    end try
end tell
EOF
    fi
    exit 0
fi

# GNOME Terminal
if command -v gnome-terminal >/dev/null 2>&1; then
    gnome-terminal -- bash -c "cd '$DIR' && ./jarvis; exec bash"
    exit 0
fi

# Konsole
if command -v konsole >/dev/null 2>&1; then
    konsole -e bash -c "cd '$DIR' && ./jarvis; exec bash" &
    exit 0
fi

# xterm
if command -v xterm >/dev/null 2>&1; then
    xterm -e "cd '$DIR' && ./jarvis" &
    exit 0
fi

# Fallback: run in current terminal
echo "No GUI terminal found - running in current terminal"
./jarvis
