#ifndef CG_LOGIC_DEVICE_H
#define CG_LOGIC_DEVICE_H 1
#include "cg_var.h"
#include <stdio.h>
#ifdef __linux
#include <dlfcn.h>
#endif // __linux
#include <stdlib.h>

// 创建Vulkan逻辑设备
bool cg_create_logic_device(cg_var_t *p_var, VkDevice *p_vk_logic_device);

#endif // CG_LOGIC_DEVICE_H 1
