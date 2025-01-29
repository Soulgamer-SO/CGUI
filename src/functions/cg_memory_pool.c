#include "cg_memory_pool.h"

bool_t cg_create_memory_pool(cg_memory_pool_var_t *p_var) {
	if (p_var->size <= sizeof(cg_memory_node_t)) {
		PRINT_ERROR("memory pool size should more bigger than %zu size!\n", sizeof(cg_memory_node_t));
		return FALSE;
	}
	p_var->last_memory_end_addr = NULL;
	p_var->memory_node_count = 0;
	if (p_var->is_memory_node_list_in_pool == TRUE) {
		p_var->memory_node_list = (cg_memory_node_t *)(p_var->memory_pool + p_var->size - sizeof(cg_memory_node_t));
	}
	p_var->free_size = 0;
	if (p_var->memory_pool == NULL) {
		PRINT_ERROR("create memory pool fail!\n");
		return FALSE;
	}
	p_var->last_memory_end_addr = p_var->memory_pool;
	p_var->free_size = p_var->size;
	PRINT_LOG("============================memory pool============================\n");
	PRINT_LOG("create memory_pool success!\n");
	PRINT_LOG("memory_pool = %p;\n", p_var->memory_pool);
	PRINT_LOG("last_memory_end_addr = %p;\n", p_var->last_memory_end_addr);
	PRINT_LOG("memory_pool_size = %zu;\n", p_var->size);
	PRINT_LOG("free_size = %zu;\n", p_var->free_size);
	PRINT_LOG("===================================================================\n");
	return TRUE;
}

inline cg_memory_node_t *cg_set_memory_node_list_addr(cg_memory_pool_var_t *p_var, void *addr, size_t size) {
	p_var->is_memory_node_list_in_pool = FALSE;
	return (cg_memory_node_t *)(addr + size - sizeof(cg_memory_node_t));
}

void *cg_alloc_memory(cg_memory_pool_var_t *p_var, size_t size) {
	if (p_var->memory_pool == NULL) {
		PRINT_ERROR("allocate memory fail! please create memory pool first!\n");
		return NULL;
	} else if ((size + sizeof(cg_memory_node_t)) > p_var->free_size) {
		PRINT_ERROR("allocate memory fail! the size is too big!\n");
		return NULL;
	}
	if (size == 0) {
		PRINT_ERROR("size can not be 0!\n");
		return NULL;
	}
	if (p_var->memory_node_count == 0) {
		p_var->memory_node_count = 1;
		if (p_var->is_memory_node_list_in_pool == TRUE) {
			p_var->memory_node_list = &p_var->memory_node_list[-1];
		}
		p_var->memory_node_list[0].addr = p_var->memory_pool;
		p_var->memory_node_list[0].end_addr = p_var->memory_pool + size;
		p_var->memory_node_list[0].is_used = TRUE;
		p_var->last_memory_end_addr = p_var->memory_node_list[0].end_addr;
		p_var->free_size -= (sizeof(cg_memory_node_t) + size);
		PRINT_LOG("============================memory pool============================\n");
		PRINT_LOG("memory_pool = %p;\n", p_var->memory_pool);
		PRINT_LOG("last_memory_end_addr = %p;\n", p_var->last_memory_end_addr);
		PRINT_LOG("memory_pool_size = %zu;\n", p_var->size);
		PRINT_LOG("memory_node_count = %d;\n", p_var->memory_node_count);
		PRINT_LOG("memory block size = %zu;\n", size);
		PRINT_LOG("memory_node.addr = %p;\n", p_var->memory_node_list[0].addr);
		PRINT_LOG("memory_node.end_addr = %p;\n", p_var->memory_node_list[0].end_addr);
		PRINT_LOG("memory_node.is_used = %d;\n", p_var->memory_node_list[0].is_used);
		PRINT_LOG("free_size = %zu;\n", p_var->free_size);
		PRINT_LOG("===================================================================\n");
		return p_var->memory_node_list[0].addr;
	} else if (p_var->memory_node_count >= 1) {
		bool_t is_free_mem_size_equ = FALSE;
		bool_t is_free_mem_size_bigger = FALSE;
		uint32_t i = 0;
		for (i = 0; i < p_var->memory_node_count; i++) {
			size_t memory_size = (size_t)(p_var->memory_node_list[i].end_addr - p_var->memory_node_list[i].addr);
			if (p_var->memory_node_list[i].is_used == FALSE && memory_size == size) {
				is_free_mem_size_equ = TRUE;
				break;
			} else if (p_var->memory_node_list[i].is_used == FALSE && memory_size > size) {
				is_free_mem_size_bigger = TRUE;
				break;
			}
		}
		// 如果已经释放的内存块的大小足够容纳即将被申请的内存块的大小,就优先利用这块之前被释放的内存块
		if (is_free_mem_size_equ == FALSE) {
			p_var->memory_node_count++;
		}
		if (is_free_mem_size_equ == TRUE) {
			p_var->memory_node_list[-1] = p_var->memory_node_list[i];
		} else if (is_free_mem_size_bigger == TRUE) {
			p_var->memory_node_list[-1].addr = p_var->memory_node_list[i].addr;
			p_var->memory_node_list[-1].end_addr = p_var->memory_node_list[-1].addr + size;
			p_var->memory_node_list[i].addr = p_var->memory_node_list[-1].end_addr;
		} else if (is_free_mem_size_equ == FALSE && is_free_mem_size_bigger == FALSE) {
			p_var->memory_node_list[-1].addr = p_var->last_memory_end_addr;
			p_var->memory_node_list[-1].end_addr = p_var->memory_node_list[-1].addr + size;
			p_var->last_memory_end_addr = p_var->memory_node_list[-1].end_addr;
		}
		p_var->memory_node_list[-1].is_used = TRUE;
		p_var->free_size -= (sizeof(cg_memory_node_t) + size);
		PRINT_LOG("============================memory pool============================\n");
		PRINT_LOG("memory_pool = %p;\n", p_var->memory_pool);
		PRINT_LOG("last_memory_end_addr = %p;\n", p_var->last_memory_end_addr);
		PRINT_LOG("memory_pool_size = %zu;\n", p_var->size);
		PRINT_LOG("memory_node_count = %d;\n", p_var->memory_node_count);
		PRINT_LOG("memory block size = %zu;\n", size);
		PRINT_LOG("memory_node.addr = %p;\n", p_var->memory_node_list[-1].addr);
		PRINT_LOG("memory_node.end_addr = %p;\n", p_var->memory_node_list[-1].end_addr);
		PRINT_LOG("memory_node.is_used = %d;\n", p_var->memory_node_list[-1].is_used);
		PRINT_LOG("free_size = %zu;\n", p_var->free_size);
		PRINT_LOG("===================================================================\n");
		return p_var->memory_node_list[-1].addr;
	}

	return NULL;
}

