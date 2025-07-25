#ifndef CG_GPU_MEMORY_POOL_H
#define CG_GPU_MEMORY_POOL_H 1
#include "cg_log.h"
#include "cg_var.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>
#define GPU_MEMORY_POOL_SIZE 4 * 1024 * 1024 * 1024UL
#define MAX_FREE_GPU_MEM_NODE_COUNT 4 * 1024
#define VK_DEVICE_ADDR_NULL 0

// 用来记录GPU内存池信息(侵入式内存池,依赖VulkanAPI),可以根据情况再创建各自独立的多个内存池
typedef struct cg_gpu_memory_pool_var {
	// 内存池,内存池开始地址
	VkDeviceAddress memory_pool;
	// 内存池总大小
	VkDeviceSize size;
	// 内存池剩余可用大小
	VkDeviceSize free_size;
	// 内存块数量,可能包括空闲内存块
	uint32_t memory_count;
	// 保存排在最后的内存块的大小
	VkDeviceSize last_memory_size;
	// 保存排在最后的内存块的尾地址
	VkDeviceAddress last_memory_end_addr;
	// 空闲内存块信息节点数量
	uint32_t free_memory_node_count;
	// 空闲内存块信息节点指针的列表
	VkDeviceAddress *free_memory_node_addr_array;
	// Vulkan 显存句柄
	VkDeviceMemory vk_device_memory;
} cg_gpu_memory_pool_var_t;

// 记录GPU内存块的信息的节点,节点本身位置在内存块的前面
typedef struct cg_gpu_memory_node {
	// 内存块地址
	VkDeviceAddress memory_addr;
	// 内存块大小(不包含内存信息节点本身)
	VkDeviceSize size;
	// 表示内存块是否被使用
	bool is_used;
	// 记录上一个内存块信息节点的地址
	VkDeviceAddress prev_memory_node_addr;
} cg_gpu_memory_node_t;

/*创建内存池(侵入式内存池)
注意:确保memory_pool_var变量的生命周期和自己期望的一致,因为这个变量就代表了内存池*/
bool cg_create_gpu_memory_pool(cg_gpu_memory_pool_var_t *p_mp, cg_var_t *vk_var);

// 使用内存池，分配指定大小的内存块
VkDeviceAddress cg_alloc_gpu_memory(cg_gpu_memory_pool_var_t *p_mp, size_t size);

// 释放指定内存块
void cg_free_gpu_memory(cg_gpu_memory_pool_var_t *p_mp, void *memory_addr);

// 如果成功该函数返回内存块占用大小
size_t cg_get_gpu_memory_size(cg_gpu_memory_pool_var_t *p_mp, void *memory_addr);

// 如果成功,参数返回信息节点的索引,失败就返回-1
int32_t cg_get_gpu_memory_node_index(cg_gpu_memory_pool_var_t *p_mp, void *memory_addr);

// 信息节点地址的列表末尾添加一个元素
bool cg_add_one_p_gpu_memory_node(cg_gpu_memory_pool_var_t *p_mp, cg_gpu_memory_node_t *p_memory_node);

// 删除信息节点地址的列表中一个元素(末尾交换法)
bool cg_rm_one_p_gpu_memory_node(cg_gpu_memory_pool_var_t *p_mp, uint32_t index);

#endif // CG_GPU_MEMORY_POOL_H 1
