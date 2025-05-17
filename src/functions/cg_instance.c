#include "cg_instance.h"

bool cg_create_instance(cg_var_t *p_var, VkInstance *p_vk_instance) {
	p_var->library_var.vk_result = VK_SUCCESS;

	// 加载Vulkan全局函数 Vulkan只有三个全局函数 PFN_vkEnumerateInstanceExtensionProperties,PFN_vkEnumerateInstanceLayerProperties,PFN_vkCreateInstance
	PFN_vkEnumerateInstanceExtensionProperties enumerate_instance_extension_properties = nullptr;
	enumerate_instance_extension_properties = (PFN_vkEnumerateInstanceExtensionProperties)p_var->library_var.vk_get_instance_proc_addr(nullptr, "vkEnumerateInstanceExtensionProperties");
	if (enumerate_instance_extension_properties == nullptr) {
		PRINT_ERROR("load vkEnumerateInstanceExtensionProperties fail!\n");
		return false;
	}

	PFN_vkEnumerateInstanceLayerProperties enumerate_instance_layer_properties = nullptr;
	enumerate_instance_layer_properties = (PFN_vkEnumerateInstanceLayerProperties)p_var->library_var.vk_get_instance_proc_addr(nullptr, "vkEnumerateInstanceLayerProperties");
	if (enumerate_instance_layer_properties == nullptr) {
		PRINT_ERROR("load vkEnumerateInstanceLayerProperties fail!\n");
		return false;
	}

	PFN_vkCreateInstance create_instance = nullptr;
	create_instance = (PFN_vkCreateInstance)p_var->library_var.vk_get_instance_proc_addr(nullptr, "vkCreateInstance");
	if (create_instance == nullptr) {
		PRINT_ERROR("load vkCreateInstance fail!\n");
		return false;
	}

	p_var->instance_var.instance_extension_count = 0;
	p_var->library_var.vk_result = enumerate_instance_extension_properties(nullptr, &p_var->instance_var.instance_extension_count, nullptr);
	if (p_var->library_var.vk_result != VK_SUCCESS || p_var->instance_var.instance_extension_count == 0) {
		PRINT_ERROR("get instance_extensions_count fail!\n");
		return false;
	}

	p_var->instance_var.instance_extension_arry = (VkExtensionProperties *)cg_alloc_memory(p_var->p_memory_pool_var, p_var->instance_var.instance_extension_count * sizeof(VkExtensionProperties));
	if (p_var->instance_var.instance_extension_arry != nullptr) {
		PRINT_LOG("alloc memory success!\n");
		p_var->library_var.vk_result = enumerate_instance_extension_properties(nullptr, &p_var->instance_var.instance_extension_count, &p_var->instance_var.instance_extension_arry[0]);
		if (p_var->library_var.vk_result != VK_SUCCESS || p_var->instance_var.instance_extension_count == 0) {
			PRINT_ERROR("get available_extension_arry fail!\n");
			return false;
		}
	}

#ifdef DEBUG
	else {
		for (uint32_t i = 0; i < p_var->instance_var.instance_extension_count; i++) {
			PRINT_LOG("available_extension_arry[%d] %s 版本%d;\n", i, p_var->instance_var.instance_extension_arry[i].extensionName, p_var->instance_var.instance_extension_arry[i].specVersion);
		}
	}
#endif // DEBUG

	p_var->instance_var.enabled_instance_extension_count = 2;
	char *enabled_extension_name_arry[p_var->instance_var.enabled_instance_extension_count];
	enabled_extension_name_arry[0] = VK_KHR_SURFACE_EXTENSION_NAME;
#ifdef __linux
	enabled_extension_name_arry[1] = VK_KHR_XCB_SURFACE_EXTENSION_NAME;
#endif // __linux
#ifdef _WIN32
	enabled_extension_name_arry[1] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
#endif // _WIN32

	/* p_var->instance_var.enabled_extension_name_arry = (char***)malloc(p_var->instance_var.enabled_instance_extension_count * sizeof(char *));
	    if (p_var->instance_var.enabled_extension_name_arry == nullptr)
	    {
		return false;
	    }
	    p_var->instance_var.enabled_extension_name_arry[0] = VK_KHR_SURFACE_EXTENSION_NAME;
	#ifdef __linux
	    p_var->instance_var.enabled_extension_name_arry[1] = VK_KHR_XCB_SURFACE_EXTENSION_NAME;
	#endif // __linux
	#ifdef _WIN32
	    p_var->instance_var.enabled_extension_name_arry[1] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
	#endif // _WIN32
	 p_var->instance_var.enabled_extension_name_arry = enabled_extension_name_arry; */

	VkApplicationInfo application_info = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pNext = nullptr,
		.pApplicationName = "Soul",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName = "SoulEngine",
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = VK_MAKE_VERSION(1, 3, 261)};

	VkInstanceCreateInfo instance_create_info = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.pApplicationInfo = &application_info,
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = nullptr,
		.enabledExtensionCount = p_var->instance_var.enabled_instance_extension_count,
		.ppEnabledExtensionNames = (const char *const *)enabled_extension_name_arry};

	p_var->library_var.vk_result = create_instance(&instance_create_info, nullptr, p_vk_instance);
	if (p_var->library_var.vk_result != VK_SUCCESS || p_vk_instance == VK_NULL_HANDLE) {
		PRINT_ERROR("create instance fail!\n");
		return false;
	} else {
#ifdef DEBUG
		// 打印已启用的实例扩展列表
		for (uint32_t i = 0; i < p_var->instance_var.enabled_instance_extension_count; i++) {
			PRINT_LOG("enabled_extension_name_arry[%d] %s;\n", i, enabled_extension_name_arry[i]);
		}
		PRINT_LOG("%d个实例扩展已经全部启用!\n", p_var->instance_var.enabled_instance_extension_count);
		PRINT_LOG("create instance success!\n");
#endif // DEBUG
	}

	return true;
}
