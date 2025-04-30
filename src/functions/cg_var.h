#ifndef CG_VAR_H
#define CG_VAR_H 1
#include "cg_memory_pool.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>

#ifdef __linux
#include <xcb/xcb_icccm.h>
#endif // __linux

typedef struct library_var {
	// Vulkan命令返回代码 Vulkan command return codes
	VkResult vk_result;
	// Linux下加载Vulkan函数库
#ifdef __linux
	void *vulkan_library;
#endif // __linux

// Windows下加载Vulkan函数库
#ifdef _WIN32
	HMODULE vulkan_library;
#endif // _WIN32

	// 加载实例函数的函数 PFN_vkGetInstanceProcAddr
	PFN_vkGetInstanceProcAddr vk_get_instance_proc_addr;
	// 加载逻辑设备函数的函数 PFN_vkGetDeviceProcAddr
	PFN_vkGetDeviceProcAddr vk_get_device_proc_addr;
} library_var_t;

// instance
typedef struct vk_instance_var {
	// 创建Vulkan实例
	VkInstance vk_instance;
	// 获得Vulkan实例扩展数量
	uint32_t instance_extension_count;
	// 获得所有可用实例扩展的名单
	VkExtensionProperties *instance_extension_arry;
	// 已启用的实例扩展数量
	uint32_t enabled_instance_extension_count;
	// 已启用的实例扩展的名单
	char **enabled_extension_name_arry;
} vk_instance_var_t;

// physical device
typedef struct vk_physical_device_var_t {
	// 要被选择的物理设备(显卡)
	VkPhysicalDevice physical_device;
	// 获取可用的物理设备的数量
	uint32_t physical_device_count;
	// 获取可用的物理设备的句柄名单
	VkPhysicalDevice *available_physical_device_arry;
	// 被选择的显卡的索引
	uint32_t physical_device_index;
	// 其他显卡的索引
	uint32_t other_physical_device_index;
	// 选择的其他显卡
	VkPhysicalDevice other_physical_device;
	// 可用的物理设备扩展的数量
	uint32_t physical_device_extensions_count;
	// 检查物理设备功能和属性,选择想要的物理设备
	bool is_physical_device_supported;
	VkPhysicalDeviceFeatures device_feature_arry;
	VkPhysicalDeviceProperties device_properties;
	// 可用的物理设备扩展列表
	VkExtensionProperties *available_physcial_device_extension_arry;
	// 启用的物理设备扩展的数量
	uint32_t enabled_physical_device_extensions_count;
	char **enabled_physcial_device_extension_arry;
	// 获取物理设备内存属性
	VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
} vk_physical_device_var_t;

// logic device
typedef struct vk_logic_device_var {
	// 获取队列家族和它们的属性,选择想要的队列家族
	uint32_t queue_family_count;
	VkQueueFamilyProperties *queue_family_arry;
	VkQueueFamilyProperties *queue_family_property_arry;

	// 选择想要的队列家族并返回其索引
	uint32_t queue_family_index;
	// 支持有关图形操作的队列家族的索引
	uint32_t graphic_queue_family_index;
	// 队列的句柄
	VkQueue queue_family_handle;
	// 优先级列表
	uint32_t queue_priority_arry_count;
	uint32_t queue_priority_arry_index;
	float *queue_priority_arry;

	// 创建Vulkan逻辑设备
	VkDevice vk_logic_device;
} vk_logic_device_var_t;

// command pool var
typedef struct vk_command_pool_var {
	// 命令池
	VkCommandPool command_pool;
	// 命令缓存列表
	uint32_t command_buffer_count;
	VkCommandBuffer *command_buffer_arry;
} vk_command_pool_var_t;

