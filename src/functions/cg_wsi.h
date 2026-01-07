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

#ifndef CG_WSI_H
#define CG_WSI_H 1
#include "cg_var.h"
#ifdef __linux
#include <xcb/xcb_icccm.h>
#include <xcb/xcb_util.h>
#include <xcb/xproto.h>
#endif // __linux
#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

// 创建窗口
bool cg_create_window(cg_var_t *p_var);

#endif // CG_WSI_H 1
