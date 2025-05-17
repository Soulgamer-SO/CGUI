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
#define MEMORY_POOL_SIZE 4 * 1024 * 1024 * 1024UL
#define MAX_FREE_MEM_NODE_COUNT 4 * 1024

// 记录内存块的信息的节点,节点本身位置在内存块的前面
struct cg_memory_node {
	// 表示内存块是否被使用
	bool is_used;
	// 内存块地址
	void *memory_addr;
	// 内存块大小(不包含内存信息节点本身)
	size_t size;
	// 记录上一个内存块信息节点的地址
	struct cg_memory_node *prev_memory_node_addr;
};
typedef struct cg_memory_node cg_memory_node_t;

// 用来记录内存池信息(侵入式内存池),可以根据情况再创建各自独立的多个内存池
typedef struct cg_memory_pool_var {
	// 内存池,内存池开始地址
	void *memory_pool;
	// 内存池总大小
	size_t size;
	// 内存池剩余可用大小
	size_t free_size;
	// 内存块数量,可能包括空闲内存块
	uint32_t memory_count;
	// 保存排在最后的内存块的大小
	size_t last_memory_size;
	// 保存排在最后的内存块的尾地址
	void *last_memory_end_addr;
	// 空闲内存块信息节点数量
	uint32_t free_memory_node_count;
	// 空闲内存块信息节点地址的列表
	cg_memory_node_t **free_memory_node_addr_arry;
} cg_memory_pool_var_t;

/*创建内存池(侵入式内存池)
注意:确保memory_pool_var变量的生命周期和自己期望的一致,因为这个变量就代表了内存池*/
bool cg_create_memory_pool(cg_memory_pool_var_t *p_mp);

// 使用内存池，分配指定大小的内存块,alloc_size是该内存块大小,如果成功该函数会返回新地址，失败就返回nullptr
void *cg_alloc_memory(cg_memory_pool_var_t *p_mp, size_t size);

// 释放指定内存块
void cg_free_memory(cg_memory_pool_var_t *p_mp, void *memory_addr);

// 如果成功该函数返回内存块占用大小,失败就返回0
size_t cg_get_memory_size(cg_memory_pool_var_t *p_mp, void *memory_addr);

// 如果成功,参数返回信息节点列表元素的索引,失败就返回-1
int32_t cg_get_memory_node_index(cg_memory_pool_var_t *p_mp, void *memory_addr);

// 信息节点地址的列表末尾添加一个元素
bool cg_add_one_p_memory_node(cg_memory_pool_var_t *p_mp, cg_memory_node_t *p_memory_node);

// 删除信息节点地址的列表中一个元素(末尾交换法)
bool cg_rm_one_p_memory_node(cg_memory_pool_var_t *p_mp, uint32_t index);

#endif // CG_MEMORY_POOL_H 1
