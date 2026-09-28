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

#define TEST_CG_SMALL_MEMORY_POOL_SIZE (512U)
#define TEST_CG_FRAGMENTED_MEMORY_POOL_SIZE (2048U)
#define TEST_CG_MAX_FREE_MEM_NODE_COUNT (8U)

static int g_test_fail_count = 0;

static void test_check(bool condition, const char *message) {
    if (condition == false) {
        PRINT_ERROR("%s\n", message);
        g_test_fail_count++;
    } else {
        PRINT_LOG("%s\n", message);
    }
}

static bool init_test_memory_pool(cg_memory_pool_info_t *p_mp,
                                  size_t pool_size,
                                  uint32_t max_free_nodes) {
    *p_mp = (cg_memory_pool_info_t){
        .memory_pool = calloc(1, pool_size),
        .size = pool_size,
        .free_memory_node_addr_array =
            calloc(max_free_nodes, sizeof(cg_memory_node_t *)),
        .free_memory_node_addr_max_count = max_free_nodes};

    if (p_mp->memory_pool == nullptr ||
        p_mp->free_memory_node_addr_array == nullptr) {
        return false;
    }

    return cg_create_memory_pool(p_mp);
}

static void destroy_test_memory_pool(cg_memory_pool_info_t *p_mp) {
    free(p_mp->memory_pool);
    free(p_mp->free_memory_node_addr_array);
    *p_mp = (cg_memory_pool_info_t){0};
}

static cg_memory_node_t *get_memory_node(void *memory_addr) {
    return (cg_memory_node_t *)((char *)memory_addr - sizeof(cg_memory_node_t));
}

static void test_tail_allocation_priority(void) {
    cg_memory_pool_info_t memory_pool = {0};
    if (init_test_memory_pool(&memory_pool,
                              TEST_CG_SMALL_MEMORY_POOL_SIZE,
                              TEST_CG_MAX_FREE_MEM_NODE_COUNT) == false) {
        test_check(false, "测试1：创建内存池失败");
        destroy_test_memory_pool(&memory_pool);
        return;
    }

    void *block_1 = cg_alloc_memory(&memory_pool, 64);
    void *block_2 = cg_alloc_memory(&memory_pool, 64);
    if (block_1 == nullptr || block_2 == nullptr) {
        test_check(false, "测试1：分配初始内存块失败");
        destroy_test_memory_pool(&memory_pool);
        return;
    }

    test_check(cg_free_memory(&memory_pool, block_1),
               "测试1：释放首块成功");

    void *block_3 = cg_alloc_memory(&memory_pool, 64);
    test_check(block_3 != nullptr && block_3 != block_1,
               "测试1：尾部有空间时应优先从尾部新分配");

    test_check(memory_pool.free_memory_node_count == 1 &&
                   memory_pool.free_memory_node_addr_array[0] ==
                       get_memory_node(block_1),
               "测试1：尾部优先分配不应消耗空闲节点数组中的首块");

    destroy_test_memory_pool(&memory_pool);
}

