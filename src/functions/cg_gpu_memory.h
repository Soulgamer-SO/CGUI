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

#ifndef CG_GPU_MEMORY_H
#define CG_GPU_MEMORY_H 1
#include "cg_info.h"
#include "cg_log.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>
#define GPU_MEMORY_POOL_SIZE (4ULL * 1024 * 1024 * 1024)
#define MAX_FREE_GPU_MEM_NODE_COUNT (4U * 1024)
#define VK_DEVICE_ADDR_NULL 0ULL

#endif // CG_GPU_MEMORY_H 1
