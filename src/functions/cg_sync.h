#ifndef CG_SYNC_H
#define CG_SYNC_H 1
#include "cg_var.h"
#include <stdio.h>
#include <stdlib.h>

// 创建信号量
void cg_create_semaphore(cg_var_t *p_var);

// 创建围栏
void cg_create_fence(cg_var_t *p_var);

// 等待围栏状态
void cg_wait_for_fences(cg_var_t *p_var);

// 重置围栏状态
void cg_reset_for_fences(cg_var_t *p_var);

// 将命令缓冲提交给队列
void cg_submit_command_buff_to_queue(cg_var_t *p_var);

#endif // CG_SYNC_H 1
