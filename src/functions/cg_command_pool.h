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

#ifndef CG_COMMAND_POOL_H
#define CG_COMMAND_POOL_H 1
#include "cg_info.h"

// 创建命令池
bool cg_create_command_pool(cg_info_t *p_info, VkCommandPool *p_command_pool);

// 分配命令缓冲区
bool cg_create_command_buffer_array(cg_info_t *p_info, VkCommandPool command_pool, VkCommandBuffer *command_buffer_array, uint32_t command_buffer_count);

#endif // CG_COMMAND_POOL_H 1
