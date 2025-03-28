#include "cg_physical_device.h"

bool cg_enumerate_physical_device(cg_var_t *p_var, uint32_t *p_physical_device_count, VkPhysicalDevice *available_physical_device_list) {
	// 确认可用物理设备句柄名单之前，先加载实例级函数 PFN_vkEnumeratePhysicalDevices()
	PFN_vkEnumeratePhysicalDevices enumerate_physical_devices = NULL;
	enumerate_physical_devices = (PFN_vkEnumeratePhysicalDevices)p_var->library_var.get_instance_proc_addr(p_var->instance_var.vk_instance, "vkEnumeratePhysicalDevices");
	if (enumerate_physical_devices == NULL) {
		PRINT_ERROR("load vkEnumeratePhysicalDevices fail!\n");
		return FALSE;
	}
	p_var->library_var.vk_result = enumerate_physical_devices(p_var->instance_var.vk_instance, p_physical_device_count, available_physical_device_list);
	if (p_var->library_var.vk_result != VK_SUCCESS || *p_physical_device_count == 0) {
		PRINT_ERROR("get available_handle_device_list fail!\n");
		return FALSE;
	}

	return TRUE;
}

bool cg_select_physical_device(cg_var_t *p_var, uint32_t *p_physical_device_count, VkPhysicalDevice *available_physical_device_list, VkPhysicalDevice *p_physical_device) {

	// 初始化物理设备(选择默认显卡并初始)
	p_var->physical_device_var.physical_device_index = 0;
	p_var->physical_device_var.physical_device = p_var->physical_device_var.available_physical_device_list[p_var->physical_device_var.physical_device_index];
	p_var->physical_device_var.other_physical_device_index = 0;
	p_var->physical_device_var.other_physical_device = p_var->physical_device_var.available_physical_device_list[p_var->physical_device_var.other_physical_device_index];

	// 获取物理设备扩展列表
	p_var->physical_device_var.physical_device_extensions_count = 0;
	PFN_vkEnumerateDeviceExtensionProperties enumerate_device_extension_properties = NULL;
	enumerate_device_extension_properties = (PFN_vkEnumerateDeviceExtensionProperties)p_var->library_var.get_instance_proc_addr(p_var->instance_var.vk_instance, "vkEnumerateDeviceExtensionProperties");
	if (enumerate_device_extension_properties == NULL) {
		PRINT_ERROR("load vkEnumerateDeviceExtensionProperties fail!\n");
		return FALSE;
	}

	p_var->library_var.vk_result = enumerate_device_extension_properties(p_var->physical_device_var.physical_device, NULL, &p_var->physical_device_var.physical_device_extensions_count, NULL);
	if (p_var->library_var.vk_result != VK_SUCCESS || p_var->physical_device_var.physical_device_extensions_count == 0) {
		PRINT_ERROR("get extensions_device_count fail!\n");
		return FALSE;
	}

	PFN_vkGetPhysicalDeviceFeatures get_physical_device_features = NULL;
	get_physical_device_features = (PFN_vkGetPhysicalDeviceFeatures)p_var->library_var.get_instance_proc_addr(p_var->instance_var.vk_instance, "vkGetPhysicalDeviceFeatures");
	if (get_physical_device_features == NULL) {
		PRINT_ERROR("load vkGetPhysicalDeviceFeatures fail!\n");
		return FALSE;
	}

	// 获取物理设备属性
	PFN_vkGetPhysicalDeviceProperties get_physical_device_properties = NULL;
	get_physical_device_properties = (PFN_vkGetPhysicalDeviceProperties)p_var->library_var.get_instance_proc_addr(p_var->instance_var.vk_instance, "vkGetPhysicalDeviceProperties");
	if (get_physical_device_properties == NULL) {
		PRINT_ERROR("load vkGetPhysicalDeviceProperties fail!\n");
		return FALSE;
	}

#ifdef DEBUG
	// 统计有几个物理设备
	for (uint32_t i = 0; i < p_var->physical_device_var.physical_device_count; i++) {
		PRINT_LOG("第%d个物理设备句柄 physical_device[%d] %p\n", (i + 1), i, available_physical_device_list[i]);
	}
#endif // DEBUG

	// 检查物理设备功能和属性,选择想要的物理设备
	for (p_var->physical_device_var.physical_device_index = 0;
	     p_var->physical_device_var.physical_device_index < *p_physical_device_count;
	     p_var->physical_device_var.physical_device_index++) {
		p_var->physical_device_var.physical_device = available_physical_device_list[p_var->physical_device_var.physical_device_index];
		get_physical_device_features(p_var->physical_device_var.physical_device, &p_var->physical_device_var.device_feature_list);
		get_physical_device_properties(p_var->physical_device_var.physical_device, &p_var->physical_device_var.device_properties);
		PRINT_LOG("当前的 physical_device = physical_handle_device[%d], deviceName = %s;\n", p_var->physical_device_var.physical_device_index, p_var->physical_device_var.device_properties.deviceName);
		if (p_var->physical_device_var.device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			if (((p_var->physical_device_var.device_feature_list.geometryShader && p_var->physical_device_var.device_feature_list.textureCompressionBC) && (p_var->physical_device_var.device_feature_list.shaderFloat64 && p_var->physical_device_var.device_feature_list.multiViewport)) == TRUE) {
				PRINT_LOG("device_properties.deviceType = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;\ndevice_feature_list.geometryShader = 1;\n");
				PRINT_LOG("device_feature_list.textureCompressionBC = %d;\ndevice_feature_list.shaderFloat64 = %d;\ndevice_feature_list.multiViewport = %d;\n", p_var->physical_device_var.device_feature_list.textureCompressionBC, p_var->physical_device_var.device_feature_list.shaderFloat64, p_var->physical_device_var.device_feature_list.multiViewport);
				p_var->physical_device_var.is_physical_device_supported = TRUE;
			}
		}
		if (p_var->physical_device_var.is_physical_device_supported == TRUE) {
			PRINT_LOG("is_physical_device_supported = TRUE;\n");
			break;
		}
	}
	PRINT_LOG(
		"Selected physical_device = physical_handle_device[%d], deviceName = %s;\n",
		p_var->physical_device_var.physical_device_index,
		p_var->physical_device_var.device_properties.deviceName);

	// 获取可用的物理设备扩展列表
	p_var->library_var.vk_result = enumerate_device_extension_properties(
		p_var->physical_device_var.physical_device, NULL,
		&p_var->physical_device_var.physical_device_extensions_count, NULL);
	if (p_var->library_var.vk_result != VK_SUCCESS || p_var->physical_device_var.physical_device_extensions_count == 0) {
		PRINT_ERROR("get extensions_device_count fail!\n");
		return FALSE;
	}
	p_var->physical_device_var.available_physcial_device_extension_list = (VkExtensionProperties *)cg_alloc_memory(
		p_var->p_memory_pool_var,
		p_var->physical_device_var.physical_device_extensions_count * sizeof(VkExtensionProperties));
	if (p_var->physical_device_var.available_physcial_device_extension_list == NULL) {
		PRINT_ERROR("create available_physcial_device_extension_list fail!\n");
		return FALSE;
	} else if (p_var->physical_device_var.available_physcial_device_extension_list != NULL) {
		PRINT_LOG("alloc memory success!\n");
		p_var->library_var.vk_result = enumerate_device_extension_properties(
			p_var->physical_device_var.physical_device, NULL,
			&p_var->physical_device_var.physical_device_extensions_count,
			&p_var->physical_device_var.available_physcial_device_extension_list[0]);
		if (p_var->physical_device_var.physical_device_extensions_count == 0) {
			PRINT_ERROR("get available_physcial_device_extension_list fail!\n");
			return FALSE;
		}
	}

	// 打印可用的物理设备扩展的列表
	/* #ifdef DEBUG
		for (uint32_t i = 0; i < p_var->physical_device_var.physical_device_extensions_count; i++) {
			PRINT_LOG("available_physcial_device_extension_list[%d] %s 版本%d;\n", i, (p_var->physical_device_var.available_physcial_device_extension_list[i].extensionName), (p_var->physical_device_var.available_physcial_device_extension_list[i]).specVersion);
		}
	#endif */

	// 添加要启用的物理设备扩展列表

	// 获取物理设备内存属性
	PFN_vkGetPhysicalDeviceMemoryProperties get_physical_device_memory_properties = NULL;
	get_physical_device_memory_properties = (PFN_vkGetPhysicalDeviceMemoryProperties)p_var->library_var.get_instance_proc_addr(p_var->instance_var.vk_instance, "vkGetPhysicalDeviceMemoryProperties");
	if (get_physical_device_memory_properties != NULL) {
		get_physical_device_memory_properties(
			p_var->physical_device_var.physical_device, &p_var->physical_device_var.physical_device_memory_properties);
		if (p_var->library_var.vk_result != VK_SUCCESS) {
			PRINT_ERROR("get physical_device_memory_properties fail!\n");
			return FALSE;
		}
	}

	return TRUE;
}
