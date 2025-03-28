#ifndef CG_PHYSICAL_DEVICE_H
#define CG_PHYSICAL_DEVICE_H 1
#include "cg_var.h"
#ifdef __linux
#include <dlfcn.h>
#endif // __linux

// 创建Vulkan物理设备列表
bool cg_enumerate_physical_device(cg_var_t *p_var, uint32_t *p_physical_device_count, VkPhysicalDevice *available_physical_device_list);

// 选择想要的Vulkan物理设备
bool cg_select_physical_device(cg_var_t *p_var, uint32_t *p_physical_device_count, VkPhysicalDevice *available_physical_device_list, VkPhysicalDevice *p_physical_device);

#endif // CG_PHYSICAL_DEVICE_H 1
