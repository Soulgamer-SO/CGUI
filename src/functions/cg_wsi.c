#include "cg_wsi.h"
#include "cg_color.h"
#include "cg_command_pool.h"
#include "cg_destroy.h"
#include "cg_event_loop.h"
#include "cg_instance.h"
#include "cg_logic_device.h"
#include "cg_physical_device.h"
#include "cg_swapchain.h"

bool cg_create_window(cg_var_t *p_var) {
	p_var->wsi_var.surface = VK_NULL_HANDLE;
	p_var->wsi_var.window_name = "CGUI-App";
	p_var->wsi_var.window_x = 200;
	p_var->wsi_var.window_y = 200;
	p_var->wsi_var.window_width = 1280;
	p_var->wsi_var.window_height = 720;
	p_var->wsi_var.border_width = 0;
	p_var->wsi_var.is_window_resizeable = true;
	p_var->wsi_var.swapchain = VK_NULL_HANDLE;
	p_var->wsi_var.old_swapchain = VK_NULL_HANDLE;

#ifdef VK_USE_PLATFORM_XCB_KHR
	p_var->wsi_var.xcb_surface_create_info.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
	p_var->wsi_var.xcb_surface_create_info.pNext = nullptr;
	p_var->wsi_var.xcb_surface_create_info.flags = 0;
	p_var->wsi_var.XCB_API_var.screen_num = 0;
	p_var->wsi_var.xcb_surface_create_info.connection = xcb_connect(nullptr, &p_var->wsi_var.XCB_API_var.screen_num);
	bool is_xcb_connect_has_error = false;
	is_xcb_connect_has_error = xcb_connection_has_error(p_var->wsi_var.xcb_surface_create_info.connection);
	if (is_xcb_connect_has_error == true) {
		xcb_disconnect(p_var->wsi_var.xcb_surface_create_info.connection);
		PRINT_ERROR("xcb connect fail!\n");
		return false;
	}
	p_var->wsi_var.xcb_surface_create_info.window = xcb_generate_id(p_var->wsi_var.xcb_surface_create_info.connection);
	p_var->wsi_var.XCB_API_var.mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	p_var->wsi_var.XCB_API_var.screen = xcb_setup_roots_iterator(xcb_get_setup(p_var->wsi_var.xcb_surface_create_info.connection)).data;
	xcb_create_window_aux(
		p_var->wsi_var.xcb_surface_create_info.connection,
		XCB_COPY_FROM_PARENT,
		p_var->wsi_var.xcb_surface_create_info.window,
		p_var->wsi_var.XCB_API_var.screen->root,
		p_var->wsi_var.window_x, p_var->wsi_var.window_y,
		p_var->wsi_var.window_width, p_var->wsi_var.window_height,
		p_var->wsi_var.XCB_API_var.screen->root_depth,
		XCB_WINDOW_CLASS_INPUT_OUTPUT,
		p_var->wsi_var.XCB_API_var.screen->root_visual,
		p_var->wsi_var.XCB_API_var.mask,
		&(xcb_create_window_value_list_t){
			.background_pixel = cg_change_RGB_color(255, 0, 0),
			.event_mask = XCB_EVENT_MASK_EXPOSURE});
	PFN_vkCreateXcbSurfaceKHR create_xcb_surface = nullptr;
	create_xcb_surface = (PFN_vkCreateXcbSurfaceKHR)p_var->library_var.get_instance_proc_addr(p_var->instance_var.vk_instance, "vkCreateXcbSurfaceKHR");
	if (create_xcb_surface == nullptr) {
		PRINT_ERROR("load vkCreateXcbSurfaceKHR fail!\n");
		return false;
	}
	p_var->library_var.vk_result = create_xcb_surface(
		p_var->instance_var.vk_instance,
		&p_var->wsi_var.xcb_surface_create_info,
		nullptr, &p_var->wsi_var.surface);
	if (p_var->library_var.vk_result != VK_SUCCESS || p_var->wsi_var.surface == VK_NULL_HANDLE) {
		PRINT_ERROR("create xcb surface fail!\n");
		return false;
	}

	xcb_icccm_set_wm_name(
		p_var->wsi_var.xcb_surface_create_info.connection,
		p_var->wsi_var.xcb_surface_create_info.window,
		XCB_ATOM_STRING,
		8, strlen(p_var->wsi_var.window_name),
		p_var->wsi_var.window_name);
	if (p_var->wsi_var.is_window_resizeable == false) {
		xcb_icccm_size_hints_set_min_size(
			&p_var->wsi_var.XCB_API_var.window_size_hints,
			p_var->wsi_var.window_width, p_var->wsi_var.window_height);
		xcb_icccm_size_hints_set_max_size(
			&p_var->wsi_var.XCB_API_var.window_size_hints,
			p_var->wsi_var.window_width, p_var->wsi_var.window_height);
		xcb_icccm_set_wm_size_hints(
			p_var->wsi_var.xcb_surface_create_info.connection,
			p_var->wsi_var.xcb_surface_create_info.window,
			XCB_ATOM_WM_NORMAL_HINTS, &p_var->wsi_var.XCB_API_var.window_size_hints);
	}

	// 弹出窗口
	xcb_map_window(
		p_var->wsi_var.xcb_surface_create_info.connection,
		p_var->wsi_var.xcb_surface_create_info.window);
	xcb_flush(p_var->wsi_var.xcb_surface_create_info.connection);
#endif // VK_USE_PLATFORM_XCB_KHR

#ifdef VK_USE_PLATFORM_WIN32_KHR
	// 注册Windows class
	p_var->wsi_var.WinAPI_var.w_class_name = (LPCSTR) "WindowClassSoul";
	p_var->wsi_var.WinAPI_var.wnd_class = (WNDCLASSEX){
		.cbSize = sizeof(p_var->wsi_var.WinAPI_var.wnd_class),
		.style = CS_HREDRAW | CS_VREDRAW,
		.lpfnWndProc = window_proc,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = p_var->wsi_var.WinAPI_var.hInstance,
		.hIcon = LoadIcon(p_var->wsi_var.WinAPI_var.hInstance, "content/Soul.ico"),
		.hCursor = LoadCursor(nullptr, IDC_ARROW),
		.hbrBackground = CreateSolidBrush(cg_change_RGB_color(255, 0, 0)),
		.lpszMenuName = "menu",
		.lpszClassName = p_var->wsi_var.WinAPI_var.w_class_name,
		.hIconSm = LoadImage(
			p_var->wsi_var.WinAPI_var.hInstance,
			"content/Soul.ico", IMAGE_ICON,
			GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON),
			LR_DEFAULTSIZE | LR_LOADFROMFILE)};
	if (RegisterClassEx(&p_var->wsi_var.WinAPI_var.wnd_class) == false) {
		PRINT_ERROR("Windows API RegisterClassEx fail!\n");
		return false;
	}

	// 用Windows API创建窗口
	p_var->wsi_var.win32_surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	p_var->wsi_var.win32_surface_create_info.pNext = nullptr;
	p_var->wsi_var.win32_surface_create_info.flags = 0;
	p_var->wsi_var.win32_surface_create_info.hinstance = p_var->wsi_var.WinAPI_var.hInstance;
	p_var->wsi_var.win32_surface_create_info.hwnd = nullptr;
	p_var->wsi_var.win32_surface_create_info.hwnd = CreateWindowEx(
		0,
		p_var->wsi_var.WinAPI_var.w_class_name,
		(LPCSTR)p_var->wsi_var.window_name,
		WS_OVERLAPPEDWINDOW,
		p_var->wsi_var.window_x, p_var->wsi_var.window_y,
		p_var->wsi_var.window_width, p_var->wsi_var.window_height,
		(HWND)nullptr,
		(HMENU)nullptr,
		p_var->wsi_var.WinAPI_var.hInstance,
		(LPVOID)nullptr);
	if (p_var->wsi_var.win32_surface_create_info.hwnd == nullptr) {
		PRINT_ERROR("Windows API CreateWindowEx fail!\n");
		return false;
	}
	PFN_vkCreateWin32SurfaceKHR create_win32_surface = nullptr;
	create_win32_surface = (PFN_vkCreateWin32SurfaceKHR)p_var->library_var.get_instance_proc_addr(p_var->instance_var.vk_instance, "vkCreateWin32SurfaceKHR");
	if (create_win32_surface == nullptr) {
		PRINT_ERROR("load vkCreateWin32SurfaceKHR fail!\n");
		return false;
	}
	p_var->library_var.vk_result = create_win32_surface(
		p_var->instance_var.vk_instance, &p_var->wsi_var.win32_surface_create_info,
		nullptr, &p_var->wsi_var.surface);
	if (p_var->library_var.vk_result != VK_SUCCESS || p_var->wsi_var.surface == VK_NULL_HANDLE) {
		PRINT_ERROR("create win32 surface fail!\n");
		return false;
	}

	// 弹出窗口
	p_var->wsi_var.WinAPI_var.nCmdShow = SW_SHOW;
	ShowWindow(p_var->wsi_var.win32_surface_create_info.hwnd, p_var->wsi_var.WinAPI_var.nCmdShow);
	UpdateWindow(p_var->wsi_var.win32_surface_create_info.hwnd);

#endif // VK_USE_PLATFORM_WIN32_KHR

	if (cg_select_present_mode(p_var) == false) {
		return false;
	}
	if (cg_select_swapchain(p_var) == false) {
		return false;
	}
	p_var->wsi_var.swapchain = VK_NULL_HANDLE;
	p_var->wsi_var.old_swapchain = VK_NULL_HANDLE;
	if (cg_create_swapchain(p_var, &p_var->wsi_var.swapchain) == false) {
		return false;
	}

	return true;
}
