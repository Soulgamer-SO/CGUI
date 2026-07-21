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

#ifndef CG_INSTANCE_INFO_H
#define CG_INSTANCE_INFO_H 1
#include <stdint.h>
#include <vulkan/vulkan.h>

typedef struct vk_instance_info {
    // 创建Vulkan实例
    VkInstance vk_instance;
    // 获得Vulkan实例扩展数量
    uint32_t instance_extension_count;
    // 获得所有可用实例扩展的名单
    VkExtensionProperties *instance_extension_array;
    // 已启用的实例扩展数量
    uint32_t enabled_instance_extension_count;
    // 已启用的实例扩展的名单
    char **enabled_extension_name_array;
} vk_instance_info_t;

#endif // CG_INSTANCE_INFO_H 1
