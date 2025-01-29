#include "cg_sync.h"

void cg_create_semaphore(cg_var_t *p_var) {
	PFN_vkCreateSemaphore create_semaphore = NULL;
	create_semaphore = (PFN_vkCreateSemaphore)p_var->library_var.get_device_proc_addr(p_var->logic_device_var.vk_logic_device, "vkCreateSemaphore");
	if (create_semaphore == NULL) {
		PRINT_ERROR("load vkCreateSemaphore fail!\n");
		return;
	}

	VkSemaphoreCreateInfo semaphore_create_info = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0};
	p_var->library_var.vk_result = create_semaphore(p_var->logic_device_var.vk_logic_device, &semaphore_create_info, NULL, &p_var->sync_var.semaphore_list[0]);
	if (p_var->library_var.vk_result != VK_SUCCESS) {
		PRINT_ERROR("vkCreateSemaphore fail!\n");
		return;
	} else {
		PRINT_LOG("semaphore address = %p;\n", &p_var->sync_var.semaphore_list[0]);
	}

	return;
}

void cg_create_fence(cg_var_t *p_var) {
	PFN_vkCreateFence vkCreateFence = NULL;
	vkCreateFence = (PFN_vkCreateFence)p_var->library_var.get_device_proc_addr(p_var->logic_device_var.vk_logic_device, "vkCreateFence");
	if (vkCreateFence == NULL) {
		PRINT_ERROR("load vkCreateFence fail!\n");
		return;
	}

	VkFenceCreateInfo fence_create_info = {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.pNext = NULL,
		.flags = VK_FENCE_CREATE_SIGNALED_BIT}; // flags的值为 VK_FENCE_CREATE_SIGNALED_BIT 1或者0
	p_var->library_var.vk_result = vkCreateFence(
		p_var->logic_device_var.vk_logic_device, &fence_create_info,
		NULL, &p_var->sync_var.fence_list[0]);
	if (p_var->library_var.vk_result != VK_SUCCESS) {
		PRINT_ERROR("vkCreateFence fail!\n");
		return;
	} else {
		PRINT_LOG("fence address = %p;\n", &p_var->sync_var.fence_list[0]);
	}

	return;
}

void cg_wait_for_fences(cg_var_t *p_var) {
	PFN_vkWaitForFences vkWaitForFences = NULL;
	vkWaitForFences = (PFN_vkWaitForFences)p_var->library_var.get_device_proc_addr(p_var->logic_device_var.vk_logic_device, "vkWaitForFences");
	if (vkWaitForFences == NULL) {
		PRINT_ERROR("load vkWaitForFences fail!\n");
		return;
	}
	p_var->sync_var.is_wait_for = VK_TRUE;
	p_var->sync_var.timeout = 1000000000;
	if (p_var->sync_var.fence_count > 0) {
		p_var->library_var.vk_result = vkWaitForFences(
			p_var->logic_device_var.vk_logic_device, p_var->sync_var.fence_count,
			p_var->sync_var.fence_list, p_var->sync_var.is_wait_for, p_var->sync_var.timeout);
		if (p_var->library_var.vk_result != VK_SUCCESS) {
			PRINT_ERROR("vkWaitForFences fail!\n");
			return;
		}
	}

	return;
}

void cg_reset_for_fences(cg_var_t *p_var) {
	PFN_vkResetFences vkResetFences = NULL;
	vkResetFences = (PFN_vkResetFences)p_var->library_var.get_device_proc_addr(p_var->logic_device_var.vk_logic_device, "vkResetFences");
	if (NULL == vkResetFences) {
		PRINT_ERROR("load vkResetFences fail!\n");
		return;
	}
	p_var->library_var.vk_result = vkResetFences(
		p_var->logic_device_var.vk_logic_device,
		p_var->sync_var.fence_count, &p_var->sync_var.fence_list[0]);
	if (p_var->library_var.vk_result != VK_SUCCESS) {
		PRINT_ERROR("vkResetFences fail!\n");
		return;
	}

	return;
}

void cg_submit_command_buff_to_queue(cg_var_t *p_var) {

	VkSubmitInfo submit_info = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = NULL,
		.waitSemaphoreCount = 0,   // p_var->sync_var.wait_semaphore_count,
		.pWaitSemaphores = NULL,   //&p_var->sync_var.wait_semaphore_list[0],
		.pWaitDstStageMask = NULL, //&p_var->sync_var.semaphore_pipeline_stage_list[0],
		.commandBufferCount = p_var->command_pool_var.command_buffer_count,
		.pCommandBuffers = &p_var->command_pool_var.command_buffer_list[0],
		.signalSemaphoreCount = 0, // p_var->sync_var.semaphore_count,
		.pSignalSemaphores = NULL  //&p_var->sync_var.semaphore_list[0]
	};
	PFN_vkQueueSubmit vkQueueSubmit = NULL;
	vkQueueSubmit = (PFN_vkQueueSubmit)p_var->library_var.get_device_proc_addr(
		p_var->logic_device_var.vk_logic_device, "vkQueueSubmit");
	if (vkQueueSubmit == NULL) {
		PRINT_ERROR("load vkQueueSubmit fail!\n");
		return;
	}

	p_var->library_var.vk_result = vkQueueSubmit(
		p_var->logic_device_var.queue_family_handle,
		0, &submit_info,
		VK_NULL_HANDLE /* p_var->sync_var.fence_list[0] */);
	if (p_var->library_var.vk_result != VK_SUCCESS) {
		PRINT_ERROR("vkQueueSubmit fail!\n");
		return;
	}

	return;
}
