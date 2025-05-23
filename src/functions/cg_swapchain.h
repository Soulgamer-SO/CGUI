#ifndef CG_SWAPCHAIN_H
#define CG_SWAPCHAIN_H 1
#include "cg_var.h"
#ifdef __linux
#include <dlfcn.h>
#endif // __linux

// 选择想要的交换链
bool cg_select_swapchain(cg_var_t *p_var);

// 创建交换链
bool cg_create_swapchain(cg_var_t *p_var, VkSwapchainKHR *p_swapchain);

// 选择想要的显示模式
bool cg_select_present_mode(cg_var_t *p_var);

#endif // CG_SWAPCHAIN_H
