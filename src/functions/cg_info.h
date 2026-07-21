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

#ifndef CG_INFO_H
#define CG_INFO_H 1
#include "cg_memory.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>
#ifdef __linux
#include <xcb/xcb_icccm.h>
#endif // __linux

#include "cg_load_library_info.h"

#include "cg_instance_info.h"

#include "cg_physical_device_info.h"

#include "cg_logic_device_info.h"

#include "cg_command_pool_info.h"

#include "cg_sync_info.h"

#include "cg_wsi_info.h"

#include "cg_event_loop_info.h"

// cgui-app info
typedef struct cg_info {
    cg_memory_pool_info_t *p_memory_pool;
    library_info_t library;
    vk_instance_info_t instance;
    vk_physical_device_info_t physical_device;
    vk_logic_device_info_t logic_device;
    vk_command_pool_info_t command_pool;
    sync_info_t sync;
    wsi_info_t wsi;
    event_loop_info_t event_loop;
} cg_info_t;

#endif // CG_INFO_H 1
