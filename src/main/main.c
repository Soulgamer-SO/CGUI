#include "../functions/cg_destroy.h"
#include "../functions/cg_event_loop.h"
#include "../functions/cg_initialize.h"
#include "../functions/cg_memory_pool.h"
#include "cg_platform.h"
#define MEMORY_POOL_SIZE 1024 * 1024
#define NODE_LIST_SIZE 4 * 1024

// gdb反汇编调试命令 -exec disassemble /m main
MAIN {
	cg_memory_pool_var_t memory_pool_var = {
		.memory_pool = malloc(MEMORY_POOL_SIZE),
		.size = MEMORY_POOL_SIZE,
		.free_size = 0,
		.last_memory_end_addr = nullptr,
		.memory_node_count = 0,
		.memory_node_list = malloc(NODE_LIST_SIZE),
		.memory_node_max_count = NODE_LIST_SIZE / sizeof(cg_memory_node_t)};
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
	free(memory_pool_var.memory_node_list);
	memory_pool_var.memory_node_list = nullptr;
exit:
	PRINT_LOG("Exit success!\n");
	return EXIT_SUCCESS;
}