// sync var
typedef struct sync_var {
	// 信号量数量
	uint32_t semaphore_count;
	// 信号列表
	VkSemaphore *semaphore_arry;
	// 信号名单列表
	char **semaphore_name_arry;

	// 等待的信号量数量
	uint32_t wait_semaphore_count;
	// 等待的信号量列表
	VkSemaphore *wait_semaphore_arry;

	// 围栏列表
	VkFence *fence_arry;
	// 围栏的数量
	uint32_t fence_count;

	// 是否在等待
	bool is_wait_for;
	// 单位纳秒 1,000,000,000ns = 1s
	uint64_t timeout;

	// pipeline阶段的位掩码数量
	uint32_t semaphore_pipeline_stage_count;
	// pipeline阶段的位掩码列表
	VkPipelineStageFlags *semaphore_pipeline_stage_arry;
} sync_var_t;

// window var
typedef struct wsi_var {
	char *window_name;
	int16_t window_x;
	int16_t window_y;
	uint16_t window_width;
	uint16_t window_height;
	uint16_t border_width;
	bool is_window_resizeable;
	// 显示表面
	VkSurfaceKHR surface;
#ifdef VK_USE_PLATFORM_XCB_KHR
	// XCB API
	struct XCB_API_var {
		int screen_num;
		uint32_t mask;
		xcb_screen_t *screen;
		xcb_size_hints_t window_size_hints;
	} XCB_API_var;

	VkXcbSurfaceCreateInfoKHR xcb_surface_create_info;
#endif // VK_USE_PLATFORM_XCB_KHR

#ifdef _WIN32
	// Windows API
	struct WinAPI_var {
		// WinMain()函数参数
		HINSTANCE hInstance;
		HINSTANCE hPrevInstance;
		LPSTR pCmdLine;
		int nCmdShow;
		// RegisterClassEx()函数参数
		WNDCLASSEX wnd_class;
		LPCSTR w_class_name;
	} WinAPI_var;

	VkWin32SurfaceCreateInfoKHR win32_surface_create_info;
#endif // _WIN32

	// Vulkan显示模式的数量
	uint32_t present_mode_count;
	// Vulkan显示模式的列表
	VkPresentModeKHR *present_mode_arry;
	// 启用的显示模式
	VkPresentModeKHR enabled_present_mode;
	// 支持的显示功能
	VkSurfaceCapabilitiesKHR surface_capabilities;
	// 启用的交换链图像的数量
	uint32_t enabled_image_count;
	// 启用的交换链图像的尺寸
	VkExtent2D enabled_image_extent_size;
	// 启用的交换链图像的功能
	VkImageUsageFlags enabled_image_usage;
	// 启用的交换链图像变换
	VkSurfaceTransformFlagBitsKHR enabled_surface_transform;
	// 启用的交换链图像格式
	VkSurfaceFormatKHR enabled_surface_format;
	// 支持的交换链图像格式列表
	uint32_t surface_format_count;
	VkSurfaceFormatKHR *surface_format_arry;
	// 创建交换链 create swapchain
	VkSwapchainKHR swapchain;
	VkSwapchainKHR old_swapchain;
	// 交换链图像数量
	uint32_t swapchain_image_count;
	// 交换链图像的句柄列表
	VkImage *swapchain_image_arry;
	// 获得交换链图像
	uint32_t image_index;
	// 图像视图
	VkImageView *swapchain_image_view_arry;
} wsi_var_t;

// event loop var
typedef struct event_loop_var {
	bool is_running;
#ifdef VK_USE_PLATFORM_XCB_KHR
	xcb_generic_event_t *event;
#endif // VK_USE_PLATFORM_XCB_KHR

#ifdef _WIN32
	MSG msg;
#endif // _WIN32
} event_loop_var_t;

// var of project
typedef struct cg_var {
	cg_memory_pool_var_t *p_memory_pool_var;
	cg_gpu_memory_pool_var_t *p_gpu_memory_pool_var;
	library_var_t library_var;
	vk_instance_var_t instance_var;
	vk_physical_device_var_t physical_device_var;
	vk_logic_device_var_t logic_device_var;
	vk_command_pool_var_t command_pool_var;
	sync_var_t sync_var;
	wsi_var_t wsi_var;
	event_loop_var_t event_loop_var;
} cg_var_t;

#endif // CG_VAR_H 1
