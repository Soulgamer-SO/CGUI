#include "../functions/cg_destroy.h"
#include "../functions/cg_event_loop.h"
#include "../functions/cg_initialize.h"
#include "../functions/cg_log.h"
#include "../functions/cg_memory_pool.h"
#include "cg_platform.h"
#include <stdlib.h>

// gdb反汇编调试命令 -exec disassemble /m main
MAIN {
	cg_memory_pool_var_t memory_pool_var = {
		.memory_pool = nullptr,
		.size = MEMORY_POOL_SIZE,
		.free_size = 0,
		.memory_count = 0,
		.last_memory_size = 0,
		.last_memory_end_addr = nullptr,
		.free_memory_node_count = 0,
		.free_memory_node_addr_arry = nullptr};
	if (cg_create_memory_pool(&memory_pool_var) == false) {
		goto exit;
	} else {
		PRINT_LOG("create memory_pool_var success!\n");
	}
	cg_var_t var;
	var.p_memory_pool_var = &memory_pool_var;

#ifdef _WIN32
	var.wsi_var.WinAPI_var.hInstance = hInstance,
	var.wsi_var.WinAPI_var.hPrevInstance = hPrevInstance;
	var.wsi_var.WinAPI_var.pCmdLine = pCmdLine;
	var.wsi_var.WinAPI_var.nCmdShow = nCmdShow;
#endif // _WIN32
	bool is_init = false;
	is_init = cg_initialize_var(&var);
	if (is_init == false) {
		PRINT_ERROR("initialize fail!\n");
		goto destroy_memory_pool;
	}

	cg_gpu_memory_pool_var_t gpu_memory_pool_var = {
		.memory_pool = VK_DEVICE_ADDR_NULL,
		.size = MEMORY_POOL_SIZE,
		.free_size = 0,
		.memory_count = 0,
		.last_memory_size = 0,
		.last_memory_end_addr = VK_DEVICE_ADDR_NULL,
		.free_memory_node_count = 0,
		.free_memory_node_addr_arry = nullptr,
		.vk_device_memory = VK_NULL_HANDLE};
	VkBuffer vk_buffer;
	PFN_vkCreateBuffer vk_create_buffer = nullptr;
	vk_create_buffer = (PFN_vkCreateBuffer)var.library_var.get_device_proc_addr(var.logic_device_var.vk_logic_device, "vkCreateBuffer");
	VkBufferCreateInfo vk_buff_create_info = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.size = MEMORY_POOL_SIZE,
		.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = VK_NULL_HANDLE};
	if (vk_create_buffer != nullptr) {
		var.library_var.vk_result = vk_create_buffer(var.logic_device_var.vk_logic_device, &vk_buff_create_info, VK_NULL_HANDLE, &vk_buffer);
		if (var.library_var.vk_result != VK_SUCCESS) {
			PRINT_ERROR("create vk_create_buffer fail!\n");
			goto destroy_memory_pool;
		}
	}

	gpu_memory_pool_var.memory_pool;
	if (cg_create_gpu_memory_pool(&gpu_memory_pool_var) == false) {
		goto destroy_memory_pool;
	} else {
		PRINT_LOG("create gpu_memory_pool_var success!\n");
	}

	cg_event_loop(&var);

destroy_gpu_memory_pool:
destroy_memory_pool:
	cg_destroy_and_exit(&var);
	free(memory_pool_var.memory_pool);
	memory_pool_var.memory_pool = nullptr;
	free(memory_pool_var.free_memory_node_addr_arry);
	memory_pool_var.free_memory_node_addr_arry = nullptr;
exit:
	PRINT_LOG("Exit success!\n");
	return EXIT_SUCCESS;
}
