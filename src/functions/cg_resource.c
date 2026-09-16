/*
Copyright (C) 2025  Soulgamer <SOsoulgamer@outlook.com>.

This file is part of CGUI.

CGUI is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

CGUI is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "cg_resource.h"
#include <stdio.h>
#include <string.h>
#ifdef WINDOWS
#include <windows.h>
#endif // WINDOWS
#ifdef LINUX
#include <unistd.h>
#endif // LINUX

bool cg_get_resource_path(const char *resource_name, char *path, size_t path_size) {
    char executable_path[CG_RESOURCE_PATH_MAX];
    size_t executable_path_length = 0;

#ifdef LINUX
    ssize_t length = readlink("/proc/self/exe", executable_path, sizeof(executable_path) - 1);
    if (length <= 0 || (size_t)length >= sizeof(executable_path)) {
        return false;
    }
    executable_path[length] = '\0';
    executable_path_length = (size_t)length;
#endif // LINUX

#ifdef WINDOWS
    DWORD length = GetModuleFileNameA(nullptr, executable_path, sizeof(executable_path));
    if (length == 0 || length >= sizeof(executable_path)) {
        return false;
    }
    executable_path_length = length;
#endif // WINDOWS

    char *last_separator = strrchr(executable_path, '\\');
    char *last_slash = strrchr(executable_path, '/');
    if (last_slash > last_separator) {
        last_separator = last_slash;
    }
    if (last_separator == nullptr) {
        return false;
    }
    *last_separator = '\0';
    executable_path_length = (size_t)(last_separator - executable_path);
    const char *resource_roots[] = {
        "%.*s/content/%s",
        "%.*s/../content/%s",
        "%.*s/../../content/%s"};
    for (size_t i = 0; i < sizeof(resource_roots) / sizeof(resource_roots[0]); i++) {
        int written = snprintf(path, path_size, resource_roots[i], (int)executable_path_length, executable_path, resource_name);
        if (written <= 0 || (size_t)written >= path_size) {
            continue;
        }
        FILE *resource_file = fopen(path, "rb");
        if (resource_file != nullptr) {
            fclose(resource_file);
            return true;
        }
    }
    return false;
}