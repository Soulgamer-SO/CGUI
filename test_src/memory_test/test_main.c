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

#include "../functions/cg_log.h"
#include "../functions/cg_memory.h"

// 测试内存池的分配和释放功能
#define TEST_CG_MEMORY_POOL_SIZE (2048ULL)
#define TEST_CG_MAX_FREE_MEM_NODE_COUNT (64U)

int main() {
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

    // 测试double free
    void *test_memory_5 = cg_alloc_memory(&test_memory_pool, 128);
    void *test_memory_double_free = test_memory_5;
    cg_free_memory(&test_memory_pool, test_memory_5);
    bool is_double_free = false;
    is_double_free = cg_free_memory(&test_memory_pool, test_memory_double_free);
    if (is_double_free == true) {
        PRINT_ERROR("存在内存双重释放!is_double_free = true!\n");
    } else if (is_double_free == false) {
        PRINT_LOG("不存在内存双重释放!is_double_free = false!\n");
    }

    PRINT_LOG("test memory pool end!\n");

destroy_memory_pool:
    free(test_memory_pool.memory_pool);
    test_memory_pool.memory_pool = nullptr;
    free(test_memory_pool.free_memory_node_addr_array);
    test_memory_pool.free_memory_node_addr_array = nullptr;
exit:
    PRINT_LOG("Exit success!\n");
    return EXIT_SUCCESS;
}
