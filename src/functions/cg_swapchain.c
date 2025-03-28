#include "cg_swapchain.h"

bool cg_select_swapchain(cg_var_t *p_var) {
	p_var->wsi_var.old_swapchain = VK_NULL_HANDLE;

	// 设置交换链图像的数量
	p_var->wsi_var.enabled_image_count = p_var->wsi_var.surface_capabilities.maxImageCount + 1;
	if (p_var->wsi_var.enabled_image_count > p_var->wsi_var.surface_capabilities.maxImageCount &&
	    p_var->wsi_var.surface_capabilities.maxImageCount > 0) {
		p_var->wsi_var.enabled_image_count = p_var->wsi_var.surface_capabilities.maxImageCount;
	}

	// 设置交换链图像的尺寸
	if (p_var->wsi_var.surface_capabilities.currentExtent.width == 0xFFFFFFFF) {
		p_var->wsi_var.enabled_image_extent_size.width = 1280;
		p_var->wsi_var.enabled_image_extent_size.height = 720;
	}
	if (p_var->wsi_var.enabled_image_extent_size.width > p_var->wsi_var.surface_capabilities.maxImageExtent.width) {
		p_var->wsi_var.enabled_image_extent_size.width = p_var->wsi_var.surface_capabilities.maxImageExtent.width;
	} else if (p_var->wsi_var.enabled_image_extent_size.width < p_var->wsi_var.surface_capabilities.minImageExtent.width) {
		p_var->wsi_var.enabled_image_extent_size.width = p_var->wsi_var.surface_capabilities.minImageExtent.width;
	}

	if (p_var->wsi_var.enabled_image_extent_size.height > p_var->wsi_var.surface_capabilities.maxImageExtent.height) {
		p_var->wsi_var.enabled_image_extent_size.height = p_var->wsi_var.surface_capabilities.maxImageExtent.height;
	} else if (p_var->wsi_var.enabled_image_extent_size.height < p_var->wsi_var.surface_capabilities.minImageExtent.height) {
		p_var->wsi_var.enabled_image_extent_size.height = p_var->wsi_var.surface_capabilities.minImageExtent.height;
	} else {
		p_var->wsi_var.enabled_image_extent_size = p_var->wsi_var.surface_capabilities.currentExtent;
	}

	// 设置交换链图像的功能
	if (p_var->wsi_var.surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) {
		p_var->wsi_var.enabled_image_usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		PRINT_LOG("enabled_image_usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;\n");
	}
	// 设置交换链图像的变换
	p_var->wsi_var.enabled_surface_transform = p_var->wsi_var.surface_capabilities.currentTransform;

	// 获得支持的交换链图像的格式的列表
	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR get_physical_device_surface_formats = NULL;
	get_physical_device_surface_formats = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)p_var->library_var.get_instance_proc_addr(p_var->instance_var.vk_instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
	if (get_physical_device_surface_formats == NULL) {
		PRINT_ERROR("load vkGetPhysicalDeviceSurfaceFormatsKHR fail!\n");
		return FALSE;
	}
	p_var->wsi_var.surface_format_count = 0;
	p_var->library_var.vk_result = get_physical_device_surface_formats(
		p_var->physical_device_var.physical_device,
		p_var->wsi_var.surface,
		&p_var->wsi_var.surface_format_count,
		NULL);
	if (p_var->library_var.vk_result != VK_SUCCESS || p_var->wsi_var.surface_format_count == 0) {
		PRINT_ERROR("surface_format_count fail!\n");
		return FALSE;
	}
	p_var->wsi_var.surface_format_list = (VkSurfaceFormatKHR *)cg_alloc_memory(
		p_var->p_memory_pool_var,
		p_var->wsi_var.surface_format_count * sizeof(VkSurfaceFormatKHR));
	p_var->library_var.vk_result = get_physical_device_surface_formats(
		p_var->physical_device_var.physical_device, p_var->wsi_var.surface,
		&p_var->wsi_var.surface_format_count, p_var->wsi_var.surface_format_list);
	if (p_var->library_var.vk_result != VK_SUCCESS || p_var->wsi_var.surface_format_list == NULL) {
		PRINT_ERROR("surface_format_list fail!\n");
		return FALSE;
	} else {
		PRINT_LOG("alloc memory success!\n");
	}

#ifdef DEBUG
	for (uint32_t i = 0; i < p_var->wsi_var.surface_format_count; i++) {
		PRINT_LOG("format_list[%d] = %d;\n", i, p_var->wsi_var.surface_format_list[i].format);
	}
#endif

	// 选择交换链图像的格式 VK_FORMAT_B8G8R8A8_UNORM
	p_var->wsi_var.enabled_surface_format.format = VK_FORMAT_B8G8R8A8_UNORM;

	// 判断是否支持自己想要的交换链图像的格式,VK_FORMAT_UNDEFINED 表示支持任意格式
	bool is_surface_format_supported = FALSE;
	for (uint32_t i = 0; i < p_var->wsi_var.surface_format_count; i++) {
		if (p_var->wsi_var.surface_format_list[i].format == p_var->wsi_var.enabled_surface_format.format) {
			p_var->wsi_var.enabled_surface_format.colorSpace = p_var->wsi_var.surface_format_list[i].colorSpace;
			is_surface_format_supported = TRUE;
			break;
		}
	}

	if (is_surface_format_supported == TRUE) {
		PRINT_LOG("is_surface_format_supported = TRUE!\n");
	} else {
		PRINT_LOG("is_surface_format_supported = FALSE!\n");
		return FALSE;
	}

	return TRUE;
}

