#include "cg_command_pool.h"

bool cg_create_command_pool(cg_var_t *p_var, VkCommandPool *p_command_pool) {
	VkCommandPoolCreateInfo command_pool_create_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = NULL,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = p_var->logic_device_var.graphic_queue_family_index};

	PFN_vkCreateCommandPool create_command_pool = NULL;
	create_command_pool = (PFN_vkCreateCommandPool)p_var->library_var.get_device_proc_addr(p_var->logic_device_var.vk_logic_device, "vkCreateCommandPool");
	if (create_command_pool == NULL) {
		PRINT_ERROR("load vkCreateCommandPool fail!\n");
		return FALSE;
	}

	*p_command_pool = VK_NULL_HANDLE;
	p_var->library_var.vk_result = create_command_pool(
		p_var->logic_device_var.vk_logic_device, &command_pool_create_info,
		NULL, p_command_pool);
	if (p_var->library_var.vk_result != VK_SUCCESS) {
		PRINT_ERROR("create command_pool fail!\n");
		return FALSE;
	}

	return TRUE;
}

void cg_create_command_buffer_list(cg_var_t *p_var, VkCommandPool command_pool, VkCommandBuffer *command_buffer_list, uint32_t command_buffer_count) {
	if (command_buffer_list == NULL) {
		PRINT_ERROR("create_command_buffer fail!\n");
		return;
	}
	VkCommandBufferAllocateInfo command_buffer_allocate_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = NULL,
		.commandPool = command_pool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = command_buffer_count};

	PFN_vkAllocateCommandBuffers allocate_command_buffers = NULL;
	allocate_command_buffers = (PFN_vkAllocateCommandBuffers)p_var->library_var.get_device_proc_addr(p_var->logic_device_var.vk_logic_device, "vkAllocateCommandBuffers");
	if (allocate_command_buffers == NULL) {
		PRINT_ERROR("load vkAllocateCommandBuffers fail!\n");
		return;
	}

	p_var->library_var.vk_result = allocate_command_buffers(p_var->logic_device_var.vk_logic_device, &command_buffer_allocate_info, command_buffer_list);
	if (p_var->library_var.vk_result != VK_SUCCESS) {
		PRINT_ERROR("vkAllocateCommandBuffers fail!\n");
		return;
	}

	PRINT_LOG("create_command_buffer success!\n");
	return;
}

void cg_begin_record_command_buffer(cg_var_t *p_var, VkCommandBuffer command_buffer, VkCommandBufferUsageFlags command_buffer_usage) {
	// VkCommandBufferInheritanceInfo secondary_command_buffer_info;
	VkCommandBufferBeginInfo command_buffer_begin_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = NULL,
		.flags = command_buffer_usage,
		.pInheritanceInfo = NULL};

	PFN_vkBeginCommandBuffer vkBeginCommandBuffer = NULL;
	vkBeginCommandBuffer = (PFN_vkBeginCommandBuffer)p_var->library_var.get_instance_proc_addr(p_var->instance_var.vk_instance, "vkBeginCommandBuffer");
	if (vkBeginCommandBuffer == NULL) {
		PRINT_ERROR("load vkBeginCommandBuffer fail!\n");
		return;
	}

	p_var->library_var.vk_result = vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);
	if (p_var->library_var.vk_result != VK_SUCCESS) {
		PRINT_ERROR("vkBeginCommandBuffer fail!\n");
		return;
	}

	return;
}

void cg_end_record_command_buffer(cg_var_t *p_var, VkCommandBuffer command_buffer) {
	PFN_vkEndCommandBuffer vkEndCommandBuffer = NULL;
	vkEndCommandBuffer = (PFN_vkEndCommandBuffer)p_var->library_var.get_instance_proc_addr(p_var->instance_var.vk_instance, "vkEndCommandBuffer");
	if (vkEndCommandBuffer == NULL) {
		PRINT_ERROR("load vkEndCommandBuffer fail!\n");
		return;
	}

	p_var->library_var.vk_result = vkEndCommandBuffer(command_buffer);
	if (p_var->library_var.vk_result != VK_SUCCESS) {
		PRINT_ERROR("vkEndCommandBuffer fail!\n");
		return;
	}

	return;
}

void cg_reset_command_buffer(cg_var_t *p_var, VkCommandBuffer command_buffer, VkCommandBufferResetFlags command_buffer_reset_flag) {
	PFN_vkResetCommandBuffer vkResetCommandBuffer = NULL;
	vkResetCommandBuffer = (PFN_vkResetCommandBuffer)p_var->library_var.get_instance_proc_addr(p_var->instance_var.vk_instance, "vkResetCommandBuffer");
	if (vkResetCommandBuffer == NULL) {
		PRINT_ERROR("load vkResetCommandBuffer fail!\n");
		return;
	}

	vkResetCommandBuffer(command_buffer, command_buffer_reset_flag);
	if (p_var->library_var.vk_result != VK_SUCCESS) {
		PRINT_ERROR("vkResetCommandBuffer fail!\n");
		return;
	}

	return;
}

void cg_reset_command_pool(cg_var_t *p_var, VkCommandPool command_pool, VkCommandBufferResetFlags command_buffer_reset_flag) {
	PFN_vkResetCommandPool vkResetCommandPool = NULL;
	vkResetCommandPool = (PFN_vkResetCommandPool)p_var->library_var.get_device_proc_addr(p_var->logic_device_var.vk_logic_device, "vkResetCommandPool");
	if (vkResetCommandPool == NULL) {
		PRINT_ERROR("load vkResetCommandPool fail!\n");
		return;
	}

	vkResetCommandPool(p_var->logic_device_var.vk_logic_device, command_pool, command_buffer_reset_flag);
	if (p_var->library_var.vk_result != VK_SUCCESS) {
		PRINT_ERROR("vkResetCommandPool fail!\n");
		return;
	}

	return;
}
