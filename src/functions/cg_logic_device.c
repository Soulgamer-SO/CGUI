#include "cg_logic_device.h"

bool cg_create_logic_device(cg_var_t *p_var, VkDevice *p_vk_logic_device) {
	p_var->logic_device_var.queue_family_count = 0;
	PFN_vkGetPhysicalDeviceQueueFamilyProperties get_physical_device_queue_family_properties = nullptr;
	get_physical_device_queue_family_properties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)p_var->library_var.vk_get_instance_proc_addr(
		p_var->instance_var.vk_instance, "vkGetPhysicalDeviceQueueFamilyProperties");
	if (get_physical_device_queue_family_properties == nullptr) {
		PRINT_ERROR("load vkGetPhysicalDeviceQueueFamilyProperties fail");
		return false;
	}

	get_physical_device_queue_family_properties(
		p_var->physical_device_var.physical_device, &p_var->logic_device_var.queue_family_count, nullptr);
	if (p_var->logic_device_var.queue_family_count == 0) {
		PRINT_ERROR("get queue_families_count fail!\n");
		return false;
	}

	p_var->logic_device_var.queue_family_array = (VkQueueFamilyProperties *)cg_alloc_memory(
		p_var->p_memory_pool_var,
		p_var->logic_device_var.queue_family_count * sizeof(VkQueueFamilyProperties));
	if (p_var->logic_device_var.queue_family_array == nullptr) {
		PRINT_ERROR("get queue_familiy_array fail!\n");
		return false;
	} else {
		PRINT_LOG("alloc memory success!\n");
	}

	get_physical_device_queue_family_properties(
		p_var->physical_device_var.physical_device, &p_var->logic_device_var.queue_family_count, p_var->logic_device_var.queue_family_array);
	if (p_var->logic_device_var.queue_family_count == 0) {
		PRINT_ERROR("get queue_familiy_array fail!\n");
		return false;
	}

	p_var->logic_device_var.queue_family_index = 0;
	p_var->logic_device_var.queue_priority_array_count = 16;
	p_var->logic_device_var.queue_priority_array_index = 0;
	p_var->logic_device_var.queue_priority_array = (float *)cg_alloc_memory(
		p_var->p_memory_pool_var,
		p_var->logic_device_var.queue_priority_array_count * sizeof(float));
	if (p_var->logic_device_var.queue_priority_array != nullptr) {
		PRINT_LOG("alloc memory success!\n");
		for (uint32_t i = 0; i < p_var->logic_device_var.queue_priority_array_count; i++) {
			p_var->logic_device_var.queue_priority_array[i] = 0.001f * (i + 1);
			PRINT_LOG("queue_priority_array[%d] = %f;\n", i, p_var->logic_device_var.queue_priority_array[i]);
		}
	}

#ifdef DEBUG
	// 打印所有队列信息
	char **queue_info = cg_alloc_memory(p_var->p_memory_pool_var, 5 * sizeof(char *));
	if (queue_info == nullptr) {
		return false;
	} else {
		PRINT_LOG("alloc memory success!\n");
	}

	for (p_var->logic_device_var.queue_family_index = 0;
	     p_var->logic_device_var.queue_family_index < p_var->logic_device_var.queue_family_count;
	     p_var->logic_device_var.queue_family_index++) {
		PRINT_LOG("============================== queue_familiy_array[%d] ==================================\n", p_var->logic_device_var.queue_family_index);
		if (p_var->logic_device_var.queue_family_array[p_var->logic_device_var.queue_family_index].queueCount > 0) {
			if (p_var->logic_device_var.queue_family_array[p_var->logic_device_var.queue_family_index].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				queue_info[0] = " VK_QUEUE_GRAPHICS_BIT ";
			} else {
				queue_info[0] = "";
			}
			if (p_var->logic_device_var.queue_family_array[p_var->logic_device_var.queue_family_index].queueFlags & VK_QUEUE_COMPUTE_BIT) {
				queue_info[1] = " VK_QUEUE_COMPUTE_BIT";
			} else {
				queue_info[1] = "";
			}
			if (p_var->logic_device_var.queue_family_array[p_var->logic_device_var.queue_family_index].queueFlags & VK_QUEUE_TRANSFER_BIT) {
				queue_info[2] = " VK_QUEUE_TRANSFER_BIT";
			} else {
				queue_info[2] = "";
			}
			if (p_var->logic_device_var.queue_family_array[p_var->logic_device_var.queue_family_index].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) {
				queue_info[3] = " VK_QUEUE_SPARSE_BINDING_BIT";
			} else {
				queue_info[3] = "";
			}
			if (p_var->logic_device_var.queue_family_array[p_var->logic_device_var.queue_family_index].queueFlags & VK_QUEUE_PROTECTED_BIT) {
				queue_info[4] = " VK_QUEUE_PROTECTED_BIT";
			} else {
				queue_info[4] = "";
			}
			PRINT_LOG("queueFlags =%s%s%s%s%s;\n", queue_info[0], queue_info[1], queue_info[2], queue_info[3], queue_info[4]);
		} else {
			PRINT_LOG("queueFlags = none;\n");
		}
		PRINT_LOG("queueCount = %d;\n", p_var->logic_device_var.queue_family_array[p_var->logic_device_var.queue_family_index].queueCount);
		PRINT_LOG("timestampValidBits = %d;\n", p_var->logic_device_var.queue_family_array[p_var->logic_device_var.queue_family_index].timestampValidBits);
		PRINT_LOG(
			"minImageTransferGranularity = { width:%d, height:%d, depth:%d };\n",
			p_var->logic_device_var.queue_family_array[p_var->logic_device_var.queue_family_index].minImageTransferGranularity.width,
			p_var->logic_device_var.queue_family_array[p_var->logic_device_var.queue_family_index].minImageTransferGranularity.height,
			p_var->logic_device_var.queue_family_array[p_var->logic_device_var.queue_family_index].minImageTransferGranularity.depth);
		PRINT_LOG("=======================================================================================\n");
	}
