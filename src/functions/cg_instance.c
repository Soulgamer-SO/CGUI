#include "cg_instance.h"

bool cg_create_instance(cg_var_t *p_var, VkInstance *p_vk_instance) {
	p_var->library_var.vk_result = VK_SUCCESS;

	// 加载Vulkan全局函数 Vulkan只有三个全局函数 PFN_vkEnumerateInstanceExtensionProperties,PFN_vkEnumerateInstanceLayerProperties,PFN_vkCreateInstance
	PFN_vkEnumerateInstanceExtensionProperties enumerate_instance_extension_properties = NULL;
	enumerate_instance_extension_properties = (PFN_vkEnumerateInstanceExtensionProperties)p_var->library_var.get_instance_proc_addr(NULL, "vkEnumerateInstanceExtensionProperties");
	if (enumerate_instance_extension_properties == NULL) {
		PRINT_ERROR("load vkEnumerateInstanceExtensionProperties fail!\n");
		return false;
	}

	PFN_vkEnumerateInstanceLayerProperties enumerate_instance_layer_properties = NULL;
	enumerate_instance_layer_properties = (PFN_vkEnumerateInstanceLayerProperties)p_var->library_var.get_instance_proc_addr(NULL, "vkEnumerateInstanceLayerProperties");
	if (enumerate_instance_layer_properties == NULL) {
		PRINT_ERROR("load vkEnumerateInstanceLayerProperties fail!\n");
		return false;
	}

	PFN_vkCreateInstance create_instance = NULL;
	create_instance = (PFN_vkCreateInstance)p_var->library_var.get_instance_proc_addr(NULL, "vkCreateInstance");
	if (create_instance == NULL) {
		PRINT_ERROR("load vkCreateInstance fail!\n");
		return false;
	}

	p_var->instance_var.instance_extension_count = 0;
	p_var->library_var.vk_result = enumerate_instance_extension_properties(NULL, &p_var->instance_var.instance_extension_count, NULL);
	if (p_var->library_var.vk_result != VK_SUCCESS || p_var->instance_var.instance_extension_count == 0) {
		PRINT_ERROR("get instance_extensions_count fail!\n");
		return false;
	}

	p_var->instance_var.instance_extension_list = (VkExtensionProperties *)cg_alloc_memory(p_var->p_memory_pool_var, p_var->instance_var.instance_extension_count * sizeof(VkExtensionProperties));
	if (p_var->instance_var.instance_extension_list != NULL) {
		PRINT_LOG("alloc memory success!\n");
		p_var->library_var.vk_result = enumerate_instance_extension_properties(NULL, &p_var->instance_var.instance_extension_count, &p_var->instance_var.instance_extension_list[0]);
		if (p_var->library_var.vk_result != VK_SUCCESS || p_var->instance_var.instance_extension_count == 0) {
			PRINT_ERROR("get available_extension_list fail!\n");
			return false;
		}
	}

#ifdef DEBUG
	else {
		for (uint32_t i = 0; i < p_var->instance_var.instance_extension_count; i++) {
			PRINT_LOG("available_extension_list[%d] %s 版本%d;\n", i, p_var->instance_var.instance_extension_list[i].extensionName, p_var->instance_var.instance_extension_list[i].specVersion);
		}
	}
#endif // DEBUG

	p_var->instance_var.enabled_instance_extension_count = 2;
	char *enabled_extension_name_list[p_var->instance_var.enabled_instance_extension_count];
	enabled_extension_name_list[0] = VK_KHR_SURFACE_EXTENSION_NAME;
#ifdef __linux
	enabled_extension_name_list[1] = VK_KHR_XCB_SURFACE_EXTENSION_NAME;
#endif // __linux
#ifdef _WIN32
	enabled_extension_name_list[1] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
#endif // _WIN32

	/* p_var->instance_var.enabled_extension_name_list = (char***)malloc(p_var->instance_var.enabled_instance_extension_count * sizeof(char *));
	    if (p_var->instance_var.enabled_extension_name_list == NULL)
	    {
		return false;
	    }
	    p_var->instance_var.enabled_extension_name_list[0] = VK_KHR_SURFACE_EXTENSION_NAME;
	#ifdef __linux
	    p_var->instance_var.enabled_extension_name_list[1] = VK_KHR_XCB_SURFACE_EXTENSION_NAME;
	#endif // __linux
	#ifdef _WIN32
	    p_var->instance_var.enabled_extension_name_list[1] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
	#endif // _WIN32
	 p_var->instance_var.enabled_extension_name_list = enabled_extension_name_list; */

	VkApplicationInfo application_info = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pNext = NULL,
		.pApplicationName = "Soul",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName = "SoulEngine",
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = VK_MAKE_VERSION(1, 3, 261)};

	VkInstanceCreateInfo instance_create_info = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.pApplicationInfo = &application_info,
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = NULL,
		.enabledExtensionCount = p_var->instance_var.enabled_instance_extension_count,
		.ppEnabledExtensionNames = (const char *const *)enabled_extension_name_list};

	p_var->library_var.vk_result = create_instance(&instance_create_info, NULL, p_vk_instance);
	if (p_var->library_var.vk_result != VK_SUCCESS || p_vk_instance == VK_NULL_HANDLE) {
		PRINT_ERROR("create instance fail!\n");
		return false;
	} else {
#ifdef DEBUG
		// 打印已启用的实例扩展列表
		for (uint32_t i = 0; i < p_var->instance_var.enabled_instance_extension_count; i++) {
			PRINT_LOG("enabled_extension_name_list[%d] %s;\n", i, enabled_extension_name_list[i]);
		}
		PRINT_LOG("%d个实例扩展已经全部启用!\n", p_var->instance_var.enabled_instance_extension_count);
		PRINT_LOG("create instance success!\n");
#endif // DEBUG
	}

	return true;
}
