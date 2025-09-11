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

#ifndef CG_PHYSICAL_DEVICE_H
#define CG_PHYSICAL_DEVICE_H 1
#include "cg_var.h"
#ifdef __linux
#include <dlfcn.h>
#endif // __linux

// 创建Vulkan物理设备列表
bool cg_enumerate_physical_device(cg_var_t *p_var, uint32_t *p_physical_device_count, VkPhysicalDevice *available_physical_device_array);

// 选择想要的Vulkan物理设备
bool cg_select_physical_device(cg_var_t *p_var, uint32_t *p_physical_device_count, VkPhysicalDevice *available_physical_device_array, VkPhysicalDevice *p_physical_device);

#endif // CG_PHYSICAL_DEVICE_H 1