size_t cg_get_memory_size(cg_memory_pool_var_t *p_var, void *memory_addr) {
	size_t memory_size = 0;
	if (p_var->memory_pool == NULL) {
		PRINT_ERROR("must create memory pool first!\n");
		return 0;
	}
	if (memory_addr < p_var->memory_pool || memory_addr >= p_var->memory_pool + p_var->size) {
		PRINT_ERROR("memory is not in the memory pool!\n");
		return 0;
	}
	if (memory_addr == NULL) {
		PRINT_ERROR("memory address must not be NULL!\n");
		return 0;
	}
	cg_memory_node_t *memory_node_list = (cg_memory_node_t *)(p_var->memory_pool + p_var->size - p_var->memory_node_count * sizeof(cg_memory_node_t));
	for (uint32_t i = 0; i < p_var->memory_node_count; i++) {
		if (memory_node_list[i].addr == memory_addr) {
			memory_size = (size_t)(memory_node_list[i].end_addr - memory_node_list[i].addr);
			break;
		}
	}

	return memory_size;
}

uint32_t cg_get_memory_node_index(cg_memory_pool_var_t *p_var, void *memory_addr) {
	if (p_var->memory_pool == NULL) {
		PRINT_ERROR("must create memory pool first!\n");
		return 0;
	}
	if (memory_addr < p_var->memory_pool || memory_addr >= p_var->memory_pool + p_var->size) {
		PRINT_ERROR("memory is not in the memory pool!\n");
		return 0;
	}
	if (memory_addr == NULL) {
		PRINT_ERROR("memory address must not be NULL!\n");
		return 0;
	}
	cg_memory_node_t *memory_node_list = (cg_memory_node_t *)(p_var->memory_pool + p_var->size - p_var->memory_node_count * sizeof(cg_memory_node_t));
	uint32_t i = 0;
	for (i = 0; i < p_var->memory_node_count; i++) {
		if (memory_node_list[i].addr == memory_addr) {
			break;
		}
	}

	return i;
}

cg_memory_node_t *cg_get_memory_node(cg_memory_pool_var_t *p_var, void *memory_addr, void *memory_end_addr) {
	if (p_var->memory_pool == NULL) {
		PRINT_ERROR("must create memory pool first!\n");
		return 0;
	}
	if (memory_addr < p_var->memory_pool || memory_addr >= p_var->memory_pool + p_var->size) {
		PRINT_ERROR("memory is not in the memory pool!\n");
		return 0;
	}
	if ((memory_addr == NULL && memory_end_addr == NULL) || (!(memory_addr == NULL || memory_end_addr == NULL))) {
		PRINT_ERROR("one of them must be NULL and the other must not be NULL!\n");
		return 0;
	}

	cg_memory_node_t *memory_node_list = (cg_memory_node_t *)(p_var->memory_pool + p_var->size - p_var->memory_node_count * sizeof(cg_memory_node_t));
	uint32_t i = 0;
	if (memory_addr != NULL && memory_end_addr == NULL) {
		for (i = 0; i < p_var->memory_node_count; i++) {
			if (memory_node_list[i].addr == memory_addr) {
				break;
			}
		}
	} else if (memory_addr == NULL && memory_end_addr != NULL) {
		for (i = 0; i < p_var->memory_node_count; i++) {
			if (memory_node_list[i].end_addr == memory_end_addr) {
				break;
			}
		}
	}

	return &memory_node_list[i];
}

