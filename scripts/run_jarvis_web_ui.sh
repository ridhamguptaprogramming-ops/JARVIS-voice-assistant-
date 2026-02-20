#!/usr/bin/env bash
set -euo pipefail

DIR="$(cd "$(dirname "$0")/.." && pwd)"
UI_FILE="$DIR/web/index.html"

if [[ ! -f "$UI_FILE" ]]; then
    echo "Web UI not found: $UI_FILE"
    exit 1
fi

if [[ "${OSTYPE:-}" == darwin* ]]; then
    open "$UI_FILE"
    exit 0
fi

if command -v xdg-open >/dev/null 2>&1; then
    xdg-open "$UI_FILE" >/dev/null 2>&1 &
    exit 0
fi

if command -v sensible-browser >/dev/null 2>&1; then
    sensible-browser "$UI_FILE" >/dev/null 2>&1 &
    exit 0
fi

echo "Open this file in your browser: $UI_FILE"
