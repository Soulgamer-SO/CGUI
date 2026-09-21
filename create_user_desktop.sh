#!/usr/bin/env bash
set -Eeuo pipefail

exec_name="cgui-app"
desktop_name="${exec_name}.desktop"
script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)"

# Resolve the executable from the most likely locations.
resolved_exec=""
for candidate in \
  "$script_dir/bin/$exec_name" \
  "$script_dir/build/release/$exec_name" \
  "$script_dir/build/debug/$exec_name"; do
  if [[ -x "$candidate" ]]; then
    resolved_exec="$candidate"
    break
  fi
done

icon_path="$script_dir/content/Soul.ico"
local_app_dir="$HOME/.local/share/applications"
project_desktop_path="$script_dir/$desktop_name"
installed_desktop_path="$local_app_dir/$desktop_name"

if [[ -z "$resolved_exec" ]]; then
  echo "Error: executable '$exec_name' not found in ./bin, ./build/release, or ./build/debug." >&2
  echo "Please run 'make release' or 'make install' first." >&2
  exit 1
fi

if [[ ! -f "$icon_path" ]]; then
  echo "Error: icon file not found: $icon_path" >&2
  exit 1
fi

mkdir -p "$local_app_dir"

cat >"$project_desktop_path" <<EOF
[Desktop Entry]
Version=1.0
Type=Application
Name=$exec_name
Comment=$exec_name
Exec="$resolved_exec"
Path=$script_dir
Icon=$icon_path
Terminal=false
Categories=Game;
StartupNotify=true
EOF

cp -f "$project_desktop_path" "$installed_desktop_path"
chmod 755 "$project_desktop_path" "$installed_desktop_path"

# Refresh icon cache for desktop entries when available.
if command -v gtk-update-icon-cache >/dev/null 2>&1; then
  gtk-update-icon-cache -f "$script_dir/content" >/dev/null 2>&1 || true
fi

echo "Project desktop entry created: $project_desktop_path"
echo "User desktop entry installed: $installed_desktop_path"
echo "Done."
