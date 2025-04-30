#ifndef CG_DESTROY_H
#define CG_DESTROY_H 1
#include "cg_var.h"
#ifdef __linux
#include <dlfcn.h>
#endif // __linux

// 销毁Vulkan实例
void cg_destroy_instance(cg_var_t *p_var, VkInstance vk_instance);

// 销毁交换链
void cg_destroy_swapchain(cg_var_t *p_var, VkSwapchainKHR swapchain);

// 销毁命令池
void cg_destroy_command_pool(cg_var_t *p_var, VkCommandPool command_pool);

// 销毁
void cg_destroy(cg_var_t *p_var);

#endif // CG_DESTROY_H 1
