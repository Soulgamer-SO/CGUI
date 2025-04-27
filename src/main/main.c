#include "../functions/cg_destroy.h"
#include "../functions/cg_event_loop.h"
#include "../functions/cg_initialize.h"
#include "../functions/cg_log.h"
#include "../functions/cg_memory_pool.h"
#include "cg_platform.h"
#include <stdlib.h>

// gdb反汇编调试命令 -exec disassemble /m main
MAIN {
#define MEMORY_POOL_SIZE 1024 * 1024
#define MAX_FREE_MEM_NODE_COUNT 256
	cg_memory_pool_var_t memory_pool_var = {
		.memory_pool = nullptr,
		.size = MEMORY_POOL_SIZE,
		.free_size = 0,
		.last_memory_end_addr = nullptr,
		.free_memory_node_count = MAX_FREE_MEM_NODE_COUNT,
		.free_memory_node_addr_list = nullptr};
	memory_pool_var.memory_pool = calloc(1, MEMORY_POOL_SIZE);
	memory_pool_var.free_memory_node_addr_list = calloc(MAX_FREE_MEM_NODE_COUNT, sizeof(cg_memory_pool_var_t *));
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
	if (cg_initialize_var(&var) == false) {
		PRINT_ERROR("initialize fail!\n");
		goto destroy_and_exit;
	}

	cg_event_loop(&var);

destroy_and_exit:
	cg_destroy_and_exit(&var);
	free(memory_pool_var.memory_pool);
	memory_pool_var.memory_pool = nullptr;
	free(memory_pool_var.free_memory_node_addr_list);
	memory_pool_var.free_memory_node_addr_list = nullptr;
exit:
	PRINT_LOG("Exit success!\n");
	return EXIT_SUCCESS;
}