bool cg_create_swapchain(cg_var_t *p_var, VkSwapchainKHR *p_swapchain) {
	PFN_vkCreateSwapchainKHR create_swapchain = NULL;
	create_swapchain = (PFN_vkCreateSwapchainKHR)p_var->library_var.get_device_proc_addr(p_var->logic_device_var.vk_logic_device, "vkCreateSwapchainKHR");
	if (create_swapchain == NULL) {
		PRINT_ERROR("load vkCreateSwapchainKHR fail!\n");
		return FALSE;
	}

	*p_swapchain = VK_NULL_HANDLE;
	p_var->library_var.vk_result = create_swapchain(
		p_var->logic_device_var.vk_logic_device,
		&(VkSwapchainCreateInfoKHR){
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.pNext = NULL,
			.flags = 0,
			.surface = p_var->wsi_var.surface,
			.minImageCount = p_var->wsi_var.enabled_image_count,
			.imageFormat = p_var->wsi_var.enabled_surface_format.format,
			.imageColorSpace = p_var->wsi_var.enabled_surface_format.colorSpace,
			.imageExtent = p_var->wsi_var.enabled_image_extent_size,
			.imageArrayLayers = 1,
			.imageUsage = p_var->wsi_var.enabled_image_usage,
			.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = NULL,
			.preTransform = p_var->wsi_var.enabled_surface_transform,
			.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.presentMode = p_var->wsi_var.enabled_present_mode,
			.clipped = VK_TRUE,
			.oldSwapchain = p_var->wsi_var.old_swapchain},
		NULL, p_swapchain);
	if (p_var->library_var.vk_result != VK_SUCCESS || p_swapchain == VK_NULL_HANDLE) {
		PRINT_ERROR("create swapchain fail!\n");
		return FALSE;
	}

	// destroy_swapchain(p_var, p_var->wsi_var.old_swapchain);
	// p_var->wsi_var.old_swapchain = VK_NULL_HANDLE;

	// 获得交换链图像的列表
	PFN_vkGetSwapchainImagesKHR get_swapchain_image_list = NULL;
	get_swapchain_image_list = (PFN_vkGetSwapchainImagesKHR)p_var->library_var.get_device_proc_addr(p_var->logic_device_var.vk_logic_device, "vkGetSwapchainImagesKHR");
	if (get_swapchain_image_list == NULL) {
		PRINT_ERROR("load vkGetSwapchainImagesKHR fail!\n");
		return FALSE;
	}
	p_var->wsi_var.swapchain_image_count = 0;
	p_var->library_var.vk_result = get_swapchain_image_list(
		p_var->logic_device_var.vk_logic_device,
		p_var->wsi_var.swapchain,
		&p_var->wsi_var.swapchain_image_count, NULL);
	if (p_var->library_var.vk_result != VK_SUCCESS || p_var->wsi_var.swapchain_image_count == 0) {
		PRINT_ERROR("swapchain_image_count fail!\n");
		return FALSE;
	}
	p_var->wsi_var.swapchain_image_list = (VkImage *)cg_alloc_memory(p_var->p_memory_pool_var, p_var->wsi_var.swapchain_image_count * sizeof(VkImage));
	if (p_var->wsi_var.swapchain_image_list != NULL) {
		p_var->library_var.vk_result = get_swapchain_image_list(
			p_var->logic_device_var.vk_logic_device, p_var->wsi_var.swapchain,
			&p_var->wsi_var.swapchain_image_count, p_var->wsi_var.swapchain_image_list);
		if (p_var->library_var.vk_result != VK_SUCCESS || p_var->wsi_var.swapchain_image_list == NULL) {
			PRINT_ERROR("swapchain_image_list fail!\n");
			return FALSE;
		} else {
			PRINT_LOG("alloc memory success!\n");
		}

#ifdef DEBUG
		for (uint32_t i = 0; i < p_var->wsi_var.swapchain_image_count; i++) {
			PRINT_LOG("swapchain_image_list[%d] address = %p;\n", i, p_var->wsi_var.swapchain_image_list[i]);
		}
#endif // DEBUG
	}

	// 获得下个用于显示的图像的索引
	PFN_vkAcquireNextImageKHR acquire_next_image = NULL;
	acquire_next_image = (PFN_vkAcquireNextImageKHR)p_var->library_var.get_device_proc_addr(p_var->logic_device_var.vk_logic_device, "vkAcquireNextImageKHR");
	if (acquire_next_image == NULL) {
		PRINT_ERROR("load vkAcquireNextImageKHR fail!\n");
		return FALSE;
	}
	p_var->library_var.vk_result = acquire_next_image(
		p_var->logic_device_var.vk_logic_device,
		p_var->wsi_var.swapchain,
		2000000000,		  // p_var->sync_var.timeout,
		VK_SEMAPHORE_TYPE_BINARY, // p_var->sync_var.semaphore_list[0],
		VK_NULL_HANDLE,		  // p_var->sync_var.fence_list[0],
		&p_var->wsi_var.image_index);
	switch (p_var->library_var.vk_result) {
	case VK_SUCCESS:
	case VK_SUBOPTIMAL_KHR:
		break;
	default:
		PRINT_ERROR("vkAcquireNextImageKHR fail!\n");
		return FALSE;
	}

	return TRUE;
}

