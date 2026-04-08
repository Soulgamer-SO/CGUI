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

#include "../functions/cg_destroy.h"
#include "../functions/cg_event_loop.h"
#include "../functions/cg_gpu_memory.h"
#include "../functions/cg_initialize.h"
#include "../functions/cg_log.h"
#include "../functions/cg_memory.h"
#include "cg_platform.h"
#include <stdlib.h>
#define CG_MEMORY_POOL_SIZE (4ULL * 1024 * 1024 * 1024)
#define CG_MAX_FREE_MEM_NODE_COUNT (4U * 1024)

MAIN {
	cg_var_t var;
	cg_memory_pool_var_t memory_pool_var = {
		.memory_pool = nullptr,
		.size = CG_MEMORY_POOL_SIZE,
		.free_size = 0,
		.memory_count = 0,
		.p_last_memory_node = nullptr,
		.free_memory_node_count = 0,
		.free_memory_node_addr_array = nullptr,
		.free_memory_node_addr_max_count = CG_MAX_FREE_MEM_NODE_COUNT};
	memory_pool_var.memory_pool = calloc(1, memory_pool_var.size);
	memory_pool_var.free_memory_node_addr_array = calloc(CG_MAX_FREE_MEM_NODE_COUNT, sizeof(cg_memory_node_t *));
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

	// 测试cg_free_memory函数
#if 1
	void *test_memory = cg_alloc_memory(&memory_pool_var, 1024ULL * 1024);
	if (test_memory == nullptr) {
		PRINT_ERROR("cg_alloc_memory fail!\n");
		goto destroy_var;
	}
#endif

	cg_event_loop(&var);
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
