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

#ifndef CG_COMMAND_POOL_INFO_H
#define CG_COMMAND_POOL_INFO_H 1
#include <stdint.h>
#include <vulkan/vulkan.h>

typedef struct vk_command_pool_info {
    // 命令池
    VkCommandPool command_pool;
    // 命令缓存列表
    uint32_t command_buffer_count;
    VkCommandBuffer *command_buffer_array;
} vk_command_pool_info_t;

#endif // CG_COMMAND_POOL_INFO_H 1
