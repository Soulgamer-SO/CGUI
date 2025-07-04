#ifndef CG_COMMAND_POOL_H
#define CG_COMMAND_POOL_H 1
#include "cg_var.h"

// 创建命令池
bool cg_create_command_pool(cg_var_t *p_var, VkCommandPool *p_command_pool);

// 分配命令缓冲区
void cg_create_command_buffer_array(cg_var_t *p_var, VkCommandPool command_pool, VkCommandBuffer *command_buffer_array, uint32_t command_buffer_count);

// 开始命令缓冲区记录操作
void cg_begin_record_command_buffer(cg_var_t *p_var, VkCommandBuffer command_buffer, VkCommandBufferUsageFlags command_buffer_usage);

// 结束命令缓冲区记录操作
void cg_end_record_command_buffer(cg_var_t *p_var, VkCommandBuffer command_buffer);

// 重置命令缓冲区
void cg_reset_command_buffer(cg_var_t *p_var, VkCommandBuffer command_buffer, VkCommandBufferResetFlags command_buffer_reset_flag);

// 重置命令池
void cg_reset_command_pool(cg_var_t *p_var, VkCommandPool command_pool, VkCommandBufferResetFlags command_buffer_reset_flag);

#endif // CG_COMMAND_POOL_H 1
