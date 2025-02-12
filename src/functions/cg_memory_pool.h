#ifndef CG_MEMORY_POOL_H
#define CG_MEMORY_POOL_H 1
#include "cg_bool.h"
#include "cg_log.h"
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
	bool_t is_used;
} cg_memory_node_t;

// 用来记录内存池信息
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
	// 记录内存块信息的节点的列表(数据呈逆序排列)
	cg_memory_node_t *memory_node_list;
	// 内存块信息节点的列表本身是否放在内存池里
	bool_t is_memory_node_list_in_pool;
	// 内存块信息节点的最大数量(只有节点列表本身不在内存池里才适用)
	uint32_t memory_node_max_count;
} cg_memory_pool_var_t;

/*创建内存池
示例代码:
#define MEM_POOL_SIZE 1024 * 1024
 cg_memory_pool_var_t memory_pool_var = {
		.memory_pool = malloc(MEM_POOL_SIZE),
		.size = MEM_POOL_SIZE,
		.free_size = 0
		.last_memory_end_addr = NULL,
		.memory_node_count = 0,
		.memory_node_list = NULL,
		.is_memory_node_list_in_pool = TRUE, //TRUE列表本身也放内存池里
		.memory_node_max_count = 0};
		if (cg_create_memory_pool(&memory_pool_var) == FALSE) {
		goto exit;
	} else {
		PRINT_LOG("create memory_pool_var success!\n");
	}
注意:确保memory_pool_var变量的生命周期和自己期望的一致,因为这个变量就代表了内存池*/
bool_t cg_create_memory_pool(cg_memory_pool_var_t *p_var);

// 如果内存块信息节点的列表本身不在内存池内就用此函数设置
cg_memory_node_t *cg_set_memory_node_list_addr(void *node_list_addr, size_t node_list_size);

// 获取内存块内存占用大小
size_t cg_get_memory_size(cg_memory_pool_var_t *p_var, void *memory_addr);

// 获取内存块信息节点的索引
uint32_t cg_get_memory_node_index(cg_memory_pool_var_t *p_var, void *memory_addr);

// 根据内存首地址或者尾地址来获取内存块信息节点
cg_memory_node_t *cg_get_memory_node(cg_memory_pool_var_t *p_var, void *memory_addr, void *memory_end_addr);

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