bool cg_select_present_mode(cg_var_t *p_var) {
	p_var->wsi_var.present_mode_count = 0;
	PFN_vkGetPhysicalDeviceSurfacePresentModesKHR get_physical_device_surface_present_modes = NULL;
	get_physical_device_surface_present_modes = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)p_var->library_var.get_instance_proc_addr(
		p_var->instance_var.vk_instance, "vkGetPhysicalDeviceSurfacePresentModesKHR");
	if (get_physical_device_surface_present_modes == NULL) {
		PRINT_ERROR("load vkGetPhysicalDeviceSurfacePresentModesKHR fail!\n");
		return FALSE;
	}
	p_var->library_var.vk_result = get_physical_device_surface_present_modes(
		p_var->physical_device_var.physical_device, p_var->wsi_var.surface,
		&p_var->wsi_var.present_mode_count, NULL);
	if (p_var->library_var.vk_result != VK_SUCCESS || p_var->wsi_var.present_mode_count == 0) {
		PRINT_ERROR("present_mode_count fail!\n");
		return FALSE;
	}

	// 获得Vulkan显示模式的列表
	p_var->wsi_var.present_mode_list = (VkPresentModeKHR *)cg_alloc_memory(
		p_var->p_memory_pool_var,
		p_var->wsi_var.present_mode_count * sizeof(VkPresentModeKHR));
	if (p_var->wsi_var.present_mode_list == NULL) {
		return FALSE;
	} else {
		PRINT_LOG("alloc memory success!\n");
	}

	p_var->library_var.vk_result = get_physical_device_surface_present_modes(
		p_var->physical_device_var.physical_device, p_var->wsi_var.surface,
		&p_var->wsi_var.present_mode_count, p_var->wsi_var.present_mode_list);
	if (p_var->library_var.vk_result != VK_SUCCESS || p_var->wsi_var.present_mode_list == NULL) {
		PRINT_ERROR("present_mode_list fail!\n");
		return FALSE;
	}

	// 遍历打印Vulkan显示模式的列表
