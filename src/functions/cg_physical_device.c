#include "cg_physical_device.h"

bool cg_enumerate_physical_device(cg_var_t *p_var, uint32_t *p_physical_device_count, VkPhysicalDevice *available_physical_device_array) {
	// 确认可用物理设备句柄名单之前，先加载实例级函数 PFN_vkEnumeratePhysicalDevices()
	PFN_vkEnumeratePhysicalDevices enumerate_physical_devices = nullptr;
	enumerate_physical_devices = (PFN_vkEnumeratePhysicalDevices)p_var->library_var.vk_get_instance_proc_addr(p_var->instance_var.vk_instance, "vkEnumeratePhysicalDevices");
	if (enumerate_physical_devices == nullptr) {
		PRINT_ERROR("load vkEnumeratePhysicalDevices fail!\n");
		return false;
	}
	p_var->library_var.vk_result = enumerate_physical_devices(p_var->instance_var.vk_instance, p_physical_device_count, available_physical_device_array);
	if (p_var->library_var.vk_result != VK_SUCCESS || *p_physical_device_count == 0) {
		PRINT_ERROR("get available_handle_device_array fail!\n");
		return false;
	}

	return true;
}

bool cg_select_physical_device(cg_var_t *p_var, uint32_t *p_physical_device_count, VkPhysicalDevice *available_physical_device_array, VkPhysicalDevice *p_physical_device) {

	// 初始化物理设备(选择默认显卡并初始)
	p_var->physical_device_var.physical_device_index = 0;
	p_var->physical_device_var.physical_device = p_var->physical_device_var.available_physical_device_array[p_var->physical_device_var.physical_device_index];
	p_var->physical_device_var.other_physical_device_index = 0;
	p_var->physical_device_var.other_physical_device = p_var->physical_device_var.available_physical_device_array[p_var->physical_device_var.other_physical_device_index];

	// 获取物理设备扩展列表
	p_var->physical_device_var.physical_device_extensions_count = 0;
	PFN_vkEnumerateDeviceExtensionProperties enumerate_device_extension_properties = nullptr;
	enumerate_device_extension_properties = (PFN_vkEnumerateDeviceExtensionProperties)p_var->library_var.vk_get_instance_proc_addr(p_var->instance_var.vk_instance, "vkEnumerateDeviceExtensionProperties");
	if (enumerate_device_extension_properties == nullptr) {
		PRINT_ERROR("load vkEnumerateDeviceExtensionProperties fail!\n");
		return false;
	}

	p_var->library_var.vk_result = enumerate_device_extension_properties(p_var->physical_device_var.physical_device, nullptr, &p_var->physical_device_var.physical_device_extensions_count, nullptr);
	if (p_var->library_var.vk_result != VK_SUCCESS || p_var->physical_device_var.physical_device_extensions_count == 0) {
		PRINT_ERROR("get extensions_device_count fail!\n");
		return false;
	}

	PFN_vkGetPhysicalDeviceFeatures get_physical_device_features = nullptr;
	get_physical_device_features = (PFN_vkGetPhysicalDeviceFeatures)p_var->library_var.vk_get_instance_proc_addr(p_var->instance_var.vk_instance, "vkGetPhysicalDeviceFeatures");
	if (get_physical_device_features == nullptr) {
		PRINT_ERROR("load vkGetPhysicalDeviceFeatures fail!\n");
		return false;
	}

	// 获取物理设备属性
	PFN_vkGetPhysicalDeviceProperties get_physical_device_properties = nullptr;
	get_physical_device_properties = (PFN_vkGetPhysicalDeviceProperties)p_var->library_var.vk_get_instance_proc_addr(p_var->instance_var.vk_instance, "vkGetPhysicalDeviceProperties");
	if (get_physical_device_properties == nullptr) {
		PRINT_ERROR("load vkGetPhysicalDeviceProperties fail!\n");
		return false;
	}

#ifdef DEBUG
	// 统计有几个物理设备
	for (uint32_t i = 0; i < p_var->physical_device_var.physical_device_count; i++) {
		PRINT_LOG("第%d个物理设备句柄 physical_device[%d] %p\n", (i + 1), i, available_physical_device_array[i]);
	}
#endif // DEBUG

	// 检查物理设备功能和属性,选择想要的物理设备
	for (p_var->physical_device_var.physical_device_index = 0;
	     p_var->physical_device_var.physical_device_index < *p_physical_device_count;
	     p_var->physical_device_var.physical_device_index++) {
		p_var->physical_device_var.physical_device = available_physical_device_array[p_var->physical_device_var.physical_device_index];
		get_physical_device_features(p_var->physical_device_var.physical_device, &p_var->physical_device_var.device_feature_array);
		get_physical_device_properties(p_var->physical_device_var.physical_device, &p_var->physical_device_var.device_properties);
		PRINT_LOG("当前的 physical_device = physical_handle_device[%d], deviceName = %s;\n", p_var->physical_device_var.physical_device_index, p_var->physical_device_var.device_properties.deviceName);
		if (p_var->physical_device_var.device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			if (((p_var->physical_device_var.device_feature_array.geometryShader && p_var->physical_device_var.device_feature_array.textureCompressionBC) && (p_var->physical_device_var.device_feature_array.shaderFloat64 && p_var->physical_device_var.device_feature_array.multiViewport)) == true) {
				PRINT_LOG("device_properties.deviceType = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;\ndevice_feature_array.geometryShader = 1;\n");
				PRINT_LOG("device_feature_array.textureCompressionBC = %d;\ndevice_feature_array.shaderFloat64 = %d;\ndevice_feature_array.multiViewport = %d;\n", p_var->physical_device_var.device_feature_array.textureCompressionBC, p_var->physical_device_var.device_feature_array.shaderFloat64, p_var->physical_device_var.device_feature_array.multiViewport);
				p_var->physical_device_var.is_physical_device_supported = true;
			}
		}
		if (p_var->physical_device_var.is_physical_device_supported == true) {
			PRINT_LOG("is_physical_device_supported = true;\n");
			break;
		}
	}
	PRINT_LOG(
		"Selected physical_device = physical_handle_device[%d], deviceName = %s;\n",
		p_var->physical_device_var.physical_device_index,
		p_var->physical_device_var.device_properties.deviceName);

	// 获取可用的物理设备扩展列表
	p_var->library_var.vk_result = enumerate_device_extension_properties(
		p_var->physical_device_var.physical_device, nullptr,
		&p_var->physical_device_var.physical_device_extensions_count, nullptr);
	if (p_var->library_var.vk_result != VK_SUCCESS || p_var->physical_device_var.physical_device_extensions_count == 0) {
		PRINT_ERROR("get extensions_device_count fail!\n");
		return false;
	}
	p_var->physical_device_var.available_physcial_device_extension_array = (VkExtensionProperties *)cg_alloc_memory(
		p_var->p_memory_pool_var,
		p_var->physical_device_var.physical_device_extensions_count * sizeof(VkExtensionProperties));
	if (p_var->physical_device_var.available_physcial_device_extension_array == nullptr) {
		PRINT_ERROR("create available_physcial_device_extension_array fail!\n");
		return false;
	} else if (p_var->physical_device_var.available_physcial_device_extension_array != nullptr) {
		PRINT_LOG("alloc memory success!\n");
		p_var->library_var.vk_result = enumerate_device_extension_properties(
			p_var->physical_device_var.physical_device, nullptr,
			&p_var->physical_device_var.physical_device_extensions_count,
			&p_var->physical_device_var.available_physcial_device_extension_array[0]);
		if (p_var->physical_device_var.physical_device_extensions_count == 0) {
			PRINT_ERROR("get available_physcial_device_extension_arrt fail!\n");
			return false;
		}
	}

	// 打印可用的物理设备扩展的列表
	/* #ifdef DEBUG
		for (uint32_t i = 0; i < p_var->physical_device_var.physical_device_extensions_count; i++) {
			PRINT_LOG("available_physcial_device_extension_array[%d] %s 版本%d;\n", i, (p_var->physical_device_var.available_physcial_device_extension_array[i].extensionName), (p_var->physical_device_var.available_physcial_device_extension_array[i]).specVersion);
		}
	#endif */

	// 添加要启用的物理设备扩展列表

	// 获取物理设备内存属性
	PFN_vkGetPhysicalDeviceMemoryProperties get_physical_device_memory_properties = nullptr;
	get_physical_device_memory_properties = (PFN_vkGetPhysicalDeviceMemoryProperties)p_var->library_var.vk_get_instance_proc_addr(p_var->instance_var.vk_instance, "vkGetPhysicalDeviceMemoryProperties");
	if (get_physical_device_memory_properties != nullptr) {
		get_physical_device_memory_properties(
			p_var->physical_device_var.physical_device, &p_var->physical_device_var.physical_device_memory_properties);
		if (p_var->library_var.vk_result != VK_SUCCESS) {
			PRINT_ERROR("get physical_device_memory_properties fail!\n");
			return false;
		}
	}

	return true;
}
