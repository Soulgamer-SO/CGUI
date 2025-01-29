#ifndef CG_INSTANCE_H
#define CG_INSTANCE_H 1
#include "cg_var.h"
#include <stdio.h>
#ifdef __linux
#include <dlfcn.h>
#endif // __linux
#include <stdlib.h>

// 创建Vulkan实例
bool_t cg_create_instance(cg_var_t *p_var, VkInstance *p_vk_instance);

#endif // CG_INSTANCE_H 1
