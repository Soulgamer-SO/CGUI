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
	cg_info_t info;
	cg_memory_pool_info_t memory_pool = {
		.memory_pool = nullptr,
		.size = CG_MEMORY_POOL_SIZE,
		.free_size = 0,
		.memory_count = 0,
		.p_last_memory_node = nullptr,
		.free_memory_node_count = 0,
		.free_memory_node_addr_array = nullptr,
		.free_memory_node_addr_max_count = CG_MAX_FREE_MEM_NODE_COUNT};
	memory_pool.memory_pool = calloc(1, memory_pool.size);
	memory_pool.free_memory_node_addr_array = calloc(CG_MAX_FREE_MEM_NODE_COUNT, sizeof(cg_memory_node_t *));
	if (cg_create_memory_pool(&memory_pool) == false) {
		goto exit;
	} else {
		info.p_memory_pool = &memory_pool;
		PRINT_LOG("create memory_pool success!\n");
	}

	// 测试内存池的分配和释放功能
#if DEBUG
#define TEST_CG_MEMORY_POOL_SIZE (1024ULL)
#define TEST_CG_MAX_FREE_MEM_NODE_COUNT (32U)
	PRINT_LOG("test memory pool start!\n");
	cg_memory_pool_info_t test_memory_pool = {
		.memory_pool = nullptr,
		.size = TEST_CG_MEMORY_POOL_SIZE,
		.free_size = 0,
		.memory_count = 0,
		.p_last_memory_node = nullptr,
		.free_memory_node_count = 0,
		.free_memory_node_addr_array = nullptr,
		.free_memory_node_addr_max_count = TEST_CG_MAX_FREE_MEM_NODE_COUNT};
	test_memory_pool.memory_pool = calloc(1, test_memory_pool.size);
	test_memory_pool.free_memory_node_addr_array = calloc(TEST_CG_MAX_FREE_MEM_NODE_COUNT, sizeof(cg_memory_node_t *));
	if (cg_create_memory_pool(&test_memory_pool) == false || test_memory_pool.memory_pool == nullptr || test_memory_pool.free_memory_node_addr_array == nullptr) {
		goto exit;
	} else {
		PRINT_LOG("create memory_pool success!\n");
	}

	void *test_memory_1 = cg_alloc_memory(&test_memory_pool, 128);
	if (test_memory_1 == nullptr) {
		PRINT_ERROR("alloc test_memory_1 fail!\n");
		goto destroy_memory_pool;
	}

	void *test_memory_2 = cg_alloc_memory(&test_memory_pool, 256);
	if (test_memory_2 == nullptr) {
		PRINT_ERROR("alloc test_memory_2 fail!\n");
		goto destroy_memory_pool;
	}

	void *test_memory_3 = cg_alloc_memory(&test_memory_pool, (640 - 3 * sizeof(cg_memory_node_t)));
	if (test_memory_3 == nullptr) {
		PRINT_ERROR("alloc test_memory_3 fail!\n");
		goto destroy_memory_pool;
	}

	// 测试分配过大的内存块,检查是否正确返回nullptr
	void *test_too_large_memory = cg_alloc_memory(&test_memory_pool, TEST_CG_MEMORY_POOL_SIZE);
	if (test_too_large_memory == nullptr) {
		PRINT_ERROR("alloc test_too_large_memory fail!\n");
	}

	// 释放内存块test_memory_1,测试第一个内存块的释放是否成功
	if (cg_free_memory(&test_memory_pool, test_memory_1) == false) {
		PRINT_ERROR("free test_memory_1 fail!\n");
		goto destroy_memory_pool;
	} else {
		test_memory_1 = nullptr;
	}

	// 释放内存块test_memory_2,测试内存块合并功能,检查内存池的free_size是否正确更新
	if (cg_free_memory(&test_memory_pool, test_memory_2) == false) {
		PRINT_ERROR("free test_memory_2 fail!\n");
		goto destroy_memory_pool;
	} else {
		test_memory_2 = nullptr;
	}

	// 再次分配内存块test_memory_4,大小为128字节,检查是分配到的地址是否和内存池开始地址一样
	void *test_memory_4 = cg_alloc_memory(&test_memory_pool, 128);
	if (test_memory_4 == nullptr) {
		PRINT_ERROR("alloc test_memory_4 fail!\n");
		goto destroy_memory_pool;
	} else {
		if (test_memory_4 == test_memory_pool.memory_pool + sizeof(cg_memory_node_t)) {
			PRINT_LOG("和内存池开始地址一样!\n");
		} else {
			PRINT_ERROR("和内存池开始地址不一样!\n");
		}
	}

	PRINT_LOG("test memory pool end!\n");
#endif

#ifdef _WIN32
	info.wsi.WinAPI_info.hInstance = hInstance,
	info.wsi.WinAPI_info.hPrevInstance = hPrevInstance;
	info.wsi.WinAPI_info.pCmdLine = pCmdLine;
	info.wsi.WinAPI_info.nCmdShow = nCmdShow;
#endif // _WIN32
	bool is_init = false;
	is_init = cg_initialize_var(&info);
	if (is_init == false) {
		PRINT_ERROR("initialize fail!\n");
		goto destroy_memory_pool;
	}

	cg_event_loop(&info);

destroy_var:
	cg_destroy(&info);
destroy_memory_pool:
#if DEBUG
	free(test_memory_pool.memory_pool);
	test_memory_pool.memory_pool = nullptr;
	free(test_memory_pool.free_memory_node_addr_array);
	test_memory_pool.free_memory_node_addr_array = nullptr;
#endif
	free(memory_pool.memory_pool);
	memory_pool.memory_pool = nullptr;
	free(memory_pool.free_memory_node_addr_array);
	memory_pool.free_memory_node_addr_array = nullptr;
exit:
	PRINT_LOG("Exit success!\n");
	return EXIT_SUCCESS;
}