// 删除一个内存块信息节点
void cg_rm_one_memory_node(cg_memory_pool_var_t *p_var, uint32_t index) {
	cg_memory_node_t *memory_node_list = (cg_memory_node_t *)(p_var->memory_pool + p_var->size - p_var->memory_node_count * sizeof(cg_memory_node_t));
	if (p_var->memory_node_count == 1) {
		memset(memory_node_list, 0, sizeof(cg_memory_node_t));
		p_var->memory_node_count = 0;
		return;
	}
	if (index == 0) {
		p_var->memory_node_count--;
		return;
	}
	cg_memory_node_t *new_memory_node_list = (cg_memory_node_t *)(memory_node_list + sizeof(cg_memory_node_t));
	if (index == p_var->memory_node_count) {
		p_var->memory_node_count--;
		memmove(new_memory_node_list, memory_node_list, p_var->memory_node_count * sizeof(cg_memory_node_t));
		return;
	}
	if (index > 0 && index < p_var->memory_node_count) {
		memmove(new_memory_node_list, memory_node_list, index * sizeof(cg_memory_node_t));
		p_var->memory_node_count--;
		return;
	}
}

void cg_free_memory(cg_memory_pool_var_t *p_var, void *memory_addr) {
	if (p_var->memory_pool == NULL) {
		PRINT_ERROR("must create memory pool first!\n");
		return;
	}
	if (memory_addr < p_var->memory_pool || memory_addr >= p_var->memory_pool + p_var->size) {
		PRINT_ERROR("memory is not in the memory pool!\n");
		return;
	}
	if (memory_addr == NULL) {
		PRINT_ERROR("memory address must not be NULL!\n");
		return;
	}

	cg_memory_node_t *p_memory_node = cg_get_memory_node(p_var, memory_addr, NULL);
	uint32_t memory_index = cg_get_memory_node_index(p_var, memory_addr);

	// 如果该内存块排在最后尾
	p_memory_node->is_used = FALSE;
	if (p_memory_node->end_addr == p_var->last_memory_end_addr) {
		p_var->last_memory_end_addr = p_memory_node->addr;
		cg_rm_one_memory_node(p_var, memory_index);
		return;
	}

	// 如果该内存块的前一个内存块已被释放
	cg_memory_node_t *p_previous_mem_node = cg_get_memory_node(p_var, NULL, memory_addr);
	if (p_previous_mem_node->is_used == FALSE) {
		p_previous_mem_node->end_addr = p_memory_node->end_addr;
		cg_rm_one_memory_node(p_var, memory_index);
		return;
	}

	// 如果该内存块的后一个内存块已被释放
	cg_memory_node_t *p_next_mem_node = cg_get_memory_node(p_var, NULL, p_memory_node->end_addr);
	if (p_next_mem_node->is_used == FALSE) {
		p_memory_node->end_addr = p_next_mem_node->end_addr;
		uint32_t next_memory_index = cg_get_memory_node_index(p_var, p_next_mem_node->addr);
		cg_rm_one_memory_node(p_var, next_memory_index);
		return;
	}
	return;
}

#ifdef DEBUG
void *cg_realloc_memory(cg_memory_pool_var_t *p_var, void *memory_addr, size_t size) {
	if (p_var->memory_pool == NULL) {
		PRINT_ERROR("must create memory pool first!\n");
		return memory_addr;
	} else if ((size + sizeof(cg_memory_node_t)) > p_var->free_size) {
		PRINT_ERROR("allocate memory fail! the size is too big!\n");
		return NULL;
	}
	if (memory_addr < p_var->memory_pool || memory_addr >= p_var->memory_pool + p_var->size) {
		PRINT_ERROR("memory is not in the memory pool!\n");
		return memory_addr;
	}
	if (memory_addr == NULL) {
		PRINT_ERROR("memory address must not be NULL!\n");
		return memory_addr;
	}
	if (size == 0) {
		cg_free_memory(p_var, memory_addr);
		return NULL;
	}
	size_t old_size = cg_get_memory_size(p_var, memory_addr);
	if (size > old_size) {
		void *new_memory_addr = cg_alloc_memory(p_var, size);
		if (new_memory_addr != NULL) {
			return new_memory_addr;
		}
		return memory_addr;
	}
	if (size < old_size) {
		return memory_addr;
	}
	if (size == old_size) {
		return memory_addr;
	}

	return memory_addr;
}
#endif
