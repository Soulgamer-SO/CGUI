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

#include "cg_initialize.h"
#include "cg_command_pool.h"
// #include "cg_destroy.h"
#include "cg_instance.h"
#include "cg_load_library.h"
#include "cg_logic_device.h"
#include "cg_physical_device.h"
// #include "cg_sync.h"
#include "cg_wsi.h"

bool cg_initialize_var(cg_info_t *p_info) {
	if (p_info->p_memory_pool == nullptr) {
		return false;
	}
	p_info->library.vk_result = VK_SUCCESS;
	p_info->instance.vk_instance = VK_NULL_HANDLE;
	p_info->physical_device.physical_device = VK_NULL_HANDLE;
	p_info->physical_device.enabled_physical_device_extensions_count = 2;
	p_info->physical_device.enabled_physical_device_extension_array = (char **)cg_alloc_memory(p_info->p_memory_pool, p_info->physical_device.enabled_physical_device_extensions_count * sizeof(char *));
	if (p_info->physical_device.enabled_physical_device_extension_array == nullptr) {
		PRINT_ERROR("alloc memory fail!\n");
		return false;
	}

	p_info->physical_device.enabled_physical_device_extension_array[1] = VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME;
	p_info->physical_device.enabled_physical_device_extension_array[0] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
	p_info->logic_device.vk_logic_device = VK_NULL_HANDLE;

	if (cg_load_library(p_info) == false) {
		PRINT_ERROR("load library fail!\n");
		return false;
	}
	if (cg_create_instance(p_info, &p_info->instance.vk_instance) == false) {
		PRINT_ERROR("create instance fail!\n");
		return false;
	}
	p_info->physical_device.physical_device_count = 0;
	if (cg_enumerate_physical_device(p_info, &p_info->physical_device.physical_device_count, nullptr) == false) {
		PRINT_ERROR("enumerate physical device fail!\n");
		return false;
	}
	p_info->physical_device.available_physical_device_array = (VkPhysicalDevice *)cg_alloc_memory(p_info->p_memory_pool, p_info->physical_device.physical_device_count * sizeof(VkPhysicalDevice));
	if (p_info->physical_device.available_physical_device_array == nullptr) {
		PRINT_ERROR("create available_handle_device_array fail!\n");
		return false;
	}

	if (cg_enumerate_physical_device(p_info, &p_info->physical_device.physical_device_count, &p_info->physical_device.available_physical_device_array[0]) == false) {
		PRINT_ERROR("enumerate physical device fail!\n");
		return false;
	}
	if (cg_select_physical_device(p_info, &p_info->physical_device.physical_device_count, &p_info->physical_device.available_physical_device_array[0], &p_info->physical_device.physical_device) == false) {
		PRINT_ERROR("select physical device fail!\n");
		return false;
	}
	if (cg_create_logic_device(p_info, &p_info->logic_device.vk_logic_device) == false) {
		PRINT_ERROR("create logic device fail!\n");
		return false;
	}

	PRINT_LOG("%s\n", (char *)p_info->physical_device.enabled_physical_device_extension_array[0]);
	PRINT_LOG("%s\n", (char *)p_info->physical_device.enabled_physical_device_extension_array[1]);

	// 创建命令缓存
	p_info->command_pool.command_pool = VK_NULL_HANDLE;
	p_info->command_pool.command_buffer_count = 8;
	p_info->command_pool.command_buffer_array = (VkCommandBuffer *)cg_alloc_memory(p_info->p_memory_pool, p_info->command_pool.command_buffer_count * sizeof(VkCommandBuffer));
	if (nullptr != p_info->command_pool.command_buffer_array) {
		PRINT_LOG("alloc memory success!\n");
		cg_create_command_pool(p_info, &p_info->command_pool.command_pool);
		if (p_info->command_pool.command_pool == VK_NULL_HANDLE) {
			return false;
		}
		cg_create_command_buffer_array(p_info, p_info->command_pool.command_pool, &p_info->command_pool.command_buffer_array[0], p_info->command_pool.command_buffer_count);
		cg_begin_record_command_buffer(p_info, p_info->command_pool.command_buffer_array[0], VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		cg_end_record_command_buffer(p_info, p_info->command_pool.command_buffer_array[0]);
		cg_reset_command_buffer(p_info, p_info->command_pool.command_buffer_array[0], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
		cg_reset_command_pool(p_info, p_info->command_pool.command_pool, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
	}

	/* p_info->sync.semaphore_count = 1;
	p_info->sync.semaphore_array = (VkSemaphore *)cg_alloc_memory(p_info->p_memory_pool,p_info->sync.semaphore_count * sizeof(VkSemaphore));
	if (nullptr != p_info->sync.semaphore_array)
	{
	    create_semaphore(p_info);
	}
	p_info->sync.fence_count = 1;
	p_info->sync.fence_array = (VkFence *)cg_alloc_memory(p_info->p_memory_pool,p_info->sync.fence_count * sizeof(VkFence));
	if (nullptr != p_info->sync.fence_array)
	{
	    create_fence(p_info);
	    wait_for_fences(p_info);
	    reset_for_fences(p_info);
	} */

	if (cg_create_window(p_info) == false) {
		PRINT_ERROR("create window fail!\n");
		return false;
	}

	/* p_info->sync.wait_semaphore_count = 1;
	p_info->sync.wait_semaphore_array = (VkSemaphore *)cg_alloc_memory(p_info->p_memory_pool,p_info->sync.wait_semaphore_count * sizeof(VkSemaphore));
	p_info->sync.semaphore_pipeline_stage_count = 1;
	p_info->sync.semaphore_pipeline_stage_array = (VkPipelineStageFlags *)cg_alloc_memory(p_info->p_memory_pool,p_info->sync.semaphore_pipeline_stage_count * sizeof(VkPipelineStageFlags));
	p_info->sync.semaphore_pipeline_stage_array[0] = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	if (nullptr != p_info->sync.semaphore_array &&
	    nullptr != p_info->sync.semaphore_pipeline_stage_array &&
	    nullptr != p_info->sync.fence_array)
	{
	    submit_command_buff_to_queue(p_info);
	} */

	PRINT_LOG("initialize success!\n");
	return true;
}
