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

#include "../functions/cg_destroy.h"
#include "../functions/cg_event_loop.h"
#include "../functions/cg_initialize.h"
#include "../functions/cg_log.h"
#include "../functions/cg_memory.h"
#include "../functions/cg_platform.h"
#define CG_MEMORY_POOL_SIZE (4ULL * 1024 * 1024 * 1024)
#define CG_MAX_FREE_MEM_NODE_COUNT (4U * 1024)

MAIN {
    cg_info_t info;
    cg_memory_pool_info_t memory_pool = {
        .memory_pool = nullptr,
        .size = CG_MEMORY_POOL_SIZE,
        .free_size = 0,
        .memory_count = 0,
        .p_last_memory_node = nullptr,
        .free_memory_node_count = 0,
        .free_memory_node_addr_array = nullptr,
        .free_memory_node_addr_max_count = CG_MAX_FREE_MEM_NODE_COUNT};
    memory_pool.memory_pool = calloc(1, memory_pool.size);
    memory_pool.free_memory_node_addr_array = calloc(CG_MAX_FREE_MEM_NODE_COUNT, sizeof(cg_memory_node_t *));
    if (cg_create_memory_pool(&memory_pool) == false) {
        goto exit;
    } else {
        info.p_memory_pool = &memory_pool;
        PRINT_LOG("create memory_pool success!\n");
    }

#ifdef WINDOWS
    info.wsi.WinAPI_info.hInstance = hInstance,
    info.wsi.WinAPI_info.hPrevInstance = hPrevInstance;
    info.wsi.WinAPI_info.pCmdLine = pCmdLine;
    info.wsi.WinAPI_info.nCmdShow = nCmdShow;
#endif // WINDOWS
    bool is_init = false;
    is_init = cg_initialize_var(&info);
    if (is_init == false) {
        PRINT_ERROR("initialize fail!\n");
        goto destroy_memory_pool;
    }

    cg_event_loop(&info);
    cg_destroy(&info);
destroy_memory_pool:
    free(memory_pool.memory_pool);
    memory_pool.memory_pool = nullptr;
    free(memory_pool.free_memory_node_addr_array);
    memory_pool.free_memory_node_addr_array = nullptr;
exit:
    PRINT_LOG("Exit success!\n");
    return EXIT_SUCCESS;
}
