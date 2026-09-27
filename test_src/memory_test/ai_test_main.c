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

/* 内存池设计契约测试
 *
 * 设计要点(参考 cg_memory.h / cg_memory.c):
 *   1. 分配优先级：第一优先级 = 末尾分配；第二优先级 = 用预先分配固定大小的
 *      空闲节点指针数组(free_memory_node_addr_array)查询复用空闲内存块；
 *   2. 释放机制：被释放的内存块，如果相邻内存块还在使用中(未释放)，就暂时选择
 *      不立即合并，此时标记该块 is_used = false，并把该块的内存块信息节点
 *      添加进空闲节点指针数组；
 *   3. 如果相邻内存块已空闲，则立即合并(中间合并替换数组条目，尾部合并移除数组条目)；
 *   4. 空闲节点指针数组是无序集合，元素顺序与实际内存块顺序无关。
 *
 *   测试项目(全部写在main函数里):
 *   测试1(正例): 分配优先级 —— 末尾有空间时走末尾分配，不消耗数组中的空闲块；
 *   测试2(正例): 释放机制 —— 相邻块在使用中，暂不合并，is_used=false，节点入数组；
 *   测试3: 多个空闲节点复用 —— 复用数组首项后，末尾交换的节点仍应保持空闲；
 *   测试4: 中间合并 —— 释放"前邻占用、后邻空闲"的中间块，应立即与后邻合并；
 *   测试5: 尾部合并 —— 释放"前邻空闲"的尾块，应立即与前邻合并，数组条目被移除；
 *   测试6: 数组不丢块 —— 多个"暂不合并"的空闲块节点都应仍可被查询到。
 *   测试7: 首块与后邻空闲块合并后，数组必须指向池内节点；
 *
 *   测试8: 空闲节点数组索引超出有效范围时，删除操作应失败。
 *   测试9: 尾部空间不足且只有一个空闲节点时，应复用该节点。
 *       test_src/memory_test/ai_test_main.c src/functions/cg_memory.c \
 *       -o /tmp/test_memory_design
 *   /tmp/test_memory_design
 * 任一断言失败时退出码非0。
 */

#include "../functions/cg_log.h"
#include "../functions/cg_memory.h"

#define TEST_CG_SMALL_MEMORY_POOL_SIZE (512U)
#define TEST_CG_FRAGMENTED_MEMORY_POOL_SIZE (2048U)
#define TEST_CG_MAX_FREE_MEM_NODE_COUNT (64U)

// 记录测试失败次数，供main函数最后判断退出码
static int g_test_fail_count = 0;

static bool init_test_memory_pool(cg_memory_pool_info_t *p_mp, size_t pool_size, uint32_t max_free_nodes) {
    *p_mp = (cg_memory_pool_info_t){
        .memory_pool = calloc(1, pool_size),
        .size = pool_size,
        .free_memory_node_addr_array = calloc(max_free_nodes, sizeof(cg_memory_node_t *)),
        .free_memory_node_addr_max_count = max_free_nodes};
    return p_mp->memory_pool != nullptr && p_mp->free_memory_node_addr_array != nullptr && cg_create_memory_pool(p_mp);
}

static void destroy_test_memory_pool(cg_memory_pool_info_t *p_mp) {
    free(p_mp->memory_pool);
    free(p_mp->free_memory_node_addr_array);
}

