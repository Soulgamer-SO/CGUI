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

#ifndef CG_LOGIC_DEVICE_INFO_H
#define CG_LOGIC_DEVICE_INFO_H 1
#include <stdint.h>
#include <vulkan/vulkan.h>

typedef struct vk_logic_device_info {
    // 获取队列家族和它们的属性,选择想要的队列家族
    uint32_t queue_family_count;
    VkQueueFamilyProperties *queue_family_array;
    VkQueueFamilyProperties *queue_family_property_array;

    // 选择想要的队列家族并返回其索引
    uint32_t queue_family_index;
    // 支持有关图形操作的队列家族的索引
    uint32_t graphic_queue_family_index;
    // 队列的句柄
    VkQueue queue_family_handle;
    // 优先级列表
    uint32_t queue_priority_array_count;
    uint32_t queue_priority_array_index;
    float *queue_priority_array;

    // 创建Vulkan逻辑设备
    VkDevice vk_logic_device;
} vk_logic_device_info_t;

#endif // CG_LOGIC_DEVICE_INFO_H 1
