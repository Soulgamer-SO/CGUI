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

#ifndef CG_PHYSICAL_DEVICE_INFO_H
#define CG_PHYSICAL_DEVICE_INFO_H 1
#include <stdint.h>
#include <vulkan/vulkan.h>

typedef struct vk_physical_device_info_t {
    // 要被选择的物理设备(显卡)
    VkPhysicalDevice physical_device;
    // 获取可用的物理设备的数量
    uint32_t physical_device_count;
    // 获取可用的物理设备的句柄名单
    VkPhysicalDevice *available_physical_device_array;
    // 被选择的显卡的索引
    uint32_t physical_device_index;
    // 其他显卡的索引
    uint32_t other_physical_device_index;
    // 选择的其他显卡
    VkPhysicalDevice other_physical_device;
    // 可用的物理设备扩展的数量
    uint32_t physical_device_extensions_count;
    // 检查物理设备功能和属性，选择想要的物理设备
    bool is_physical_device_supported;
    VkPhysicalDeviceFeatures device_feature_array;
    VkPhysicalDeviceProperties device_properties;
    // 可用的物理设备扩展列表
    VkExtensionProperties *available_physical_device_extension_array;
    // 启用的物理设备扩展的数量
    uint32_t enabled_physical_device_extensions_count;
    char **enabled_physical_device_extension_array;
    // 获取物理设备内存属性
    VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
} vk_physical_device_info_t;

#endif // CG_PHYSICAL_DEVICE_INFO_H 1
