#!/usr/bin/env bash
set -Eeuo pipefail

desktop_file="$HOME/.local/share/applications/cgui-app.desktop"

if [[ -f "$desktop_file" ]]; then
    rm -f "$desktop_file"
    echo "Removed user desktop entry: $desktop_file"
else
    echo "Not found: $desktop_file"
fi
