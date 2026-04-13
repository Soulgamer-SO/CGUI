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

#include "cg_destroy.h"

void cg_destroy_instance(cg_info_t *p_info, VkInstance vk_instance) {
	if (vk_instance != VK_NULL_HANDLE && p_info->instance.vk_instance != VK_NULL_HANDLE) {
		PFN_vkDestroyInstance destroy_instance = nullptr;
		destroy_instance = (PFN_vkDestroyInstance)p_info->library.vk_get_instance_proc_addr(p_info->instance.vk_instance, "vkDestroyInstance");
		if (destroy_instance != nullptr) {
			destroy_instance(vk_instance, nullptr);
		}
	}

	return;
}

void cg_destroy_swapchain(cg_info_t *p_info, VkSwapchainKHR swapchain) {
	if (swapchain != VK_NULL_HANDLE && p_info->logic_device.vk_logic_device != VK_NULL_HANDLE) {
		PFN_vkDestroySwapchainKHR destroy_swapchain = nullptr;
		destroy_swapchain = (PFN_vkDestroySwapchainKHR)p_info->library.vk_get_device_proc_addr(p_info->logic_device.vk_logic_device, "vkDestroySwapchainKHR");
		if (destroy_swapchain != nullptr) {
			destroy_swapchain(p_info->logic_device.vk_logic_device, swapchain, nullptr);
		}
	}

	return;
}

void cg_destroy_command_pool(cg_info_t *p_info, VkCommandPool command_pool) {
	if (command_pool != VK_NULL_HANDLE && p_info->logic_device.vk_logic_device != VK_NULL_HANDLE) {
		PFN_vkDestroyCommandPool destroy_command_pool = nullptr;
		destroy_command_pool = (PFN_vkDestroyCommandPool)p_info->library.vk_get_device_proc_addr(p_info->logic_device.vk_logic_device, "vkDestroyCommandPool");
		if (destroy_command_pool != nullptr) {
			if (command_pool != VK_NULL_HANDLE) {
				destroy_command_pool(p_info->logic_device.vk_logic_device, command_pool, nullptr);
			}
		}
	}

	return;
}

void cg_destroy(cg_info_t *p_info) {
#ifdef VK_USE_PLATFORM_XCB_KHR
	xcb_disconnect(p_info->wsi.xcb_surface_create_info.connection);
#endif // VK_USE_PLATFORM_XCB_KHR
	PFN_vkDestroySurfaceKHR destroy_surface = nullptr;
	PFN_vkDestroyDevice destroy_device = nullptr;
	if (p_info->wsi.swapchain != VK_NULL_HANDLE) {
		cg_destroy_swapchain(p_info, p_info->wsi.swapchain);
		p_info->wsi.swapchain = VK_NULL_HANDLE;
	}
	if (p_info->command_pool.command_pool != VK_NULL_HANDLE) {
		goto destroy_command_pool;
	}
	if (p_info->wsi.surface != VK_NULL_HANDLE) {
		goto destroy_surface;
	}
	if (p_info->physical_device.physical_device != VK_NULL_HANDLE || p_info->logic_device.vk_logic_device != VK_NULL_HANDLE) {
		goto destroy_device;
	}
	if (p_info->instance.vk_instance != VK_NULL_HANDLE) {
		goto destroy_instance;
	}
	if (p_info->library.vulkan_library != nullptr) {
		goto destroy_vulkan_library;
	} else {
		goto exit;
	}

destroy_command_pool:
	cg_destroy_command_pool(p_info, p_info->command_pool.command_pool);
	p_info->command_pool.command_pool = VK_NULL_HANDLE;

destroy_surface:
	destroy_surface = (PFN_vkDestroySurfaceKHR)p_info->library.vk_get_instance_proc_addr(p_info->instance.vk_instance, "vkDestroySurfaceKHR");
	if (destroy_surface != nullptr) {
		destroy_surface(p_info->instance.vk_instance, p_info->wsi.surface, nullptr);
		p_info->wsi.surface = VK_NULL_HANDLE;
	}

destroy_device:
	destroy_device = (PFN_vkDestroyDevice)p_info->library.vk_get_device_proc_addr(p_info->logic_device.vk_logic_device, "vkDestroyDevice");
	if (destroy_device != nullptr) {
		if (p_info->logic_device.vk_logic_device != VK_NULL_HANDLE) {
			destroy_device(p_info->logic_device.vk_logic_device, nullptr);
		}
	}

	p_info->logic_device.vk_logic_device = VK_NULL_HANDLE;

destroy_instance:
	cg_destroy_instance(p_info, p_info->instance.vk_instance);
	p_info->instance.vk_instance = VK_NULL_HANDLE;

destroy_vulkan_library:
#ifdef __linux
	dlclose(p_info->library.vulkan_library);
#endif // __linux

#ifdef _WIN32
	FreeLibrary(p_info->library.vulkan_library);
#endif // _WIN32
	p_info->library.vulkan_library = nullptr;

exit:
	return;
}
