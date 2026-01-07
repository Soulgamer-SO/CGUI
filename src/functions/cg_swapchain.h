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

#ifndef CG_SWAPCHAIN_H
#define CG_SWAPCHAIN_H 1
#include "cg_var.h"
#ifdef __linux
#include <dlfcn.h>
#endif // __linux

// 选择想要的交换链
bool cg_select_swapchain(cg_var_t *p_var);

// 创建交换链
bool cg_create_swapchain(cg_var_t *p_var, VkSwapchainKHR *p_swapchain);

// 选择想要的显示模式
bool cg_select_present_mode(cg_var_t *p_var);

#endif // CG_SWAPCHAIN_H
