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

// 用来记录内存池信息(侵入式内存池),可以根据情况再创建各自独立的多个内存池
typedef struct cg_memory_pool_var {
	// 内存池
	void *memory_pool;
	// 内存池总大小
	size_t size;
	// 内存池剩余可用大小
	size_t free_size;
	// 内存块数量
	uint32_t memory_block_count;
	// 保存最后尾的内存块的尾地址
	void *last_memory_end_addr;
} cg_memory_pool_var_t;

// 记录内存块的信息的节点,节点本身位置在内存块的前面
typedef struct cg_memory_node {
	// 内存块的尾地址
	size_t size;
	// 表示内存块是否被使用
	bool is_used;
	// 记录上一个内存块信息节点的地址
	cg_memory_node_t *prev_memory_addr;
} cg_memory_node_t;

/*创建内存池(侵入式内存池)

示例代码:
#define MEMORY_POOL_SIZE 1024 * 1024
	cg_memory_pool_var_t memory_pool_var = {
		.memory_pool = nullptr,
		.size = MEMORY_POOL_SIZE,
		.free_size = 0,
		.last_memory_end_addr = nullptr};
	memory_pool_var.memory_pool = malloc(MEMORY_POOL_SIZE);
	if (cg_create_memory_pool(&memory_pool_var) == false) {
		goto exit;
	} else {
		PRINT_LOG("create memory_pool_var success!\n");
	}
注意:确保memory_pool_var变量的生命周期和自己期望的一致,因为这个变量就代表了内存池*/
bool cg_create_memory_pool(cg_memory_pool_var_t *p_var);

// 使用内存池，分配指定大小的内存块,alloc_size是该内存块大小,如果成功该函数会返回新地址，失败就返回nullptr
void *cg_alloc_memory(cg_memory_pool_var_t *p_var, size_t alloc_size);

// 释放指定内存块
void cg_free_memory(cg_memory_pool_var_t *p_var, void *memory_addr);

// 使用内存池，给指定内存块重新分配内存,如果成功该函数会返回新地址，失败就返回nullptr
void *cg_realloc_memory(cg_memory_pool_var_t *p_var, void *memory_addr, size_t size);

// 如果成功该函数返回内存块占用大小
size_t cg_get_memory_size(cg_memory_pool_var_t *p_var, void *memory_addr);

// 如果成功,参数p_index指向的index会被更新
bool cg_get_memory_node_index(cg_memory_pool_var_t *p_var, void *memory_addr, int32_t *p_index);

/*
根据内存首地址或者尾地址来获取内存块信息
示例代码:
cg_memory_node_t *memory_node = cg_get_memory_node(p_var, memory_addr, nullptr);
或者
cg_memory_node_t *memory_node = cg_get_memory_node(p_var, nullptr , memory_end_addr);
*/
cg_memory_node_t cg_get_memory_node(cg_memory_pool_var_t *p_var, void *memory_addr, void *memory_end_addr);

// 添加一个内存块信息节点
bool cg_add_one_memory_node(cg_memory_pool_var_t *p_var, cg_memory_node_t memory_node_info);

// 修改一个内存块信息节点
bool cg_rm_one_memory_node(cg_memory_pool_var_t *p_var, int32_t index);

#endif // CG_MEMORY_POOL_H 1
