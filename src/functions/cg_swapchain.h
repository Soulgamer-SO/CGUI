#ifndef CG_SWAPCHAIN_H
#define CG_SWAPCHAIN_H 1
#include "cg_destroy.h"
#include "cg_instance.h"
#include "cg_logic_device.h"
#include "cg_var.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __linux
#include <dlfcn.h>
#endif // __linux

// 选择想要的交换链
bool_t cg_select_swapchain(cg_var_t *p_var);

// 创建交换链
bool_t cg_create_swapchain(cg_var_t *p_var, VkSwapchainKHR *p_swapchain);

// 选择想要的显示模式
bool_t cg_select_present_mode(cg_var_t *p_var);

#endif // CG_SWAPCHAIN_H
