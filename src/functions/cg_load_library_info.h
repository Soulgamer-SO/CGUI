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

#ifndef CG_LOAD_LIBRARY_INFO_H
#define CG_LOAD_LIBRARY_INFO_H 1
#include <vulkan/vulkan.h>

typedef struct library_info {
    // Vulkan命令返回代码 Vulkan command return codes
    VkResult vk_result;
    // Linux下加载Vulkan函数库
#ifdef LINUX
    void *vulkan_library;
#endif // LINUX

// Windows下加载Vulkan函数库
#ifdef WINDOWS
    HMODULE vulkan_library;
#endif // WINDOWS

    // 加载实例函数的函数 PFN_vkGetInstanceProcAddr
    PFN_vkGetInstanceProcAddr vk_get_instance_proc_addr;
    // 加载逻辑设备函数的函数 PFN_vkGetDeviceProcAddr
    PFN_vkGetDeviceProcAddr vk_get_device_proc_addr;
} library_info_t;

#endif // CG_LOAD_LIBRARY_INFO_H 1
