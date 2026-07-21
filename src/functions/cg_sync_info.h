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

#ifndef CG_SYNC_INFO_H
#define CG_SYNC_INFO_H 1
#include <stdint.h>
#include <vulkan/vulkan.h>

typedef struct sync_info {
    // 信号量数量
    uint32_t semaphore_count;
    // 信号列表
    VkSemaphore *semaphore_array;
    // 信号名单列表
    char **semaphore_name_array;

    // 等待的信号量数量
    uint32_t wait_semaphore_count;
    // 等待的信号量列表
    VkSemaphore *wait_semaphore_array;

    // 围栏列表
    VkFence *fence_array;
    // 围栏的数量
    uint32_t fence_count;

    // 是否在等待
    bool is_wait_for;
    // 单位纳秒 1,000,000,000ns = 1s
    uint64_t timeout;

    // pipeline阶段的位掩码数量
    uint32_t semaphore_pipeline_stage_count;
    // pipeline阶段的位掩码列表
    VkPipelineStageFlags *semaphore_pipeline_stage_array;
} sync_info_t;

#endif // CG_SYNC_INFO_H 1
