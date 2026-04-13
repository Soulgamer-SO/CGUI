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

void cg_create_semaphore(cg_info_t *p_info) {
	PFN_vkCreateSemaphore create_semaphore = nullptr;
	create_semaphore = (PFN_vkCreateSemaphore)p_info->library.vk_get_device_proc_addr(p_info->logic_device.vk_logic_device, "vkCreateSemaphore");
	if (create_semaphore == nullptr) {
		PRINT_ERROR("load vkCreateSemaphore fail!\n");
		return;
	}

	VkSemaphoreCreateInfo semaphore_create_info = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0};
	p_info->library.vk_result = create_semaphore(p_info->logic_device.vk_logic_device, &semaphore_create_info, nullptr, &p_info->sync.semaphore_array[0]);
	if (p_info->library.vk_result != VK_SUCCESS) {
		PRINT_ERROR("vkCreateSemaphore fail!\n");
		return;
	} else {
		PRINT_LOG("semaphore address = %p;\n", &p_info->sync.semaphore_array[0]);
	}

	return;
}

void cg_create_fence(cg_info_t *p_info) {
	PFN_vkCreateFence vkCreateFence = nullptr;
	vkCreateFence = (PFN_vkCreateFence)p_info->library.vk_get_device_proc_addr(p_info->logic_device.vk_logic_device, "vkCreateFence");
	if (vkCreateFence == nullptr) {
		PRINT_ERROR("load vkCreateFence fail!\n");
		return;
	}

	VkFenceCreateInfo fence_create_info = {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.pNext = nullptr,
		.flags = VK_FENCE_CREATE_SIGNALED_BIT}; // flags的值为 VK_FENCE_CREATE_SIGNALED_BIT 1或者0
	p_info->library.vk_result = vkCreateFence(
		p_info->logic_device.vk_logic_device, &fence_create_info,
		nullptr, &p_info->sync.fence_array[0]);
	if (p_info->library.vk_result != VK_SUCCESS) {
		PRINT_ERROR("vkCreateFence fail!\n");
		return;
	} else {
		PRINT_LOG("fence address = %p;\n", &p_info->sync.fence_array[0]);
	}

	return;
}

void cg_wait_for_fences(cg_info_t *p_info) {
	PFN_vkWaitForFences vkWaitForFences = nullptr;
	vkWaitForFences = (PFN_vkWaitForFences)p_info->library.vk_get_device_proc_addr(p_info->logic_device.vk_logic_device, "vkWaitForFences");
	if (vkWaitForFences == nullptr) {
		PRINT_ERROR("load vkWaitForFences fail!\n");
		return;
	}
	p_info->sync.is_wait_for = VK_TRUE;
	p_info->sync.timeout = 1000000000;
	if (p_info->sync.fence_count > 0) {
		p_info->library.vk_result = vkWaitForFences(
			p_info->logic_device.vk_logic_device, p_info->sync.fence_count,
			p_info->sync.fence_array, p_info->sync.is_wait_for, p_info->sync.timeout);
		if (p_info->library.vk_result != VK_SUCCESS) {
			PRINT_ERROR("vkWaitForFences fail!\n");
			return;
		}
	}

	return;
}

void cg_reset_for_fences(cg_info_t *p_info) {
	PFN_vkResetFences vkResetFences = nullptr;
	vkResetFences = (PFN_vkResetFences)p_info->library.vk_get_device_proc_addr(p_info->logic_device.vk_logic_device, "vkResetFences");
	if (nullptr == vkResetFences) {
		PRINT_ERROR("load vkResetFences fail!\n");
		return;
	}
	p_info->library.vk_result = vkResetFences(
		p_info->logic_device.vk_logic_device,
		p_info->sync.fence_count, &p_info->sync.fence_array[0]);
	if (p_info->library.vk_result != VK_SUCCESS) {
		PRINT_ERROR("vkResetFences fail!\n");
		return;
	}

	return;
}

void cg_submit_command_buff_to_queue(cg_info_t *p_info) {

	VkSubmitInfo submit_info = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = nullptr,
		.waitSemaphoreCount = 0,      // p_info->sync.wait_semaphore_count,
		.pWaitSemaphores = nullptr,   //&p_info->sync.wait_semaphore_array[0],
		.pWaitDstStageMask = nullptr, //&p_info->sync.semaphore_pipeline_stage_array[0],
		.commandBufferCount = p_info->command_pool.command_buffer_count,
		.pCommandBuffers = &p_info->command_pool.command_buffer_array[0],
		.signalSemaphoreCount = 0,   // p_info->sync.semaphore_count,
		.pSignalSemaphores = nullptr //&p_info->sync.semaphore_array[0]
	};
	PFN_vkQueueSubmit vkQueueSubmit = nullptr;
	vkQueueSubmit = (PFN_vkQueueSubmit)p_info->library.vk_get_device_proc_addr(
		p_info->logic_device.vk_logic_device, "vkQueueSubmit");
	if (vkQueueSubmit == nullptr) {
		PRINT_ERROR("load vkQueueSubmit fail!\n");
		return;
	}

	p_info->library.vk_result = vkQueueSubmit(
		p_info->logic_device.queue_family_handle,
		0, &submit_info,
		VK_NULL_HANDLE /* p_info->sync.fence_array[0] */);
	if (p_info->library.vk_result != VK_SUCCESS) {
		PRINT_ERROR("vkQueueSubmit fail!\n");
		return;
	}

	return;
}
