#include "../functions/cg_destroy.h"
#include "../functions/cg_event_loop.h"
#include "../functions/cg_initialize.h"
#include "../functions/cg_memory_pool.h"
#include "../functions/cg_wsi.h"
#include "cg_platform.h"
#include <string.h>
#define MEM_POOL_SIZE 1024 * 1024
#define NODE_LIST_SIZE 4 * 1024

// gdb反汇编调试命令 -exec disassemble /m main
MAIN {
	cg_memory_pool_var_t memory_pool_var = {
		.memory_pool = malloc(MEM_POOL_SIZE),
		.last_memory_end_addr = NULL,
		.memory_node_count = 0,
		.memory_node_list = NULL,
		.is_memory_node_list_in_pool = TRUE,
		.size = MEM_POOL_SIZE,
		.free_size = 0};
#ifdef DEBUG
	cg_memory_node_t *memory_node_list_memory = cg_alloc_memory(&memory_pool_var, NODE_LIST_SIZE);
	if (memory_node_list_memory == NULL) {
		goto exit;
	}
	cg_memory_pool_var_t other_memory_pool_var = {
		.memory_pool = malloc(MEM_POOL_SIZE),
		.last_memory_end_addr = NULL,
		.memory_node_count = 0,
		.memory_node_list = cg_set_memory_node_list_addr(memory_node_list_memory, NODE_LIST_SIZE),
		.is_memory_node_list_in_pool = FALSE,
		.size = MEM_POOL_SIZE,
		.free_size = 0};
#endif
	cg_var_t var;
	if (cg_create_memory_pool(&memory_pool_var) == FALSE) {
		goto exit;
	} else {
		PRINT_LOG("create memory pool success!\n");
	}
	var.p_memory_pool_var = &memory_pool_var;

#ifdef _WIN32
	var.wsi_var.WinAPI_var.hInstance = hInstance,
	var.wsi_var.WinAPI_var.hPrevInstance = hPrevInstance;
	var.wsi_var.WinAPI_var.pCmdLine = pCmdLine;
	var.wsi_var.WinAPI_var.nCmdShow = nCmdShow;
#endif // _WIN32
	if (cg_initialize_var(&var) == FALSE) {
		PRINT_ERROR("initialize fail!\n");
		goto destroy_and_exit;
	}

	cg_event_loop(&var);

destroy_and_exit:
	cg_destroy_and_exit(&var);
	free(memory_pool_var.memory_pool);
	memory_pool_var.memory_pool = NULL;
exit:
	PRINT_LOG("Exit success!\n");
	return EXIT_SUCCESS;
}
