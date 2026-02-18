#!/usr/bin/env bash
set -euo pipefail

# JARVIS automated demo-test:
# - runs Jarvis non-interactively
# - creates project/folders/files automatically
# - validates expected outputs and artifacts

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$ROOT_DIR"

DEMO_PROJECT="jarvis_auto_demo"
LOG_DIR="build"
LOG_FILE="${LOG_DIR}/jarvis_demo_test.log"

mkdir -p "$LOG_DIR"

echo "========================================="
echo "JARVIS Automated Demo-Test"
echo "========================================="
echo ""

echo "[1/4] Building JARVIS..."
make >/dev/null

echo "[2/4] Preparing clean demo workspace..."
rm -rf "$DEMO_PROJECT"
rm -f ".jarvis_last_project" "$LOG_FILE"

echo "[3/4] Running automated Jarvis scenario..."
{
    echo "hello jarvis"
    echo "make a project jarvis auto demo in python language"
    echo "open last project"
    echo "create folder src/api in project jarvis_auto_demo"
    echo "create file src/api/routes.py in project jarvis_auto_demo with template"
    echo "open file jarvis_auto_demo/main.py"
    echo "search karo instagram"
    echo "set mode developer"
    echo "help"
    echo "quit"
} | env \
    JARVIS_NO_GUI=1 \
    JARVIS_DISABLE_UI=1 \
    JARVIS_ENABLE_NOTIFICATIONS=0 \
    JARVIS_LISTEN_TIMEOUT=1 \
    JARVIS_LISTEN_RETRIES=1 \
    JARVIS_PHRASE_LIMIT=2 \
    JARVIS_NOISE_CALIBRATION=0.2 \
    ./jarvis > "$LOG_FILE" 2>&1

echo "[4/4] Validating demo results..."
fail_count=0

assert_file_exists() {
    local path="$1"
    if [ -f "$path" ]; then
        echo "  [PASS] file exists: $path"
    else
        echo "  [FAIL] missing file: $path"
        fail_count=$((fail_count + 1))
    fi
}

assert_dir_exists() {
    local path="$1"
    if [ -d "$path" ]; then
        echo "  [PASS] folder exists: $path"
    else
        echo "  [FAIL] missing folder: $path"
        fail_count=$((fail_count + 1))
    fi
}

assert_log_contains() {
    local needle="$1"
    local label="$2"
    if grep -Fq "$needle" "$LOG_FILE"; then
        echo "  [PASS] $label"
    else
        echo "  [FAIL] $label"
        fail_count=$((fail_count + 1))
    fi
}

assert_dir_exists "$DEMO_PROJECT"
assert_file_exists "$DEMO_PROJECT/main.py"
assert_dir_exists "$DEMO_PROJECT/src/api"
assert_file_exists "$DEMO_PROJECT/src/api/routes.py"

assert_log_contains "Created python project 'jarvis_auto_demo'" "project creation response"
assert_log_contains "Opening your last project 'jarvis_auto_demo'" "open last project response"
assert_log_contains "Created folder jarvis_auto_demo/src/api." "folder creation response"
assert_log_contains "Created jarvis_auto_demo/src/api/routes.py with starter code." "template file creation response"
assert_log_contains "Opening file jarvis_auto_demo/main.py in VS Code." "open file response"
assert_log_contains "Web search initiated for: 'instagram'" "search normalization response"
assert_log_contains "I have switched to developer mode." "mode switch response"

echo ""
echo "Key demo log lines:"
grep -E "Processing:|Created python project|Opening your last project|Created folder|Created jarvis_auto_demo/src/api/routes.py|Web search initiated|switched to developer mode|Shutting down" "$LOG_FILE" || true

echo ""
if [ "$fail_count" -eq 0 ]; then
    echo "========================================="
    echo "Demo-Test Status: PASS"
    echo "Log: $LOG_FILE"
    echo "========================================="
    exit 0
fi

echo "========================================="
echo "Demo-Test Status: FAIL ($fail_count check(s) failed)"
echo "Log: $LOG_FILE"
echo "========================================="
exit 1
