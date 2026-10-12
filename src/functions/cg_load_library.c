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

#include "cg_load_library.h"

bool cg_load_library(cg_info_t *p_info) {
#ifdef LINUX
    p_info->library.vulkan_library = nullptr;
    p_info->library.vulkan_library = dlopen("libvulkan.so.1", RTLD_NOW);
    if (p_info->library.vulkan_library == nullptr) {
        PRINT_ERROR("load libvulkan.so.1 fail!\n");
        return false;
    } else {
        PRINT_LOG("load vulkan.so.1 success!\n");
    }

    // 加载实例函数的函数 vkGetInstanceProcAddr
    p_info->library.vk_get_instance_proc_addr = nullptr;
    p_info->library.vk_get_instance_proc_addr = (PFN_vkGetInstanceProcAddr)dlsym(p_info->library.vulkan_library, "vkGetInstanceProcAddr");
    if (p_info->library.vk_get_instance_proc_addr == nullptr) {
        PRINT_ERROR("load vkGetInstanceProcAddr fail!\n");
        return false;
    }

    // 加载设备函数的函数 vkGetDeviceProcAddr
    p_info->library.vk_get_device_proc_addr = nullptr;
    p_info->library.vk_get_device_proc_addr = (PFN_vkGetDeviceProcAddr)dlsym(p_info->library.vulkan_library, "vkGetDeviceProcAddr");
    if (p_info->library.vk_get_device_proc_addr == nullptr) {
        PRINT_ERROR("load vkGetDeviceProcAddr fail!\n");
        return false;
    }
#endif // LINUX

#ifdef WINDOWS
    p_info->library.vulkan_library = nullptr;
    p_info->library.vulkan_library = LoadLibrary("vulkan-1.dll");
    if (p_info->library.vulkan_library == nullptr) {
        PRINT_ERROR("load vulkan-1.dll fail!\n");
        return false;
    } else {
        PRINT_LOG("load vulkan-1.dll success!\n");
    }

    // 加载实例函数的函数 vkGetInstanceProcAddr
    p_info->library.vk_get_instance_proc_addr = nullptr;
    p_info->library.vk_get_instance_proc_addr = (PFN_vkGetInstanceProcAddr)GetProcAddress(p_info->library.vulkan_library, "vkGetInstanceProcAddr");
    if (p_info->library.vk_get_instance_proc_addr == nullptr) {
        PRINT_ERROR("load vkGetInstanceProcAddr fail!\n");
        return false;
    }

    // 加载设备函数的函数 vkGetDeviceProcAddr
    p_info->library.vk_get_device_proc_addr = nullptr;
    p_info->library.vk_get_device_proc_addr = (PFN_vkGetDeviceProcAddr)GetProcAddress(p_info->library.vulkan_library, "vkGetDeviceProcAddr");
    if (p_info->library.vk_get_device_proc_addr == nullptr) {
        PRINT_ERROR("load vkGetDeviceProcAddr fail!\n");
        return false;
    }
#endif // WINDOWS

    return true;
}

bool cg_load_vk_functions(cg_info_t *p_info) {
    if (p_info->logic_device.vk_logic_device == VK_NULL_HANDLE || p_info->library.vk_get_device_proc_addr == nullptr) {
        return false;
    }

    VkDevice device = p_info->logic_device.vk_logic_device;
    p_info->library.vk_wait_for_fences = (PFN_vkWaitForFences)p_info->library.vk_get_device_proc_addr(device, "vkWaitForFences");
    p_info->library.vk_reset_fences = (PFN_vkResetFences)p_info->library.vk_get_device_proc_addr(device, "vkResetFences");
    p_info->library.vk_acquire_next_image = (PFN_vkAcquireNextImageKHR)p_info->library.vk_get_device_proc_addr(device, "vkAcquireNextImageKHR");
    p_info->library.vk_reset_command_buffer = (PFN_vkResetCommandBuffer)p_info->library.vk_get_device_proc_addr(device, "vkResetCommandBuffer");
    p_info->library.vk_begin_command_buffer = (PFN_vkBeginCommandBuffer)p_info->library.vk_get_device_proc_addr(device, "vkBeginCommandBuffer");
    p_info->library.vk_end_command_buffer = (PFN_vkEndCommandBuffer)p_info->library.vk_get_device_proc_addr(device, "vkEndCommandBuffer");
    p_info->library.vk_cmd_begin_render_pass = (PFN_vkCmdBeginRenderPass)p_info->library.vk_get_device_proc_addr(device, "vkCmdBeginRenderPass");
    p_info->library.vk_cmd_end_render_pass = (PFN_vkCmdEndRenderPass)p_info->library.vk_get_device_proc_addr(device, "vkCmdEndRenderPass");
    p_info->library.vk_cmd_bind_pipeline = (PFN_vkCmdBindPipeline)p_info->library.vk_get_device_proc_addr(device, "vkCmdBindPipeline");
    p_info->library.vk_cmd_set_viewport = (PFN_vkCmdSetViewport)p_info->library.vk_get_device_proc_addr(device, "vkCmdSetViewport");
    p_info->library.vk_cmd_set_scissor = (PFN_vkCmdSetScissor)p_info->library.vk_get_device_proc_addr(device, "vkCmdSetScissor");
    p_info->library.vk_cmd_draw = (PFN_vkCmdDraw)p_info->library.vk_get_device_proc_addr(device, "vkCmdDraw");
    p_info->library.vk_queue_submit = (PFN_vkQueueSubmit)p_info->library.vk_get_device_proc_addr(device, "vkQueueSubmit");
    p_info->library.vk_queue_present = (PFN_vkQueuePresentKHR)p_info->library.vk_get_device_proc_addr(device, "vkQueuePresentKHR");

    if (p_info->library.vk_wait_for_fences == nullptr || p_info->library.vk_reset_fences == nullptr ||
        p_info->library.vk_acquire_next_image == nullptr || p_info->library.vk_reset_command_buffer == nullptr ||
        p_info->library.vk_begin_command_buffer == nullptr || p_info->library.vk_end_command_buffer == nullptr ||
        p_info->library.vk_cmd_begin_render_pass == nullptr || p_info->library.vk_cmd_end_render_pass == nullptr ||
        p_info->library.vk_cmd_bind_pipeline == nullptr || p_info->library.vk_cmd_set_viewport == nullptr ||
        p_info->library.vk_cmd_set_scissor == nullptr || p_info->library.vk_cmd_draw == nullptr ||
        p_info->library.vk_queue_submit == nullptr || p_info->library.vk_queue_present == nullptr) {
        return false;
    }

    return true;
}
