#include "cg_initialize.h"
#include "cg_command_pool.h"
#include "cg_destroy.h"
#include "cg_instance.h"
#include "cg_load_library.h"
#include "cg_logic_device.h"
#include "cg_physical_device.h"
#include "cg_sync.h"
#include "cg_wsi.h"

bool_t cg_initialize_var(cg_var_t *p_var) {
	if (p_var->p_memory_pool_var == NULL) {
		return FALSE;
	}
	p_var->library_var.vk_result = VK_SUCCESS;
	p_var->instance_var.vk_instance = VK_NULL_HANDLE;
	p_var->physical_device_var.physical_device = VK_NULL_HANDLE;
	p_var->physical_device_var.enabled_physical_device_extensions_count = 2;
	p_var->physical_device_var.enabled_physcial_device_extension_list = (char **)cg_alloc_memory(p_var->p_memory_pool_var, p_var->physical_device_var.enabled_physical_device_extensions_count * sizeof(char *));
	if (p_var->physical_device_var.enabled_physcial_device_extension_list == NULL) {
		return FALSE;
	} else {
		PRINT_LOG("alloc memory success!\n");
	}

	p_var->physical_device_var.enabled_physcial_device_extension_list[0] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
	p_var->physical_device_var.enabled_physcial_device_extension_list[1] = VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME;
	p_var->logic_device_var.vk_logic_device = VK_NULL_HANDLE;

	if (cg_load_library(p_var) == FALSE) {
		return FALSE;
	}
	if (cg_create_instance(p_var, &p_var->instance_var.vk_instance) == FALSE) {
		return FALSE;
	}
	p_var->physical_device_var.physical_device_count = 0;
	if (cg_enumerate_physical_device(p_var, &p_var->physical_device_var.physical_device_count, NULL) == FALSE) {
		return FALSE;
	}
	p_var->physical_device_var.available_physical_device_list = (VkPhysicalDevice *)cg_alloc_memory(p_var->p_memory_pool_var, p_var->physical_device_var.physical_device_count * sizeof(VkPhysicalDevice));
	if (p_var->physical_device_var.available_physical_device_list == NULL) {
		PRINT_ERROR("create available_handle_device_list fail!\n");
		return FALSE;
	} else {
		PRINT_LOG("alloc memory success!\n");
	}

	if (cg_enumerate_physical_device(p_var, &p_var->physical_device_var.physical_device_count, &p_var->physical_device_var.available_physical_device_list[0]) == FALSE) {
		return FALSE;
	}
	if (cg_select_physical_device(p_var, &p_var->physical_device_var.physical_device_count, &p_var->physical_device_var.available_physical_device_list[0], &p_var->physical_device_var.physical_device) == FALSE) {
		return FALSE;
	}
	if (cg_create_logic_device(p_var, &p_var->logic_device_var.vk_logic_device) == FALSE) {
		return FALSE;
	}

	PRINT_LOG("%s\n", (char *)p_var->physical_device_var.enabled_physcial_device_extension_list[0]);
	PRINT_LOG("%s\n", (char *)p_var->physical_device_var.enabled_physcial_device_extension_list[1]);

	// 创建命令缓存
	p_var->command_pool_var.command_pool = VK_NULL_HANDLE;
	p_var->command_pool_var.command_buffer_count = 8;
	p_var->command_pool_var.command_buffer_list = (VkCommandBuffer *)cg_alloc_memory(p_var->p_memory_pool_var, p_var->command_pool_var.command_buffer_count * sizeof(VkCommandBuffer));
	if (NULL != p_var->command_pool_var.command_buffer_list) {
		PRINT_LOG("alloc memory success!\n");
		cg_create_command_pool(p_var, &p_var->command_pool_var.command_pool);
		if (p_var->command_pool_var.command_pool == VK_NULL_HANDLE) {
			return FALSE;
		}
		cg_create_command_buffer_list(p_var, p_var->command_pool_var.command_pool, &p_var->command_pool_var.command_buffer_list[0], p_var->command_pool_var.command_buffer_count);
		cg_begin_record_command_buffer(p_var, p_var->command_pool_var.command_buffer_list[0], VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		cg_end_record_command_buffer(p_var, p_var->command_pool_var.command_buffer_list[0]);
		cg_reset_command_buffer(p_var, p_var->command_pool_var.command_buffer_list[0], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
		cg_reset_command_pool(p_var, p_var->command_pool_var.command_pool, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
	}

	/* p_var->sync_var.semaphore_count = 1;
	p_var->sync_var.semaphore_list = (VkSemaphore *)malloc(p_var->sync_var.semaphore_count * sizeof(VkSemaphore));
	if (NULL != p_var->sync_var.semaphore_list)
	{
	    create_semaphore(p_var);
	}
	p_var->sync_var.fence_count = 1;
	p_var->sync_var.fence_list = (VkFence *)malloc(p_var->sync_var.fence_count * sizeof(VkFence));
	if (NULL != p_var->sync_var.fence_list)
	{
	    create_fence(p_var);
	    wait_for_fences(p_var);
	    reset_for_fences(p_var);
	} */

	if (cg_create_window(p_var) == FALSE) {
		return FALSE;
	}

	/* p_var->sync_var.wait_semaphore_count = 1;
	p_var->sync_var.wait_semaphore_list = (VkSemaphore *)malloc(p_var->sync_var.wait_semaphore_count * sizeof(VkSemaphore));
	p_var->sync_var.semaphore_pipeline_stage_count = 1;
	p_var->sync_var.semaphore_pipeline_stage_list = (VkPipelineStageFlags *)malloc(p_var->sync_var.semaphore_pipeline_stage_count * sizeof(VkPipelineStageFlags));
	p_var->sync_var.semaphore_pipeline_stage_list[0] = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	if (NULL != p_var->sync_var.semaphore_list &&
	    NULL != p_var->sync_var.semaphore_pipeline_stage_list &&
	    NULL != p_var->sync_var.fence_list)
	{
	    submit_command_buff_to_queue(p_var);
	} */

	PRINT_LOG("initialize success!\n");
	return TRUE;
}
