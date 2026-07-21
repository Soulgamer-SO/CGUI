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

#ifndef CG_EVENT_LOOP_INFO_H
#define CG_EVENT_LOOP_INFO_H 1
#include <stdint.h>
#include <vulkan/vulkan.h>
#ifdef __linux
#include <xcb/xcb.h>
#endif // __linux

// event loop var
typedef struct event_loop_info {
    bool is_running;
#ifdef VK_USE_PLATFORM_XCB_KHR
    xcb_generic_event_t *event;
    // file descriptor 文件描述符
    int fd;
    // 鼠标坐标
    int mouse_x, mouse_y;
    // 鼠标左键单击次数
    int left_click_count;
    // 鼠标右键单击次数
    int right_click_count;
#endif // VK_USE_PLATFORM_XCB_KHR

#ifdef _WIN32
    MSG msg;
#endif // _WIN32
} event_loop_info_t;

#endif // CG_EVENT_LOOP_INFO_H 1
