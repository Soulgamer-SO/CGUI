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

#include "cg_command_pool.h"

bool cg_create_command_pool(cg_info_t *p_info, VkCommandPool *p_command_pool) {
	VkCommandPoolCreateInfo command_pool_create_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = p_info->logic_device.graphic_queue_family_index};

	PFN_vkCreateCommandPool create_command_pool = nullptr;
	create_command_pool = (PFN_vkCreateCommandPool)p_info->library.vk_get_device_proc_addr(p_info->logic_device.vk_logic_device, "vkCreateCommandPool");
	if (create_command_pool == nullptr) {
		PRINT_ERROR("load vkCreateCommandPool fail!\n");
		return false;
	}

	*p_command_pool = VK_NULL_HANDLE;
	p_info->library.vk_result = create_command_pool(
		p_info->logic_device.vk_logic_device, &command_pool_create_info,
		nullptr, p_command_pool);
	if (p_info->library.vk_result != VK_SUCCESS) {
		PRINT_ERROR("create command_pool fail!\n");
		return false;
	}

	return true;
}

bool cg_create_command_buffer_array(cg_info_t *p_info, VkCommandPool command_pool, VkCommandBuffer *command_buffer_array, uint32_t command_buffer_count) {
	if (command_buffer_array == nullptr) {
		PRINT_ERROR("create_command_buffer fail!\n");
		return false;
	}
	VkCommandBufferAllocateInfo command_buffer_allocate_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = nullptr,
		.commandPool = command_pool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = command_buffer_count};

	PFN_vkAllocateCommandBuffers allocate_command_buffers = nullptr;
	allocate_command_buffers = (PFN_vkAllocateCommandBuffers)p_var->library.vk_get_device_proc_addr(p_var->logic_device.vk_logic_device, "vkAllocateCommandBuffers");
	if (allocate_command_buffers == nullptr) {
		PRINT_ERROR("load vkAllocateCommandBuffers fail!\n");
		return false;
	}

	p_var->library.vk_result = allocate_command_buffers(p_var->logic_device.vk_logic_device, &command_buffer_allocate_info, command_buffer_array);
	if (p_var->library.vk_result != VK_SUCCESS) {
		PRINT_ERROR("vkAllocateCommandBuffers fail!\n");
		return false;
	}

	PRINT_LOG("create_command_buffer success!\n");
	return true;
}

bool cg_begin_record_command_buffer(cg_info_t *p_var, VkCommandBuffer command_buffer, VkCommandBufferUsageFlags command_buffer_usage) {
	// VkCommandBufferInheritanceInfo secondary_command_buffer_info;
	VkCommandBufferBeginInfo command_buffer_begin_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = nullptr,
		.flags = command_buffer_usage,
		.pInheritanceInfo = nullptr};

	PFN_vkBeginCommandBuffer vkBeginCommandBuffer = nullptr;
	vkBeginCommandBuffer = (PFN_vkBeginCommandBuffer)p_var->library.vk_get_instance_proc_addr(p_var->instance.vk_instance, "vkBeginCommandBuffer");
	if (vkBeginCommandBuffer == nullptr) {
		PRINT_ERROR("load vkBeginCommandBuffer fail!\n");
		return false;
	}

	p_var->library.vk_result = vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);
	if (p_var->library.vk_result != VK_SUCCESS) {
		PRINT_ERROR("vkBeginCommandBuffer fail!\n");
		return false;
	}

	return true;
}

bool cg_end_record_command_buffer(cg_info_t *p_var, VkCommandBuffer command_buffer) {
	PFN_vkEndCommandBuffer vkEndCommandBuffer = nullptr;
	vkEndCommandBuffer = (PFN_vkEndCommandBuffer)p_var->library.vk_get_instance_proc_addr(p_var->instance.vk_instance, "vkEndCommandBuffer");
	if (vkEndCommandBuffer == nullptr) {
		PRINT_ERROR("load vkEndCommandBuffer fail!\n");
		return false;
	}

	p_var->library.vk_result = vkEndCommandBuffer(command_buffer);
	if (p_var->library.vk_result != VK_SUCCESS) {
		PRINT_ERROR("vkEndCommandBuffer fail!\n");
		return false;
	}

	return true;
}

bool cg_reset_command_buffer(cg_info_t *p_var, VkCommandBuffer command_buffer, VkCommandBufferResetFlags command_buffer_reset_flag) {
	PFN_vkResetCommandBuffer vkResetCommandBuffer = nullptr;
	vkResetCommandBuffer = (PFN_vkResetCommandBuffer)p_var->library.vk_get_instance_proc_addr(p_var->instance.vk_instance, "vkResetCommandBuffer");
	if (vkResetCommandBuffer == nullptr) {
		PRINT_ERROR("load vkResetCommandBuffer fail!\n");
		return false;
	}

	vkResetCommandBuffer(command_buffer, command_buffer_reset_flag);
	if (p_var->library.vk_result != VK_SUCCESS) {
		PRINT_ERROR("vkResetCommandBuffer fail!\n");
		return false;
	}

	return true;
}

bool cg_reset_command_pool(cg_info_t *p_var, VkCommandPool command_pool, VkCommandBufferResetFlags command_buffer_reset_flag) {
	PFN_vkResetCommandPool vkResetCommandPool = nullptr;
	vkResetCommandPool = (PFN_vkResetCommandPool)p_var->library.vk_get_device_proc_addr(p_var->logic_device.vk_logic_device, "vkResetCommandPool");
	if (vkResetCommandPool == nullptr) {
		PRINT_ERROR("load vkResetCommandPool fail!\n");
		return false;
	}

	vkResetCommandPool(p_var->logic_device.vk_logic_device, command_pool, command_buffer_reset_flag);
	if (p_var->library.vk_result != VK_SUCCESS) {
		PRINT_ERROR("vkResetCommandPool fail!\n");
		return false;
	}

	return true;
}
