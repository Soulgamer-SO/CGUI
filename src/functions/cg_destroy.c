#include "cg_destroy.h"

void cg_destroy_instance(cg_var_t *p_var, VkInstance vk_instance) {
	if (vk_instance != VK_NULL_HANDLE && p_var->instance_var.vk_instance != VK_NULL_HANDLE) {
		PFN_vkDestroyInstance destroy_instance = NULL;
		destroy_instance = (PFN_vkDestroyInstance)p_var->library_var.get_instance_proc_addr(p_var->instance_var.vk_instance, "vkDestroyInstance");
		if (destroy_instance != NULL) {
			destroy_instance(vk_instance, NULL);
		}
	}

	return;
}

void cg_destroy_swapchain(cg_var_t *p_var, VkSwapchainKHR swapchain) {
	if (swapchain != VK_NULL_HANDLE && p_var->logic_device_var.vk_logic_device != VK_NULL_HANDLE) {
		PFN_vkDestroySwapchainKHR destroy_swapchain = NULL;
		destroy_swapchain = (PFN_vkDestroySwapchainKHR)p_var->library_var.get_device_proc_addr(p_var->logic_device_var.vk_logic_device, "vkDestroySwapchainKHR");
		if (destroy_swapchain != NULL) {
			destroy_swapchain(p_var->logic_device_var.vk_logic_device, swapchain, NULL);
		}
	}

	return;
}

void cg_destroy_command_pool(cg_var_t *p_var, VkCommandPool command_pool) {
	if (command_pool != VK_NULL_HANDLE && p_var->logic_device_var.vk_logic_device != VK_NULL_HANDLE) {
		PFN_vkDestroyCommandPool destroy_command_pool = NULL;
		destroy_command_pool = (PFN_vkDestroyCommandPool)p_var->library_var.get_device_proc_addr(p_var->logic_device_var.vk_logic_device, "vkDestroyCommandPool");
		if (destroy_command_pool != NULL) {
			if (p_var->command_pool_var.command_pool != VK_NULL_HANDLE) {
				destroy_command_pool(p_var->logic_device_var.vk_logic_device, p_var->command_pool_var.command_pool, NULL);
			}
		}
	}

	return;
}

void cg_destroy_and_exit(cg_var_t *p_var) {
#ifdef VK_USE_PLATFORM_XCB_KHR
	xcb_disconnect(p_var->wsi_var.xcb_surface_create_info.connection);
#endif // VK_USE_PLATFORM_XCB_KHR
	PFN_vkDestroySurfaceKHR destroy_surface = NULL;
	PFN_vkDestroyDevice destroy_device = NULL;
	if (p_var->wsi_var.swapchain != VK_NULL_HANDLE) {
		cg_destroy_swapchain(p_var, p_var->wsi_var.swapchain);
		p_var->wsi_var.swapchain = VK_NULL_HANDLE;
	}
	if (p_var->command_pool_var.command_pool != VK_NULL_HANDLE) {
		goto destroy_command_pool;
	}
	if (p_var->wsi_var.surface != VK_NULL_HANDLE) {
		goto destroy_surface;
	}
	if (p_var->physical_device_var.physical_device != VK_NULL_HANDLE || p_var->logic_device_var.vk_logic_device != VK_NULL_HANDLE) {
		goto destroy_device;
	}
	if (p_var->instance_var.vk_instance != VK_NULL_HANDLE) {
		goto destroy_instance;
	}
	if (p_var->library_var.vulkan_library != NULL) {
		goto destroy_vulkan_library;
	} else {
		goto exit;
	}

destroy_command_pool:
	cg_destroy_command_pool(p_var, p_var->command_pool_var.command_pool);
	p_var->command_pool_var.command_pool = VK_NULL_HANDLE;

destroy_surface:
	destroy_surface = (PFN_vkDestroySurfaceKHR)p_var->library_var.get_instance_proc_addr(p_var->instance_var.vk_instance, "vkDestroySurfaceKHR");
	if (destroy_surface != NULL) {
		destroy_surface(p_var->instance_var.vk_instance, p_var->wsi_var.surface, NULL);
		p_var->wsi_var.surface = VK_NULL_HANDLE;
	}

destroy_device:
	destroy_device = (PFN_vkDestroyDevice)p_var->library_var.get_device_proc_addr(p_var->logic_device_var.vk_logic_device, "vkDestroyDevice");
	if (destroy_device != NULL) {
		if (p_var->logic_device_var.vk_logic_device != VK_NULL_HANDLE) {
			destroy_device(p_var->logic_device_var.vk_logic_device, NULL);
		}
	}

	p_var->logic_device_var.vk_logic_device = VK_NULL_HANDLE;

destroy_instance:
	cg_destroy_instance(p_var, p_var->instance_var.vk_instance);
	p_var->instance_var.vk_instance = VK_NULL_HANDLE;

destroy_vulkan_library:
#ifdef __linux
	dlclose(p_var->library_var.vulkan_library);
#endif // __linux

#ifdef _WIN32
	FreeLibrary(p_var->library_var.vulkan_library);
#endif // _WIN32
	p_var->library_var.vulkan_library = NULL;

exit:
	return;
}
