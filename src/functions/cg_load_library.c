#include "cg_load_library.h"

bool cg_load_library(cg_var_t *p_var) {
#ifdef __linux
	p_var->library_var.vulkan_library = NULL;
	p_var->library_var.vulkan_library = dlopen("libvulkan.so.1", RTLD_NOW);
	if (p_var->library_var.vulkan_library == NULL) {
		PRINT_ERROR("load libvulkan.so.1 fail!\n");
		return FALSE;
	} else {
		PRINT_LOG("load vulkan.so.1 success!\n");
	}

	// 加载实例函数的函数 vkGetInstanceProcAddr
	p_var->library_var.get_instance_proc_addr = NULL;
	p_var->library_var.get_instance_proc_addr = (PFN_vkGetInstanceProcAddr)dlsym(
		p_var->library_var.vulkan_library, "vkGetInstanceProcAddr");
	if (p_var->library_var.get_instance_proc_addr == NULL) {
		PRINT_ERROR("load vkGetInstanceProcAddr fail!\n");
		return FALSE;
	}

	// 加载设备函数的函数 vkGetDeviceProcAddr
	p_var->library_var.get_device_proc_addr = NULL;
	p_var->library_var.get_device_proc_addr = (PFN_vkGetDeviceProcAddr)dlsym(
		p_var->library_var.vulkan_library, "vkGetDeviceProcAddr");
	if (p_var->library_var.get_device_proc_addr == NULL) {
		PRINT_ERROR("load vkGetDeviceProcAddr fail!\n");
		return FALSE;
	}
#endif // __linux

#ifdef _WIN32
	p_var->library_var.vulkan_library = NULL;
	p_var->library_var.vulkan_library = LoadLibrary("vulkan-1.dll");
	if (p_var->library_var.vulkan_library == NULL) {
		PRINT_ERROR("load vulkan-1.dll fail!\n");
		return FALSE;
	} else {
		PRINT_LOG("load vulkan-1.dll success!\n");
	}

	// 加载实例函数的函数 vkGetInstanceProcAddr
	p_var->library_var.get_instance_proc_addr = NULL;
	p_var->library_var.get_instance_proc_addr = (PFN_vkGetInstanceProcAddr)GetProcAddress(p_var->library_var.vulkan_library, "vkGetInstanceProcAddr");
	if (p_var->library_var.get_instance_proc_addr == NULL) {
		PRINT_ERROR("load vkGetInstanceProcAddr fail!\n");
		return FALSE;
	}

	// 加载设备函数的函数 vkGetDeviceProcAddr
	p_var->library_var.get_device_proc_addr = NULL;
	p_var->library_var.get_device_proc_addr = (PFN_vkGetDeviceProcAddr)GetProcAddress(p_var->library_var.vulkan_library, "vkGetDeviceProcAddr");
	if (p_var->library_var.get_device_proc_addr == NULL) {
		PRINT_ERROR("load vkGetDeviceProcAddr fail!\n");
		return FALSE;
	}
#endif // _WIN32

	return TRUE;
}
