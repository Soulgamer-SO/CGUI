#include "../functions/cg_destroy.h"
#include "../functions/cg_event_loop.h"
#include "../functions/cg_gpu_memory_pool.h"
#include "../functions/cg_initialize.h"
#include "../functions/cg_log.h"
#include "../functions/cg_memory_pool.h"
#include "cg_platform.h"
#include <stdlib.h>

// gdb反汇编调试命令 -exec disassemble /m main
MAIN {
	cg_var_t var;
	cg_memory_pool_var_t memory_pool_var = {
		.memory_pool = nullptr,
		.size = MEMORY_POOL_SIZE,
		.free_size = 0,
		.memory_count = 0,
		.last_memory_size = 0,
		.last_memory_end_addr = nullptr,
		.free_memory_node_count = 0,
		.free_memory_node_addr_array = nullptr};
	memory_pool_var.memory_pool = calloc(1, memory_pool_var.size);
	memory_pool_var.free_memory_node_addr_array = calloc(MAX_FREE_MEM_NODE_COUNT, sizeof(cg_memory_node_t *));
	if (cg_create_memory_pool(&memory_pool_var) == false) {
		goto exit;
	} else {
		var.p_memory_pool_var = &memory_pool_var;
		PRINT_LOG("create memory_pool_var success!\n");
	}
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
#if 0
	cg_gpu_memory_pool_var_t gpu_memory_pool_var = {
		.memory_pool = VK_DEVICE_ADDR_NULL,
		.size = MEMORY_POOL_SIZE,
		.free_size = 0,
		.memory_count = 0,
		.last_memory_size = 0,
		.last_memory_end_addr = VK_DEVICE_ADDR_NULL,
		.free_memory_node_count = 0,
		.free_memory_node_addr_array = VK_DEVICE_ADDR_NULL,
		.vk_device_memory = VK_NULL_HANDLE};
	if (cg_create_gpu_memory_pool(&gpu_memory_pool_var, &var) == false) {
		PRINT_ERROR("create gpu_memory_pool_var fail!\n");
		goto destroy_var;
	} else {
		var.p_gpu_memory_pool_var = &gpu_memory_pool_var;
		PRINT_LOG("create gpu_memory_pool_var success!\n");
	}
#endif
	cg_event_loop(&var);
destroy_gpu_memory_pool:
destroy_var:
	cg_destroy(&var);
destroy_memory_pool:
	free(memory_pool_var.memory_pool);
	memory_pool_var.memory_pool = nullptr;
	free(memory_pool_var.free_memory_node_addr_array);
	memory_pool_var.free_memory_node_addr_array = nullptr;
exit:
	PRINT_LOG("Exit success!\n");
	return EXIT_SUCCESS;
}
