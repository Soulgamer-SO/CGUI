#ifndef CG_MEMORY_POOL_H
#define CG_MEMORY_POOL_H 1
#include "cg_log.h"
#include <stdint.h>
#ifdef __linux
#include <alloca.h>
#endif // __linux
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// 用来记录内存块的信息的节点
typedef struct cg_memory_node {
	// 内存块的地址
	void *addr;
	// 内存块的尾地址
	void *end_addr;
	// 表示内存块是否被使用
	bool is_used;
} cg_memory_node_t;

// 用来记录内存池信息(非侵入式内存池)
typedef struct cg_memory_pool_var {
	// 内存池
	void *memory_pool;
	// 内存池总大小
	size_t size;
	// 内存池剩余可用大小
	size_t free_size;
	// 保存最后尾的内存块的尾地址
	void *last_memory_end_addr;
	// 记录内存块信息的节点的数量,也可能包括已经被释放的内存块的信息
	uint32_t memory_node_count;
	// 记录内存块信息的节点的列表
	cg_memory_node_t *memory_node_list;
	// 内存块信息节点列表的大小上限
	size_t memory_node_max_size;
} cg_memory_pool_var_t;

/*创建内存池(非侵入式内存池)

示例代码:
#define MEM_POOL_SIZE 1024 * 1024
#define NODE_LIST_SIZE 4 * 1024
cg_memory_node_t *memory_node_list_memory = malloc(NODE_LIST_SIZE);
	if (memory_node_list_memory == nullptr) {
		goto exit;
	}
	cg_memory_pool_var_t memory_pool_var = {
		.memory_pool = malloc(MEM_POOL_SIZE),
		.size = MEM_POOL_SIZE,
		.free_size = 0,
		.last_memory_end_addr = nullptr,
		.memory_node_count = 0,
		.memory_node_list = malloc(NODE_LIST_SIZE),
		.memory_node_max_size = NODE_LIST_SIZE};
	if (cg_create_memory_pool(&memory_pool_var) == false) {
		goto exit;
	} else {
		PRINT_LOG("create memory_pool_var success!\n");
	}
注意:确保memory_pool_var变量的生命周期和自己期望的一致,因为这个变量就代表了内存池*/
bool cg_create_memory_pool(cg_memory_pool_var_t *p_var);

// 获取内存块内存占用大小
size_t cg_get_memory_size(cg_memory_pool_var_t *p_var, void *memory_addr);

// 获取内存块信息节点的索引
uint32_t cg_get_memory_node_index(cg_memory_pool_var_t *p_var, void *memory_addr);

// 根据内存首地址或者尾地址来获取内存块信息节点的地址
cg_memory_node_t *cg_get_memory_node_addr(cg_memory_pool_var_t *p_var, void *memory_addr, void *memory_end_addr);

// 添加一个内存块信息节点
void cg_add_one_memory_node(cg_memory_pool_var_t *p_var, cg_memory_node_t memory_node_info);

// 删除一个内存块信息节点
void cg_rm_one_memory_node(cg_memory_pool_var_t *p_var, uint32_t index);

// 使用内存池，分配内存块
void *cg_alloc_memory(cg_memory_pool_var_t *p_var, size_t size);

// 释放指定内存块
void cg_free_memory(cg_memory_pool_var_t *p_var, void *memory_addr);

#ifdef DEBUG
// 使用内存池，给指定内存块重新分配内存
void *cg_realloc_memory(cg_memory_pool_var_t *p_var, void *memory_addr, size_t size);
#endif

#endif // CG_MEMORY_POOL_H 1