#ifdef DEBUG
	for (uint32_t i = 0; i < p_var->wsi_var.present_mode_count; i++) {
		switch (p_var->wsi_var.present_mode_list[i]) {
		case VK_PRESENT_MODE_IMMEDIATE_KHR:
			PRINT_LOG("present_mode_list[%d] = VK_PRESENT_MODE_IMMEDIATE_KHR;\n", i);
			break;
		case VK_PRESENT_MODE_MAILBOX_KHR:
			PRINT_LOG("present_mode_list[%d] = VK_PRESENT_MODE_MAILBOX_KHR;\n", i);
			break;
		case VK_PRESENT_MODE_FIFO_KHR:
			PRINT_LOG("present_mode_list[%d] = VK_PRESENT_MODE_FIFO_KHR;\n", i);
			break;
		case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
			PRINT_LOG("present_mode_list[%d] = VK_PRESENT_MODE_FIFO_RELAXED_KHR;\n", i);
			break;
		case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR:
			PRINT_LOG("present_mode_list[%d] = VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR;\n", i);
			break;
		case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR:
			PRINT_LOG("present_mode_list[%d] = VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR;\n", i);
			break;
		case VK_PRESENT_MODE_MAX_ENUM_KHR:
			PRINT_LOG("present_mode_list[%d] = VK_PRESENT_MODE_MAX_ENUM_KHR;\n", i);
			break;
		}
	}
#endif // DEBUG

	// 选择想要的显示模式
	for (uint32_t i = 0; i < p_var->wsi_var.present_mode_count; i++) {
		if (p_var->wsi_var.present_mode_list[i] == VK_PRESENT_MODE_FIFO_KHR) {
			p_var->wsi_var.enabled_present_mode = VK_PRESENT_MODE_FIFO_KHR;
			PRINT_LOG("enabled_present_mode = VK_PRESENT_MODE_FIFO_KHR;\n");
			break;
		} else {
			p_var->wsi_var.enabled_present_mode = VK_PRESENT_MODE_FIFO_KHR;
			PRINT_LOG("enabled_present_mode = VK_PRESENT_MODE_FIFO_KHR;\n");
			break;
		}
	}

	// 获得支持的显示功能
	PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR get_physical_device_surface_capabilities = NULL;
	get_physical_device_surface_capabilities = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)p_var->library_var.get_instance_proc_addr(p_var->instance_var.vk_instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
	if (get_physical_device_surface_capabilities == NULL) {
		PRINT_ERROR("load vkGetPhysicalDeviceSurfaceCapabilitiesKHR fail!\n");
		return FALSE;
	}
	p_var->library_var.vk_result = get_physical_device_surface_capabilities(
		p_var->physical_device_var.physical_device,
		p_var->wsi_var.surface,
		&p_var->wsi_var.surface_capabilities);
	if (p_var->library_var.vk_result != VK_SUCCESS) {
		PRINT_ERROR("surface_capabilities fail!\n");
		return FALSE;
	}

	return TRUE;
}