static void test_reuse_free_node_after_swap_removal(void) {
    cg_memory_pool_info_t memory_pool = {0};
    if (init_test_memory_pool(&memory_pool,
                              TEST_CG_FRAGMENTED_MEMORY_POOL_SIZE,
                              4) == false) {
        test_check(false, "测试2：创建内存池失败");
        destroy_test_memory_pool(&memory_pool);
        return;
    }

    const size_t block_size = 400;
    const size_t header_size = sizeof(cg_memory_node_t);
    const size_t filler_size =
        TEST_CG_FRAGMENTED_MEMORY_POOL_SIZE -
        5 * header_size - 3 * block_size;

    void *blocks[4] = {nullptr};
    bool allocation_succeeded = filler_size > 0;

    for (uint32_t i = 0; i < 3 && allocation_succeeded; i++) {
        blocks[i] = cg_alloc_memory(&memory_pool, block_size);
        allocation_succeeded = blocks[i] != nullptr;
    }

    if (allocation_succeeded) {
        blocks[3] = cg_alloc_memory(&memory_pool, filler_size);
        allocation_succeeded = blocks[3] != nullptr;
    }

    if (allocation_succeeded == false) {
        test_check(false, "测试2：构造碎片化内存布局失败");
        destroy_test_memory_pool(&memory_pool);
        return;
    }

    bool freed_first = cg_free_memory(&memory_pool, blocks[0]);
    bool freed_third = cg_free_memory(&memory_pool, blocks[2]);
    if (freed_first == false || freed_third == false) {
        test_check(false, "测试2：释放待复用内存块失败");
        destroy_test_memory_pool(&memory_pool);
        return;
    }

    void *reused_block = cg_alloc_memory(&memory_pool, block_size);
    cg_memory_node_t *first_node = get_memory_node(blocks[0]);
    cg_memory_node_t *other_free_node = get_memory_node(blocks[2]);

    test_check(reused_block == blocks[0],
               "测试2：应复用空闲数组中的首个匹配块");

    test_check(first_node->is_used == true &&
                   other_free_node->is_used == false &&
                   memory_pool.free_memory_node_count == 1 &&
                   memory_pool.free_memory_node_addr_array[0] ==
                       other_free_node,
               "测试2：删除数组首项后，末尾交换的空闲节点状态应保持正确");

    destroy_test_memory_pool(&memory_pool);
}

static void test_merge_with_next_free_block(void) {
    cg_memory_pool_info_t memory_pool = {0};
    if (init_test_memory_pool(&memory_pool,
                              TEST_CG_SMALL_MEMORY_POOL_SIZE,
                              TEST_CG_MAX_FREE_MEM_NODE_COUNT) == false) {
        test_check(false, "测试3：创建内存池失败");
        destroy_test_memory_pool(&memory_pool);
        return;
    }

    void *blocks[4] = {nullptr};
    bool allocation_succeeded = true;

    for (uint32_t i = 0; i < 4; i++) {
        blocks[i] = cg_alloc_memory(&memory_pool, 64);
        if (blocks[i] == nullptr) {
            allocation_succeeded = false;
            break;
        }
    }

    if (allocation_succeeded == false) {
        test_check(false, "测试3：分配内存块失败");
        destroy_test_memory_pool(&memory_pool);
        return;
    }

    bool freed_third = cg_free_memory(&memory_pool, blocks[2]);
    bool freed_second = freed_third && cg_free_memory(&memory_pool, blocks[1]);
    test_check(freed_second,
               "测试3：释放相邻内存块成功");

    cg_memory_node_t *merged_node = get_memory_node(blocks[1]);
    cg_memory_node_t *fourth_node = get_memory_node(blocks[3]);

    test_check(merged_node->is_used == false &&
                   merged_node->memory_addr == blocks[1] &&
                   merged_node->size ==
                       2 * 64 + sizeof(cg_memory_node_t) &&
                   fourth_node->prev_memory_node_addr == merged_node,
               "测试3：释放块应与后邻空闲块合并并更新后继节点");

    test_check(memory_pool.free_memory_node_count == 1 &&
                   memory_pool.free_memory_node_addr_array[0] == merged_node,
               "测试3：合并后空闲节点数组应只保留合并节点");

    destroy_test_memory_pool(&memory_pool);
}

static void test_merge_tail_with_previous_free_block(void) {
    cg_memory_pool_info_t memory_pool = {0};
    if (init_test_memory_pool(&memory_pool,
                              TEST_CG_SMALL_MEMORY_POOL_SIZE,
                              TEST_CG_MAX_FREE_MEM_NODE_COUNT) == false) {
        test_check(false, "测试4：创建内存池失败");
        destroy_test_memory_pool(&memory_pool);
        return;
    }

    void *blocks[3] = {nullptr};
    for (uint32_t i = 0; i < 3; i++) {
        blocks[i] = cg_alloc_memory(&memory_pool, 64);
        if (blocks[i] == nullptr) {
            test_check(false, "测试4：分配内存块失败");
            destroy_test_memory_pool(&memory_pool);
            return;
        }
    }

    bool freed_middle = cg_free_memory(&memory_pool, blocks[1]);
    bool freed_tail = freed_middle && cg_free_memory(&memory_pool, blocks[2]);
    test_check(freed_tail,
               "测试4：释放尾块并与前邻空闲块合并成功");

    test_check(memory_pool.memory_count == 1 &&
                   memory_pool.p_last_memory_node == get_memory_node(blocks[0]),
               "测试4：尾部合并后最后节点应回退到仍占用的前块");

    test_check(memory_pool.free_memory_node_count == 0,
               "测试4：尾部合并后应从空闲节点数组移除已回收节点");

    destroy_test_memory_pool(&memory_pool);
}

