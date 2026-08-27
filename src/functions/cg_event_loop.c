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

#include "cg_event_loop.h"

static bool cg_draw_frame(cg_info_t *p_info) {
    VkDevice device = p_info->logic_device.vk_logic_device;
    VkQueue queue = p_info->logic_device.queue_family_handle;
    VkSwapchainKHR swapchain = p_info->wsi.swapchain;
    VkCommandBuffer command_buffer = p_info->command_pool.command_buffer_array[0];
    VkSemaphore image_available = p_info->sync.semaphore_array[0];
    VkSemaphore render_finished = p_info->sync.semaphore_array[1];
    VkFence in_flight = p_info->sync.fence_array[0];

    PFN_vkWaitForFences wait_for_fences = (PFN_vkWaitForFences)p_info->library.vk_get_device_proc_addr(device, "vkWaitForFences");
    PFN_vkResetFences reset_fences = (PFN_vkResetFences)p_info->library.vk_get_device_proc_addr(device, "vkResetFences");
    PFN_vkAcquireNextImageKHR acquire_next_image = (PFN_vkAcquireNextImageKHR)p_info->library.vk_get_device_proc_addr(device, "vkAcquireNextImageKHR");
    PFN_vkResetCommandBuffer reset_command_buffer = (PFN_vkResetCommandBuffer)p_info->library.vk_get_device_proc_addr(device, "vkResetCommandBuffer");
    PFN_vkBeginCommandBuffer begin_command_buffer = (PFN_vkBeginCommandBuffer)p_info->library.vk_get_device_proc_addr(device, "vkBeginCommandBuffer");
    PFN_vkEndCommandBuffer end_command_buffer = (PFN_vkEndCommandBuffer)p_info->library.vk_get_device_proc_addr(device, "vkEndCommandBuffer");
    PFN_vkQueueSubmit queue_submit = (PFN_vkQueueSubmit)p_info->library.vk_get_device_proc_addr(device, "vkQueueSubmit");
    PFN_vkQueuePresentKHR queue_present = (PFN_vkQueuePresentKHR)p_info->library.vk_get_device_proc_addr(device, "vkQueuePresentKHR");

    if (wait_for_fences == nullptr || reset_fences == nullptr ||
        acquire_next_image == nullptr || reset_command_buffer == nullptr ||
        begin_command_buffer == nullptr || end_command_buffer == nullptr ||
        queue_submit == nullptr || queue_present == nullptr) {
        PRINT_ERROR("load frame functions fail!\n");
        return false;
    }

    VkResult result = wait_for_fences(device, 1, &in_flight, VK_TRUE, UINT64_MAX);
    if (result != VK_SUCCESS) {
        PRINT_ERROR("wait frame fence fail!\n");
        return false;
    }

    uint32_t image_index = 0;
    result = acquire_next_image(device, swapchain, UINT64_MAX, image_available, VK_NULL_HANDLE, &image_index);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        return true;
    }
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        PRINT_ERROR("acquire swapchain image fail!\n");
        return false;
    }

    result = reset_command_buffer(command_buffer, 0);
    if (result != VK_SUCCESS) {
        PRINT_ERROR("reset command buffer fail!\n");
        return false;
    }
    result = begin_command_buffer(command_buffer, &(VkCommandBufferBeginInfo){
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr});
    if (result != VK_SUCCESS) {
        PRINT_ERROR("begin command buffer fail!\n");
        return false;
    }
    result = end_command_buffer(command_buffer);
    if (result != VK_SUCCESS) {
        PRINT_ERROR("end command buffer fail!\n");
        return false;
    }

    result = reset_fences(device, 1, &in_flight);
    if (result != VK_SUCCESS) {
        PRINT_ERROR("reset frame fence fail!\n");
        return false;
    }

    VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &image_available,
        .pWaitDstStageMask = &wait_stage,
        .commandBufferCount = 1,
        .pCommandBuffers = &command_buffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &render_finished};
    result = queue_submit(queue, 1, &submit_info, in_flight);
    if (result != VK_SUCCESS) {
        PRINT_ERROR("submit frame fail!\n");
        return false;
    }

    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &render_finished,
        .swapchainCount = 1,
        .pSwapchains = &swapchain,
        .pImageIndices = &image_index,
        .pResults = nullptr};
    result = queue_present(queue, &present_info);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        return true;
    }
    if (result != VK_SUCCESS) {
        PRINT_ERROR("present frame fail!\n");
        return false;
    }

    return true;
}

#ifdef LINUX
#include <xcb/xcb_event.h>

void cg_event_loop(cg_info_t *p_info) {
    xcb_connection_t *connection = p_info->wsi.xcb_surface_create_info.connection;
    xcb_window_t window = p_info->wsi.xcb_surface_create_info.window;

    p_info->event_loop.is_running = true;
    p_info->event_loop.fd = xcb_get_file_descriptor(connection);

    while (p_info->event_loop.is_running) {
        while ((p_info->event_loop.event = xcb_poll_for_event(connection)) != nullptr) {
            switch (XCB_EVENT_RESPONSE_TYPE(p_info->event_loop.event)) {
            case XCB_EXPOSE: {
                xcb_expose_event_t *expose_event = (xcb_expose_event_t *)p_info->event_loop.event;
                if (expose_event->window == window) {
                    PRINT_LOG(
                        "expose_event x = %i; expose_event y = %i; expose_event width = %i; expose_event height = %i;\n",
                        expose_event->x,
                        expose_event->y,
                        expose_event->width,
                        expose_event->height);
                }
                break;
            }
            case XCB_CLIENT_MESSAGE:
                if (((xcb_client_message_event_t *)p_info->event_loop.event)->window == window &&
                    ((xcb_client_message_event_t *)p_info->event_loop.event)->data.data32[0] == p_info->wsi.XCB_API_info.wm_delete_window_atom) {
                    p_info->event_loop.is_running = false;
                }
                break;
            case XCB_DESTROY_NOTIFY:
                if (((xcb_destroy_notify_event_t *)p_info->event_loop.event)->window == window) {
                    p_info->event_loop.is_running = false;
                }
                break;
            default:
                break;
            }

            free(p_info->event_loop.event);
            p_info->event_loop.event = nullptr;
        }

        if (xcb_connection_has_error(connection) != 0) {
            p_info->event_loop.is_running = false;
        }

        if (!cg_draw_frame(p_info)) {
            p_info->event_loop.is_running = false;
        }
    }

    return;
}
#endif // LINUX

#ifdef WINDOWS
#include "cg_input.h"

void cg_event_loop(cg_info_t *p_info) {
    p_info->event_loop.is_running = true;
    while (p_info->event_loop.is_running) {
        while (PeekMessage(&p_info->event_loop.msg, nullptr, 0, 0, PM_REMOVE) != false) {
            if (p_info->event_loop.msg.message == WM_QUIT) {
                p_info->event_loop.is_running = false;
                break;
            }

            TranslateMessage(&p_info->event_loop.msg);
            DispatchMessage(&p_info->event_loop.msg);
        }

        if (!cg_draw_frame(p_info)) {
            p_info->event_loop.is_running = false;
        }
    }

    return;
}

LRESULT CALLBACK window_proc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
    case WM_KEYDOWN:
        if (wparam == KEY_Q &&
            (GetKeyState(VK_CONTROL) & 0x8000) != 0) {
            DestroyWindow(wnd);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(wnd, msg, wparam, lparam);
    }

    return 0;
}
#endif // WINDOWS
