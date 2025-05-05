#include "cg_gpu_memory_pool.h"

bool cg_create_gpu_memory_pool(cg_gpu_memory_pool_var_t *p_mp) {
	if (p_mp->size < sizeof(cg_memory_node_t)) {
		PRINT_ERROR("memory pool size should more bigger!\n");
		return false;
	}
	VkBuffer vk_buffer;
	PFN_vkCreateBuffer vk_create_buffer = nullptr;
	vk_create_buffer = (PFN_vkCreateBuffer)p_var->library_var.vk_get_device_proc_addr(p_var->logic_device_var.vk_logic_device, "vkCreateBuffer");
	VkBufferCreateInfo vk_buff_create_info = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.size = p_mp->size,
		.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = VK_NULL_HANDLE};
	if (vk_create_buffer != nullptr) {
		p_var->library_var.vk_result = vk_create_buffer(p_var->logic_device_var.vk_logic_device, &vk_buff_create_info, VK_NULL_HANDLE, &vk_buffer);
		if (p_var->library_var.vk_result != VK_SUCCESS) {
			PRINT_ERROR("create vk_create_buffer fail!\n");
		}
		p_mp->memory_pool = calloc(1, p_mp->size);
		p_mp->free_memory_node_addr_arry = calloc(MAX_FREE_MEM_NODE_COUNT, sizeof(cg_memory_node_t *));
		if (p_mp->memory_pool == VK_DEVICE_ADDR_NULL || p_mp->free_memory_node_addr_arry == VK_DEVICE_ADDR_NULL) {
			PRINT_ERROR("create memory pool fail!\n");
			return false;
		}
		p_mp->free_size = p_mp->size;
		p_mp->memory_count = 0;
		PRINT_LOG("============================memory pool============================\n");
		PRINT_LOG("create memory_pool success!\n");
		PRINT_LOG("memory_pool = %d;\n", p_mp->memory_pool);
		PRINT_LOG("memory_pool_size = %zu;\n", p_mp->size);
		PRINT_LOG("free_size = %d;\n", p_mp->free_size);
		PRINT_LOG("===================================================================\n");
		return true;
	}
}

void *cg_alloc_gpu_memory(cg_gpu_memory_pool_var_t *p_mp, size_t size) {
	;
}
