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

#include "cg_sync.h"

bool cg_create_semaphore(cg_info_t *p_info, VkSemaphore *p_semaphore) {
    if (p_semaphore == nullptr) {
        return false;
    }
    PFN_vkCreateSemaphore create_semaphore = nullptr;
    create_semaphore = (PFN_vkCreateSemaphore)p_info->library.vk_get_device_proc_addr(p_info->logic_device.vk_logic_device, "vkCreateSemaphore");
    if (create_semaphore == nullptr) {
        PRINT_ERROR("load vkCreateSemaphore fail!\n");
        return false;
    }

    VkSemaphoreCreateInfo semaphore_create_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0};
    p_info->library.vk_result = create_semaphore(p_info->logic_device.vk_logic_device, &semaphore_create_info, nullptr, p_semaphore);
    if (p_info->library.vk_result != VK_SUCCESS) {
        PRINT_ERROR("vkCreateSemaphore fail!\n");
        return false;
    } else {
        PRINT_LOG("semaphore address = %p;\n", &p_info->sync.semaphore_array[0]);
    }

    return true;
}

bool cg_create_fence(cg_info_t *p_info, VkFence *p_fence) {
    if (p_fence == nullptr) {
        return false;
    }
    PFN_vkCreateFence vkCreateFence = nullptr;
    vkCreateFence = (PFN_vkCreateFence)p_info->library.vk_get_device_proc_addr(p_info->logic_device.vk_logic_device, "vkCreateFence");
    if (vkCreateFence == nullptr) {
        PRINT_ERROR("load vkCreateFence fail!\n");
        return false;
    }

    VkFenceCreateInfo fence_create_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT}; // flags的值为 VK_FENCE_CREATE_SIGNALED_BIT 1或者0
    p_info->library.vk_result = vkCreateFence(
        p_info->logic_device.vk_logic_device, &fence_create_info,
        nullptr, p_fence);
    if (p_info->library.vk_result != VK_SUCCESS) {
        PRINT_ERROR("vkCreateFence fail!\n");
        return false;
    } else {
        PRINT_LOG("fence address = %p;\n", &p_info->sync.fence_array[0]);
    }

    return true;
}
