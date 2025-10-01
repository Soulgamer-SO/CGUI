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

#ifndef CG_DESTROY_H
#define CG_DESTROY_H 1
#include "cg_var.h"
#ifdef __linux
#include <dlfcn.h>
#endif // __linux

// 销毁Vulkan实例
void cg_destroy_instance(cg_var_t *p_var, VkInstance vk_instance);

// 销毁交换链
void cg_destroy_swapchain(cg_var_t *p_var, VkSwapchainKHR swapchain);

// 销毁命令池
void cg_destroy_command_pool(cg_var_t *p_var, VkCommandPool command_pool);

// 销毁
void cg_destroy(cg_var_t *p_var);

#endif // CG_DESTROY_H 1