#endif

	// 选择支持 VK_QUEUE_GRAPHICS_BIT 的队列
	// VK_QUEUE_GRAPHICS_BIT = 0x00000001 // 二进制=1
	// VK_QUEUE_COMPUTE_BIT = 0x00000002 // 二进制=10
	// VK_QUEUE_TRANSFER_BIT = 0x00000004 // 二进制=100
	// VK_QUEUE_SPARSE_BINDING_BIT = 0x00000008 // 二进制=1000
	// VK_QUEUE_PROTECTED_BIT = 0x00000010 // 二进制=1010
	p_var->logic_device_var.graphic_queue_family_index = 0;
	for (p_var->logic_device_var.queue_family_index = 0;
	     p_var->logic_device_var.queue_family_index < p_var->logic_device_var.queue_family_count;
	     p_var->logic_device_var.queue_family_index++) {
		if (p_var->logic_device_var.queue_family_array[p_var->logic_device_var.queue_family_index].queueCount > 0 &&
		    (p_var->logic_device_var.queue_family_array[p_var->logic_device_var.queue_family_index].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
			p_var->logic_device_var.graphic_queue_family_index = p_var->logic_device_var.queue_family_index;
			break;
		}
	}

	VkDeviceQueueCreateInfo queue_create_info = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.queueFamilyIndex = p_var->logic_device_var.graphic_queue_family_index,
		.queueCount = p_var->logic_device_var.queue_family_array[p_var->logic_device_var.graphic_queue_family_index].queueCount,
		.pQueuePriorities = &p_var->logic_device_var.queue_priority_array[p_var->logic_device_var.queue_priority_array_index]};

	VkDeviceCreateInfo device_create_info = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &queue_create_info,
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = nullptr,
		.enabledExtensionCount = p_var->physical_device_var.enabled_physical_device_extensions_count,
		.ppEnabledExtensionNames = (const char *const *)&p_var->physical_device_var.enabled_physcial_device_extension_array[0],
		.pEnabledFeatures = &p_var->physical_device_var.device_feature_array};

	// create logic device
	PFN_vkCreateDevice create_device = nullptr;
	create_device = (PFN_vkCreateDevice)p_var->library_var.vk_get_instance_proc_addr(
		p_var->instance_var.vk_instance, "vkCreateDevice");
	if (create_device == nullptr) {
		PRINT_ERROR("load vkCreateDevice device fail!\n");
		return false;
	}

	p_var->library_var.vk_result = create_device(
		p_var->physical_device_var.physical_device, &device_create_info,
		nullptr, p_vk_logic_device);
	if (p_var->library_var.vk_result != VK_SUCCESS || p_vk_logic_device == VK_NULL_HANDLE) {
		PRINT_ERROR("create vulkan logic device fail!\n");
		return false;
	}

#ifdef DEBUG
	for (uint32_t i = 0; i < p_var->physical_device_var.enabled_physical_device_extensions_count; i++) {
		PRINT_LOG("enabled_physcial_device_extension_array[%d] = %s;\n", i, p_var->physical_device_var.enabled_physcial_device_extension_array[i]);
	}
	PRINT_LOG("%d个物理设备扩展已经全部启用!\n", p_var->physical_device_var.enabled_physical_device_extensions_count);
	PRINT_LOG("enabled queue familiy = queue_familiy_array[%d];\n", queue_create_info.queueFamilyIndex);
	PRINT_LOG("create vulkan logic device success!\n");
#endif // DEBUG

	// 获取队列的句柄
	PFN_vkGetDeviceQueue get_device_queue = nullptr;
	get_device_queue = (PFN_vkGetDeviceQueue)p_var->library_var.vk_get_device_proc_addr(
		*p_vk_logic_device, "vkGetDeviceQueue");
	if (get_device_queue == nullptr) {
		PRINT_ERROR("load vkGetDeviceQueue fail!\n");
		return false;
	}
	p_var->logic_device_var.queue_family_handle = nullptr;
	get_device_queue(
		*p_vk_logic_device,
		p_var->logic_device_var.queue_family_index,
		0,
		&p_var->logic_device_var.queue_family_handle);
	if (p_var->logic_device_var.queue_family_handle == nullptr) {
		PRINT_ERROR("get vulkan device queue fail!\n");
		return false;
	} else {
		PRINT_LOG("queue_family_handle = %p;\n", p_var->logic_device_var.queue_family_handle);
	}

	return true;
}