int main() {
    PRINT_LOG("test memory design start!\n");

    /* ============================================================
    * 测试1(正例): 分配优先级，第一优先级是末尾分配
    * 步骤: 分配2块64字节 -> 释放第1块(后邻在使用中，暂不合并，节点入数组)
    *       -> 再分配64字节: 尾部还有大量空间，应走末尾分配，不消耗数组中的空闲块
     * ============================================================ */
    {
        cg_memory_pool_info_t test_memory_pool = {
            .memory_pool = nullptr,
            .size = TEST_CG_SMALL_MEMORY_POOL_SIZE,
            .free_size = 0,
            .memory_count = 0,
            .p_last_memory_node = nullptr,
            .free_memory_node_count = 0,
            .free_memory_node_addr_array = nullptr,
            .free_memory_node_addr_max_count = TEST_CG_MAX_FREE_MEM_NODE_COUNT};
        test_memory_pool.memory_pool = calloc(1, test_memory_pool.size);
        test_memory_pool.free_memory_node_addr_array = calloc(TEST_CG_MAX_FREE_MEM_NODE_COUNT, sizeof(cg_memory_node_t *));
        if (cg_create_memory_pool(&test_memory_pool) == false || test_memory_pool.memory_pool == nullptr || test_memory_pool.free_memory_node_addr_array == nullptr) {
            PRINT_ERROR("测试1: create memory_pool fail!\n");
            goto exit;
        } else {
            PRINT_LOG("测试1: create memory_pool success!\n");
        }

        void *test_memory_1 = cg_alloc_memory(&test_memory_pool, 64);
        if (test_memory_1 == nullptr) {
            PRINT_ERROR("测试1: alloc test_memory_1 fail!\n");
            goto destroy_memory_pool_1;
        }
        void *test_memory_2 = cg_alloc_memory(&test_memory_pool, 64);
        if (test_memory_2 == nullptr) {
            PRINT_ERROR("测试1: alloc test_memory_2 fail!\n");
            goto destroy_memory_pool_1;
        }

        // 释放 test_memory_1(首块): 后邻 test_memory_2 还在使用中，暂不合并，
        // 标记 is_used=false 并把节点加入空闲节点指针数组
        if (cg_free_memory(&test_memory_pool, test_memory_1) == false) {
            PRINT_ERROR("测试1: free test_memory_1 fail!\n");
            goto destroy_memory_pool_1;
        }

        // 再分配64字节: 尾部还有大量空间，第一优先级应走末尾分配，而不是消耗数组里的 test_memory_1
        void *test_memory_3 = cg_alloc_memory(&test_memory_pool, 64);
        if (test_memory_3 == nullptr) {
            PRINT_ERROR("测试1: alloc test_memory_3 fail!\n");
            goto destroy_memory_pool_1;
        }
        if (test_memory_3 != test_memory_1) {
            PRINT_LOG("测试1: 新分配走末尾分配，地址与 test_memory_1 不同!\n");
        } else {
            PRINT_ERROR("测试1: 新分配复用了数组中的 test_memory_1，没有走末尾分配!\n");
            g_test_fail_count++;
        }
        // 数组只有一个有效元素时，它必须是 test_memory_1 的节点(未被消耗)
        if (test_memory_pool.free_memory_node_count == 1 && test_memory_pool.free_memory_node_addr_array[0] == (cg_memory_node_t *)((char *)test_memory_1 - sizeof(cg_memory_node_t))) {
            PRINT_LOG("测试1: 数组中的 test_memory_1 未被消耗!\n");
        } else {
            PRINT_ERROR("测试1: 数组中的 test_memory_1 被消耗或数组内容被改动!\n");
            g_test_fail_count++;
        }

destroy_memory_pool_1:
        free(test_memory_pool.memory_pool);
        test_memory_pool.memory_pool = nullptr;
        free(test_memory_pool.free_memory_node_addr_array);
        test_memory_pool.free_memory_node_addr_array = nullptr;
    }

    /* ============================================================
    * 测试2(正例): 释放机制 —— 相邻块在使用中，暂不合并，is_used=false，节点入数组
    * 步骤: 分配3块64字节 -> 释放第2块(前后邻都在使用中)
    *       -> 检查该块 is_used=false，且其节点已加入空闲节点指针数组
     * ============================================================ */
    {
        cg_memory_pool_info_t test_memory_pool = {
            .memory_pool = nullptr,
            .size = TEST_CG_SMALL_MEMORY_POOL_SIZE,
            .free_size = 0,
            .memory_count = 0,
            .p_last_memory_node = nullptr,
            .free_memory_node_count = 0,
            .free_memory_node_addr_array = nullptr,
            .free_memory_node_addr_max_count = TEST_CG_MAX_FREE_MEM_NODE_COUNT};
        test_memory_pool.memory_pool = calloc(1, test_memory_pool.size);
        test_memory_pool.free_memory_node_addr_array = calloc(TEST_CG_MAX_FREE_MEM_NODE_COUNT, sizeof(cg_memory_node_t *));
        if (cg_create_memory_pool(&test_memory_pool) == false || test_memory_pool.memory_pool == nullptr || test_memory_pool.free_memory_node_addr_array == nullptr) {
            PRINT_ERROR("测试2: create memory_pool fail!\n");
            goto exit;
        } else {
            PRINT_LOG("测试2: create memory_pool success!\n");
        }

        void *test_memory_1 = cg_alloc_memory(&test_memory_pool, 64);
        if (test_memory_1 == nullptr) {
            PRINT_ERROR("测试2: alloc test_memory_1 fail!\n");
            goto destroy_memory_pool_2;
        }
        void *test_memory_2 = cg_alloc_memory(&test_memory_pool, 64);
        if (test_memory_2 == nullptr) {
            PRINT_ERROR("测试2: alloc test_memory_2 fail!\n");
            goto destroy_memory_pool_2;
        }
        void *test_memory_3 = cg_alloc_memory(&test_memory_pool, 64);
        if (test_memory_3 == nullptr) {
            PRINT_ERROR("测试2: alloc test_memory_3 fail!\n");
            goto destroy_memory_pool_2;
        }

        // 释放 test_memory_2(中间块，前后邻都在使用中): 暂不合并，is_used=false，节点入数组
        if (cg_free_memory(&test_memory_pool, test_memory_2) == false) {
            PRINT_ERROR("测试2: free test_memory_2 fail!\n");
            goto destroy_memory_pool_2;
        }
        // 该块应被标记为 is_used = false
        cg_memory_node_t *p_test_memory_2_node = (cg_memory_node_t *)((char *)test_memory_2 - sizeof(cg_memory_node_t));
        if (p_test_memory_2_node->is_used == false) {
            PRINT_LOG("测试2: test_memory_2 已标记 is_used = false!\n");
        } else {
            PRINT_ERROR("测试2: test_memory_2 未被标记 is_used = false!\n");
            g_test_fail_count++;
        }
        // 该块的节点应已加入空闲节点指针数组
        if (test_memory_pool.free_memory_node_count == 1 && test_memory_pool.free_memory_node_addr_array[0] == p_test_memory_2_node) {
            PRINT_LOG("测试2: test_memory_2 的节点已加入数组!\n");
        } else {
            PRINT_ERROR("测试2: test_memory_2 的节点未加入数组!\n");
            g_test_fail_count++;
        }

destroy_memory_pool_2:
        free(test_memory_pool.memory_pool);
        test_memory_pool.memory_pool = nullptr;
        free(test_memory_pool.free_memory_node_addr_array);
        test_memory_pool.free_memory_node_addr_array = nullptr;
    }

    /* ============================================================
     * 测试3: 复用数组首项后，末尾交换的节点仍应保持空闲
     * ============================================================ */
    {
        cg_memory_pool_info_t test_memory_pool = {0};
        if (init_test_memory_pool(&test_memory_pool, TEST_CG_FRAGMENTED_MEMORY_POOL_SIZE, 4) == false) {
            PRINT_ERROR("测试3: create memory_pool fail!\n");
            g_test_fail_count++;
            destroy_test_memory_pool(&test_memory_pool);
        } else {
            void *blocks[4] = {nullptr};
            bool allocations_succeeded = true;
            for (uint32_t i = 0; i < 3; i++) {
                blocks[i] = cg_alloc_memory(&test_memory_pool, 400);
                if (blocks[i] == nullptr) {
                    allocations_succeeded = false;
                    break;
                }
            }
            if (allocations_succeeded == true) {
                blocks[3] = cg_alloc_memory(&test_memory_pool,
                                            TEST_CG_FRAGMENTED_MEMORY_POOL_SIZE - 5 * sizeof(cg_memory_node_t) - 3 * 400);
                allocations_succeeded = blocks[3] != nullptr;
            }
            if (allocations_succeeded == false) {
                PRINT_ERROR("测试3: alloc memory blocks fail!\n");
                g_test_fail_count++;
            } else {
                cg_free_memory(&test_memory_pool, blocks[0]);
                cg_free_memory(&test_memory_pool, blocks[2]);
                void *reused_block = cg_alloc_memory(&test_memory_pool, 400);
                cg_memory_node_t *first_node = (cg_memory_node_t *)((char *)blocks[0] - sizeof(cg_memory_node_t));
                cg_memory_node_t *other_free_node = (cg_memory_node_t *)((char *)blocks[2] - sizeof(cg_memory_node_t));
                if (reused_block != blocks[0] || first_node->is_used == false || other_free_node->is_used == true ||
                    test_memory_pool.free_memory_node_count != 1 || test_memory_pool.free_memory_node_addr_array[0] != other_free_node) {
                    PRINT_ERROR("测试3: 复用空闲节点后，空闲节点状态或数组内容错误!\n");
                    g_test_fail_count++;
                }
            }
            destroy_test_memory_pool(&test_memory_pool);
        }
    }

    /* ============================================================
    * 测试4: 中间合并 —— 释放"前邻占用、后邻空闲"的中间块，应立即与后邻合并
    * 步骤: 分配4块64字节 -> 释放第3块(前后邻都在使用中，入数组)
    *       -> 释放第2块: 后邻第3块已空闲，应立即与后邻合并
     * ============================================================ */
    {
        cg_memory_pool_info_t test_memory_pool = {
            .memory_pool = nullptr,
            .size = TEST_CG_SMALL_MEMORY_POOL_SIZE,
            .free_size = 0,
            .memory_count = 0,
            .p_last_memory_node = nullptr,
            .free_memory_node_count = 0,
            .free_memory_node_addr_array = nullptr,
            .free_memory_node_addr_max_count = TEST_CG_MAX_FREE_MEM_NODE_COUNT};
        test_memory_pool.memory_pool = calloc(1, test_memory_pool.size);
        test_memory_pool.free_memory_node_addr_array = calloc(TEST_CG_MAX_FREE_MEM_NODE_COUNT, sizeof(cg_memory_node_t *));
        if (cg_create_memory_pool(&test_memory_pool) == false || test_memory_pool.memory_pool == nullptr || test_memory_pool.free_memory_node_addr_array == nullptr) {
            PRINT_ERROR("测试4: create memory_pool fail!\n");
            goto exit;
        } else {
            PRINT_LOG("测试4: create memory_pool success!\n");
        }

        void *test_memory_1 = cg_alloc_memory(&test_memory_pool, 64);
        if (test_memory_1 == nullptr) {
            PRINT_ERROR("测试4: alloc test_memory_1 fail!\n");
            goto destroy_memory_pool_4;
        }
        void *test_memory_2 = cg_alloc_memory(&test_memory_pool, 64);
        if (test_memory_2 == nullptr) {
            PRINT_ERROR("测试4: alloc test_memory_2 fail!\n");
            goto destroy_memory_pool_4;
        }
        void *test_memory_3 = cg_alloc_memory(&test_memory_pool, 64);
        if (test_memory_3 == nullptr) {
            PRINT_ERROR("测试4: alloc test_memory_3 fail!\n");
            goto destroy_memory_pool_4;
        }
        void *test_memory_4 = cg_alloc_memory(&test_memory_pool, 64);
        if (test_memory_4 == nullptr) {
            PRINT_ERROR("测试4: alloc test_memory_4 fail!\n");
            goto destroy_memory_pool_4;
        }

        // 释放 test_memory_3(中间块，前后邻都在使用中): 暂不合并，节点入数组
        if (cg_free_memory(&test_memory_pool, test_memory_3) == false) {
            PRINT_ERROR("测试4: free test_memory_3 fail!\n");
            goto destroy_memory_pool_4;
        }

        // 释放 test_memory_2: 前邻 test_memory_1 占用，后邻 test_memory_3 已空闲，
        // 应立即与后邻合并
        bool is_free_memory_2_success = cg_free_memory(&test_memory_pool, test_memory_2);
        if (is_free_memory_2_success == true) {
            PRINT_LOG("测试4: 释放 test_memory_2 成功，与后邻合并!\n");
        } else {
            PRINT_ERROR("测试4: 释放 test_memory_2 失败，相邻块已空闲应立即合并!\n");
            g_test_fail_count++;
        }

destroy_memory_pool_4:
        free(test_memory_pool.memory_pool);
        test_memory_pool.memory_pool = nullptr;
        free(test_memory_pool.free_memory_node_addr_array);
        test_memory_pool.free_memory_node_addr_array = nullptr;
    }

    /* ============================================================
    * 测试5: 尾部合并 —— 释放"前邻空闲"的尾块，应立即与前邻合并，数组条目被移除
    * 步骤: 分配3块64字节 -> 释放第2块(前后邻都在使用中，入数组)
    *       -> 释放第3块(尾块，前邻第2块已空闲): 应立即与前邻合并，
    *          合并后 memory_count==1，数组条目被移除
     * ============================================================ */
    {
        cg_memory_pool_info_t test_memory_pool = {
            .memory_pool = nullptr,
            .size = TEST_CG_SMALL_MEMORY_POOL_SIZE,
            .free_size = 0,
            .memory_count = 0,
            .p_last_memory_node = nullptr,
            .free_memory_node_count = 0,
            .free_memory_node_addr_array = nullptr,
            .free_memory_node_addr_max_count = TEST_CG_MAX_FREE_MEM_NODE_COUNT};
        test_memory_pool.memory_pool = calloc(1, test_memory_pool.size);
        test_memory_pool.free_memory_node_addr_array = calloc(TEST_CG_MAX_FREE_MEM_NODE_COUNT, sizeof(cg_memory_node_t *));
        if (cg_create_memory_pool(&test_memory_pool) == false || test_memory_pool.memory_pool == nullptr || test_memory_pool.free_memory_node_addr_array == nullptr) {
            PRINT_ERROR("测试5: create memory_pool fail!\n");
            goto exit;
        } else {
            PRINT_LOG("测试5: create memory_pool success!\n");
        }

        void *test_memory_1 = cg_alloc_memory(&test_memory_pool, 64);
        if (test_memory_1 == nullptr) {
            PRINT_ERROR("测试5: alloc test_memory_1 fail!\n");
            goto destroy_memory_pool_5;
        }
        void *test_memory_2 = cg_alloc_memory(&test_memory_pool, 64);
        if (test_memory_2 == nullptr) {
            PRINT_ERROR("测试5: alloc test_memory_2 fail!\n");
            goto destroy_memory_pool_5;
        }
        void *test_memory_3 = cg_alloc_memory(&test_memory_pool, 64);
        if (test_memory_3 == nullptr) {
            PRINT_ERROR("测试5: alloc test_memory_3 fail!\n");
            goto destroy_memory_pool_5;
        }

        // 释放 test_memory_2(中间块，前后邻都在使用中): 暂不合并，节点入数组
        if (cg_free_memory(&test_memory_pool, test_memory_2) == false) {
            PRINT_ERROR("测试5: free test_memory_2 fail!\n");
            goto destroy_memory_pool_5;
        }

        // 释放 test_memory_3(尾块): 前邻 test_memory_2 已空闲，应立即与前邻合并，
        // 合并后的尾部空间回收，test_memory_2 在数组中的条目被移除
        bool is_free_memory_3_success = cg_free_memory(&test_memory_pool, test_memory_3);
        if (is_free_memory_3_success == true) {
            PRINT_LOG("测试5: 释放 test_memory_3 成功，与前邻合并!\n");
        } else {
            PRINT_ERROR("测试5: 释放 test_memory_3 失败，相邻块已空闲应立即合并!\n");
            g_test_fail_count++;
        }
        // 合并后只应剩 test_memory_1，检查 memory_count
        if (test_memory_pool.memory_count == 1) {
            PRINT_LOG("测试5: 合并后 memory_count == 1!\n");
        } else {
            PRINT_ERROR("测试5: 合并后 memory_count != 1,计数错乱!\n");
            g_test_fail_count++;
        }
        // 合并后 test_memory_2 在数组中的条目应被移除
        if (test_memory_pool.free_memory_node_count == 0) {
            PRINT_LOG("测试5: 合并后数组条目已移除，free_memory_node_count == 0!\n");
        } else {
            PRINT_ERROR("测试5: 合并后数组条目未移除，free_memory_node_count != 0!\n");
            g_test_fail_count++;
        }

destroy_memory_pool_5:
        free(test_memory_pool.memory_pool);
        test_memory_pool.memory_pool = nullptr;
        free(test_memory_pool.free_memory_node_addr_array);
        test_memory_pool.free_memory_node_addr_array = nullptr;
    }

    /* ============================================================
     * 测试6: 数组不丢块 —— 多个"暂不合并"的空闲块节点都应仍可被查询到
     * 步骤: 分配4块64字节 -> 释放第1块和第3块，两者都应保留在空闲节点数组中
     * ============================================================ */
    {
        cg_memory_pool_info_t test_memory_pool = {
            .memory_pool = nullptr,
            .size = TEST_CG_SMALL_MEMORY_POOL_SIZE,
            .free_size = 0,
            .memory_count = 0,
            .p_last_memory_node = nullptr,
            .free_memory_node_count = 0,
            .free_memory_node_addr_array = nullptr,
            .free_memory_node_addr_max_count = TEST_CG_MAX_FREE_MEM_NODE_COUNT};
        test_memory_pool.memory_pool = calloc(1, test_memory_pool.size);
        test_memory_pool.free_memory_node_addr_array = calloc(TEST_CG_MAX_FREE_MEM_NODE_COUNT, sizeof(cg_memory_node_t *));
        if (cg_create_memory_pool(&test_memory_pool) == false || test_memory_pool.memory_pool == nullptr || test_memory_pool.free_memory_node_addr_array == nullptr) {
            PRINT_ERROR("测试6: create memory_pool fail!\n");
            goto exit;
        } else {
            PRINT_LOG("测试6: create memory_pool success!\n");
        }

        void *test_memory_1 = cg_alloc_memory(&test_memory_pool, 64);
        void *test_memory_2 = cg_alloc_memory(&test_memory_pool, 64);
        void *test_memory_3 = cg_alloc_memory(&test_memory_pool, 64);
        void *test_memory_4 = cg_alloc_memory(&test_memory_pool, 64);
        if (test_memory_1 == nullptr || test_memory_2 == nullptr || test_memory_3 == nullptr || test_memory_4 == nullptr) {
            PRINT_ERROR("测试6: alloc memory blocks fail!\n");
            goto destroy_memory_pool_6;
        }

        if (cg_free_memory(&test_memory_pool, test_memory_1) == false || cg_free_memory(&test_memory_pool, test_memory_3) == false) {
            PRINT_ERROR("测试6: free memory blocks fail!\n");
            goto destroy_memory_pool_6;
        }
        if (test_memory_pool.free_memory_node_count != 2) {
            PRINT_ERROR("测试6: 两次释放后 free_memory_node_count != 2!\n");
            g_test_fail_count++;
        }

        bool is_test_memory_1_found = false;
        bool is_test_memory_3_found = false;
        for (uint32_t i = 0; i < test_memory_pool.free_memory_node_count; i++) {
            if (test_memory_pool.free_memory_node_addr_array[i] == (cg_memory_node_t *)((char *)test_memory_1 - sizeof(cg_memory_node_t))) {
                is_test_memory_1_found = true;
            }
            if (test_memory_pool.free_memory_node_addr_array[i] == (cg_memory_node_t *)((char *)test_memory_3 - sizeof(cg_memory_node_t))) {
                is_test_memory_3_found = true;
            }
        }
        if (is_test_memory_1_found == false || is_test_memory_3_found == false) {
            PRINT_ERROR("测试6: 空闲节点数组丢失了已释放的内存块!\n");
            g_test_fail_count++;
        }

destroy_memory_pool_6:
        free(test_memory_pool.memory_pool);
        test_memory_pool.memory_pool = nullptr;
        free(test_memory_pool.free_memory_node_addr_array);
        test_memory_pool.free_memory_node_addr_array = nullptr;
    }

    /* ============================================================
     * 测试7: 首块与后邻空闲块合并后，数组必须指向池内节点
     * ============================================================ */
    {
        cg_memory_pool_info_t test_memory_pool = {0};
        if (init_test_memory_pool(&test_memory_pool, TEST_CG_SMALL_MEMORY_POOL_SIZE, 4) == false) {
            PRINT_ERROR("测试7: create memory_pool fail!\n");
            g_test_fail_count++;
            destroy_test_memory_pool(&test_memory_pool);
        } else {
            void *first_block = cg_alloc_memory(&test_memory_pool, 64);
            void *second_block = cg_alloc_memory(&test_memory_pool, 64);
            void *third_block = cg_alloc_memory(&test_memory_pool, 64);
            if (first_block == nullptr || second_block == nullptr || third_block == nullptr ||
                cg_free_memory(&test_memory_pool, second_block) == false ||
                cg_free_memory(&test_memory_pool, first_block) == false) {
                PRINT_ERROR("测试7: prepare or merge memory blocks fail!\n");
                g_test_fail_count++;
            } else {
                cg_memory_node_t *first_node = (cg_memory_node_t *)((char *)first_block - sizeof(cg_memory_node_t));
                if (test_memory_pool.free_memory_node_count != 1 || test_memory_pool.free_memory_node_addr_array[0] != first_node ||
                    first_node->is_used == true || test_memory_pool.memory_count != 1) {
                    PRINT_ERROR("测试7: 合并后的空闲节点未正确保存在内存池中!\n");
                    g_test_fail_count++;
                }
            }
            destroy_test_memory_pool(&test_memory_pool);
        }
    }

    /* ============================================================
     * 测试8: 空闲节点数组索引超出有效范围时，删除操作应失败
     * ============================================================ */
    {
        cg_memory_pool_info_t test_memory_pool = {0};
        if (init_test_memory_pool(&test_memory_pool, TEST_CG_SMALL_MEMORY_POOL_SIZE, 4) == false) {
            PRINT_ERROR("测试8: create memory_pool fail!\n");
            g_test_fail_count++;
            destroy_test_memory_pool(&test_memory_pool);
        } else {
            void *first_block = cg_alloc_memory(&test_memory_pool, 64);
            void *middle_block = cg_alloc_memory(&test_memory_pool, 64);
            void *last_block = cg_alloc_memory(&test_memory_pool, 64);
            if (first_block == nullptr || middle_block == nullptr || last_block == nullptr ||
                cg_free_memory(&test_memory_pool, middle_block) == false) {
                PRINT_ERROR("测试8: prepare memory blocks fail!\n");
                g_test_fail_count++;
            } else {
                int32_t node_index = -1;
                bool found_node = cg_get_memory_node_index(&test_memory_pool, middle_block, &node_index);
                cg_memory_node_t *middle_node = (cg_memory_node_t *)((char *)middle_block - sizeof(cg_memory_node_t));
                bool remove_result = cg_rm_one_p_memory_node(&test_memory_pool, test_memory_pool.free_memory_node_count);
                if (found_node == false || node_index != 0 || remove_result == true ||
                    test_memory_pool.free_memory_node_count != 1 || test_memory_pool.free_memory_node_addr_array[0] != middle_node) {
                    PRINT_ERROR("测试8: 超出范围的空闲节点索引未被拒绝或改变了数组!\n");
                    g_test_fail_count++;
                }
            }
            destroy_test_memory_pool(&test_memory_pool);
        }
    }

    /* ============================================================
     * 测试9: 尾部空间不足且只有一个空闲节点时，应复用该节点
     * ============================================================ */
    {
        cg_memory_pool_info_t test_memory_pool = {0};
        if (init_test_memory_pool(&test_memory_pool, TEST_CG_FRAGMENTED_MEMORY_POOL_SIZE, 4) == false) {
            PRINT_ERROR("测试9: create memory_pool fail!\n");
            g_test_fail_count++;
            destroy_test_memory_pool(&test_memory_pool);
        } else {
            void *first_block = cg_alloc_memory(&test_memory_pool, 512);
            void *free_block = cg_alloc_memory(&test_memory_pool, 512);
            void *last_block = cg_alloc_memory(
                &test_memory_pool, TEST_CG_FRAGMENTED_MEMORY_POOL_SIZE - 4 * sizeof(cg_memory_node_t) - 2 * 512);
            if (first_block == nullptr || free_block == nullptr || last_block == nullptr) {
                PRINT_ERROR("测试9: prepare memory blocks fail!\n");
                g_test_fail_count++;
            } else if (cg_free_memory(&test_memory_pool, free_block) == false) {
                PRINT_ERROR("测试9: free middle block fail!\n");
                g_test_fail_count++;
            } else {
                void *reused_block = cg_alloc_memory(&test_memory_pool, 512);
                if (reused_block != free_block) {
                    PRINT_ERROR("测试9: 尾部空间不足时未复用唯一空闲节点!\n");
                    g_test_fail_count++;
                }
            }
            destroy_test_memory_pool(&test_memory_pool);
        }
    }

    if (g_test_fail_count == 0) {
        PRINT_LOG("所有断言通过! test memory design end!\n");
        return EXIT_SUCCESS;
    } else {
        PRINT_ERROR("共 %d 个断言失败! test memory design end!\n", g_test_fail_count);
        return EXIT_FAILURE;
    }

exit:
    if (g_test_fail_count == 0) {
        PRINT_LOG("Exit success!\n");
        return EXIT_SUCCESS;
    } else {
        PRINT_ERROR("Exit fail,共 %d 个断言失败!\n", g_test_fail_count);
        return EXIT_FAILURE;
    }
}