static void test_merge_first_block_with_next_free_block(void) {
    cg_memory_pool_info_t memory_pool = {0};
    if (init_test_memory_pool(&memory_pool,
                              TEST_CG_SMALL_MEMORY_POOL_SIZE,
                              TEST_CG_MAX_FREE_MEM_NODE_COUNT) == false) {
        test_check(false, "测试5：创建内存池失败");
        destroy_test_memory_pool(&memory_pool);
        return;
    }

    void *blocks[3] = {nullptr};
    bool allocation_succeeded = true;

    for (uint32_t i = 0; i < 3; i++) {
        blocks[i] = cg_alloc_memory(&memory_pool, 64);
        if (blocks[i] == nullptr) {
            allocation_succeeded = false;
            break;
        }
    }

    if (allocation_succeeded == false) {
        test_check(false, "测试5：分配内存块失败");
        destroy_test_memory_pool(&memory_pool);
        return;
    }

    bool freed_second = cg_free_memory(&memory_pool, blocks[1]);
    bool freed_first = freed_second && cg_free_memory(&memory_pool, blocks[0]);
    test_check(freed_first,
               "测试5：释放首块并与后邻空闲块合并成功");

    cg_memory_node_t *first_node = get_memory_node(blocks[0]);
    test_check(memory_pool.free_memory_node_count == 1 &&
                   memory_pool.free_memory_node_addr_array[0] == first_node,
               "测试5：合并后的空闲节点数组应指向池内首块节点");

    test_check(memory_pool.memory_count == 1,
               "测试5：首块合并后非空闲节点数量应正确");

    destroy_test_memory_pool(&memory_pool);
}

static void test_remove_invalid_free_node_index(void) {
    cg_memory_pool_info_t memory_pool = {0};
    if (init_test_memory_pool(&memory_pool,
                              TEST_CG_SMALL_MEMORY_POOL_SIZE,
                              TEST_CG_MAX_FREE_MEM_NODE_COUNT) == false) {
        test_check(false, "测试6：创建内存池失败");
        destroy_test_memory_pool(&memory_pool);
        return;
    }

    void *first_block = cg_alloc_memory(&memory_pool, 64);
    void *middle_block = cg_alloc_memory(&memory_pool, 64);
    void *last_block = cg_alloc_memory(&memory_pool, 64);

    if (first_block == nullptr || middle_block == nullptr ||
        last_block == nullptr ||
        cg_free_memory(&memory_pool, middle_block) == false) {
        test_check(false, "测试6：构造空闲节点失败");
        destroy_test_memory_pool(&memory_pool);
        return;
    }

    cg_memory_node_t *middle_node = get_memory_node(middle_block);
    uint32_t old_count = memory_pool.free_memory_node_count;
    bool removed = cg_rm_one_p_memory_node(&memory_pool, old_count);

    test_check(removed == false,
               "测试6：删除超出有效范围的索引应失败");

    test_check(memory_pool.free_memory_node_count == old_count &&
                   memory_pool.free_memory_node_addr_array[0] == middle_node,
               "测试6：非法索引操作不应改变空闲节点数组");

    destroy_test_memory_pool(&memory_pool);
}

int main(void) {
    PRINT_LOG("内存池测试开始\n");
    test_tail_allocation_priority();
    test_reuse_free_node_after_swap_removal();
    test_merge_with_next_free_block();
    test_merge_tail_with_previous_free_block();
    test_merge_first_block_with_next_free_block();
    test_remove_invalid_free_node_index();

    if (g_test_fail_count == 0) {
        PRINT_LOG("所有断言通过，内存池测试结束\n");
        return EXIT_SUCCESS;
    }

    PRINT_ERROR("共 %d 个断言失败，内存池测试结束\n", g_test_fail_count);
    return EXIT_FAILURE;
}
