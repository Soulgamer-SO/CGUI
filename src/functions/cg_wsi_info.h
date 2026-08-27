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

#ifndef CG_WSI_INFO_H
#define CG_WSI_INFO_H 1
#include <stdint.h>
#include <vulkan/vulkan.h>
#ifdef LINUX
#include <xcb/xcb_icccm.h>
#include <xcb/xcb_util.h>
#include <xcb/xproto.h>
#endif // LINUX
#ifdef WINDOWS
#include <windows.h>
#endif // WINDOWS

typedef struct wsi_info {
    const char *window_name;
    int16_t window_x;
    int16_t window_y;
    uint16_t window_width;
    uint16_t window_height;
    uint16_t border_width;
    bool is_window_resizeable;
    // 显示表面
    VkSurfaceKHR surface;
#ifdef LINUX
    // XCB API
    struct XCB_API_info {
        int screen_num;
        uint32_t mask;
        xcb_screen_t *screen;
        xcb_size_hints_t window_size_hints;
        xcb_create_window_value_list_t value_list;
        xcb_void_cookie_t cookie;
        xcb_atom_t wm_delete_window_atom;
    } XCB_API_info;

    VkXcbSurfaceCreateInfoKHR xcb_surface_create_info;
#endif // LINUX

#ifdef WINDOWS
    // Windows API 相关
    struct WinAPI_info {
        // WinMain()函数参数
        HINSTANCE hInstance;
        HINSTANCE hPrevInstance;
        LPSTR pCmdLine;
        int nCmdShow;
        // RegisterClassEx()函数参数
        WNDCLASSEX wnd_class;
        LPCSTR w_class_name;
    } WinAPI_info;

    VkWin32SurfaceCreateInfoKHR win32_surface_create_info;
#endif // WINDOWS

    // Vulkan显示模式的数量
    uint32_t present_mode_count;
    // Vulkan显示模式的列表
    VkPresentModeKHR *present_mode_array;
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
    VkSurfaceFormatKHR *surface_format_array;
    // 创建交换链 create swapchain
    VkSwapchainKHR swapchain;
    VkSwapchainKHR old_swapchain;
    // 交换链图像数量
    uint32_t swapchain_image_count;
    // 交换链图像的句柄列表
    VkImage *swapchain_image_array;
    // 获得交换链图像
    uint32_t image_index;
    // 图像视图
    VkImageView *swapchain_image_view_array;
} wsi_info_t;

#endif // CG_WSI_INFO_H 1
