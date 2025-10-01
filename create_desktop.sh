#!/bin/bash
cg_filename=cgui-app.desktop
echo "create desktop file cg_filename"
cg_dir_path=(pwd)
cg_exec_name=cgui-app
cg_exec_path=cg_dir_path/build/release/$cg_exec_name
cg_icon_path=cg_dir_path/content/Soul.ico
touch cg_filename
chmod +x cg_filename
cat >cg_filename <<EOF
[Desktop Entry]
Name=cg_exec_name
Comment=cg_exec_name
Exec=cg_exec_path
Icon=cg_icon_path
Terminal=false
Type=Application
Categories=Game;
EOF
