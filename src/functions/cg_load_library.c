/*
Copyright (C) 2025  Soulgamer <SOsoulgamer@outlook.com>.

This file is part of CGUI.

CGUI is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

CGUI is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "cg_load_library.h"

bool cg_load_library(cg_var_t *p_var) {
#ifdef __linux
	p_var->library_var.vulkan_library = nullptr;
	p_var->library_var.vulkan_library = dlopen("libvulkan.so.1", RTLD_NOW);
	if (p_var->library_var.vulkan_library == nullptr) {
		PRINT_ERROR("load libvulkan.so.1 fail!\n");
		return false;
	} else {
		PRINT_LOG("load vulkan.so.1 success!\n");
	}

	// 加载实例函数的函数 vkGetInstanceProcAddr
	p_var->library_var.vk_get_instance_proc_addr = nullptr;
	p_var->library_var.vk_get_instance_proc_addr = (PFN_vkGetInstanceProcAddr)dlsym(
		p_var->library_var.vulkan_library, "vkGetInstanceProcAddr");
	if (p_var->library_var.vk_get_instance_proc_addr == nullptr) {
		PRINT_ERROR("load vkGetInstanceProcAddr fail!\n");
		return false;
	}

	// 加载设备函数的函数 vkGetDeviceProcAddr
	p_var->library_var.vk_get_device_proc_addr = nullptr;
	p_var->library_var.vk_get_device_proc_addr = (PFN_vkGetDeviceProcAddr)dlsym(
		p_var->library_var.vulkan_library, "vkGetDeviceProcAddr");
	if (p_var->library_var.vk_get_device_proc_addr == nullptr) {
		PRINT_ERROR("load vkGetDeviceProcAddr fail!\n");
		return false;
	}
#endif // __linux

#ifdef _WIN32
	p_var->library_var.vulkan_library = nullptr;
	p_var->library_var.vulkan_library = LoadLibrary("vulkan-1.dll");
	if (p_var->library_var.vulkan_library == nullptr) {
		PRINT_ERROR("load vulkan-1.dll fail!\n");
		return false;
	} else {
		PRINT_LOG("load vulkan-1.dll success!\n");
	}

	// 加载实例函数的函数 vkGetInstanceProcAddr
	p_var->library_var.get_instance_proc_addr = nullptr;
	p_var->library_var.get_instance_proc_addr = (PFN_vkGetInstanceProcAddr)GetProcAddress(p_var->library_var.vulkan_library, "vkGetInstanceProcAddr");
	if (p_var->library_var.get_instance_proc_addr == nullptr) {
		PRINT_ERROR("load vkGetInstanceProcAddr fail!\n");
		return false;
	}

	// 加载设备函数的函数 vkGetDeviceProcAddr
	p_var->library_var.get_device_proc_addr = nullptr;
	p_var->library_var.get_device_proc_addr = (PFN_vkGetDeviceProcAddr)GetProcAddress(p_var->library_var.vulkan_library, "vkGetDeviceProcAddr");
	if (p_var->library_var.get_device_proc_addr == nullptr) {
		PRINT_ERROR("load vkGetDeviceProcAddr fail!\n");
		return false;
	}
#endif // _WIN32

	return true;
}
