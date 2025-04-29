#include "cg_memory_pool.h"

bool cg_create_memory_pool(cg_memory_pool_var_t *p_var) {
	if (p_var->size < sizeof(cg_memory_node_t)) {
		PRINT_ERROR("memory pool size should more bigger!\n");
		return false;
	}
	p_var->memory_pool = calloc(1, p_var->size);
	p_var->free_memory_node_addr_arry = calloc(MAX_FREE_MEM_NODE_COUNT, sizeof(cg_memory_node_t *));
	if (p_var->memory_pool == nullptr || p_var->free_memory_node_addr_arry == nullptr) {
		PRINT_ERROR("create memory pool fail!\n");
		return false;
	}
	p_var->free_size = p_var->size;
	p_var->memory_count = 0;
	PRINT_LOG("============================memory pool============================\n");
	PRINT_LOG("create memory_pool success!\n");
	PRINT_LOG("memory_pool = %p;\n", p_var->memory_pool);
	PRINT_LOG("memory_pool_size = %zu;\n", p_var->size);
	PRINT_LOG("free_size = %zu;\n", p_var->free_size);
	PRINT_LOG("===================================================================\n");
	return true;
}

void *cg_alloc_memory(cg_memory_pool_var_t *p_var, size_t size) {
	size_t node_and_mem_size = sizeof(cg_memory_node_t) + size;
	cg_memory_node_t *p_memory_node = nullptr;
	if (node_and_mem_size > p_var->free_size) {
		PRINT_ERROR("fail! the size is too large!\n");
		return nullptr;
	}
	if (size == 0) {
		PRINT_ERROR("size must not be 0!\n");
		return nullptr;
	}
	if (p_var->memory_count == 0) {
		p_memory_node = (cg_memory_node_t *)p_var->memory_pool;
		p_memory_node->memory_addr = p_var->memory_pool + sizeof(cg_memory_node_t);
		p_memory_node->size = size;
		p_memory_node->is_used = true;
		p_memory_node->prev_memory_node_addr = nullptr;
		p_var->free_size -= node_and_mem_size;
		p_var->memory_count = 1;
		p_var->last_memory_size = size;
		p_var->last_memory_end_addr = p_memory_node->memory_addr + size;
		PRINT_LOG("============================memory pool============================\n");
		PRINT_LOG("memory_pool = %p;\n", p_var->memory_pool);
		PRINT_LOG("memory_pool_size = %zu;\n", p_var->size);
		PRINT_LOG("memory block addr = %p;\n", p_memory_node->memory_addr);
		PRINT_LOG("memory block size = %zu;\n", size);
		PRINT_LOG("free_size = %zu;\n", p_var->free_size);
		PRINT_LOG("===================================================================\n");
		return p_memory_node->memory_addr;
	} else if (p_var->memory_count >= 1) {
		// 优先把last_memory_end_addr后面的内存空间分配给新内存块
		if ((p_var->memory_pool + p_var->size - p_var->last_memory_end_addr) >= node_and_mem_size) {
			p_memory_node = (cg_memory_node_t *)p_var->last_memory_end_addr;
			p_memory_node->memory_addr = p_var->last_memory_end_addr + sizeof(cg_memory_node_t);
			p_memory_node->size = size;
			p_memory_node->is_used = true;
			p_memory_node->prev_memory_node_addr = (cg_memory_node_t *)(p_var->last_memory_end_addr - p_var->last_memory_size);
			p_var->free_size -= node_and_mem_size;
			p_var->memory_count++;
			p_var->last_memory_size = size;
			p_var->last_memory_end_addr = p_memory_node->memory_addr + size;
			PRINT_LOG("============================memory pool============================\n");
			PRINT_LOG("memory_pool = %p;\n", p_var->memory_pool);
			PRINT_LOG("memory_pool_size = %zu;\n", p_var->size);
			PRINT_LOG("memory block size = %zu;\n", size);
			PRINT_LOG("memory block addr = %p;\n", p_memory_node->memory_addr);
			PRINT_LOG("free_size = %zu;\n", p_var->free_size);
			PRINT_LOG("===================================================================\n");
			return p_memory_node->memory_addr;
		}

		/*如果last_memory_end_addr后面的内存空间不够,而且之前已经释放的内存块的大小足够容纳新内存块的大小,就优先利用这块之前已经被释放的内存块*/
		bool is_free_mem_size_equ = false;
		bool is_free_mem_size_bigger = false;
		uint32_t i = 0;
		for (i = 0; i < p_var->free_memory_node_count; i++) {
			if (p_var->free_memory_node_addr_arry[i]->size == size) {
				is_free_mem_size_equ = true;
				break;
			} else if (p_var->free_memory_node_addr_arry[i]->size > size) {
				is_free_mem_size_bigger = true;
				break;
			}
		}
		p_memory_node = p_var->free_memory_node_addr_arry[i];
		if (is_free_mem_size_equ == true) {
			p_var->free_memory_node_addr_arry[i]->is_used = true;
			p_var->free_size -= p_memory_node->size;
			cg_rm_one_p_memory_node(p_var, i);
			PRINT_LOG("============================memory pool============================\n");
			PRINT_LOG("memory_pool = %p;\n", p_var->memory_pool);
			PRINT_LOG("memory_pool_size = %zu;\n", p_var->size);
			PRINT_LOG("memory block size = %zu;\n", size);
			PRINT_LOG("memory block addr = %p;\n", p_memory_node->memory_addr);
			PRINT_LOG("free_size = %zu;\n", p_var->free_size);
			PRINT_LOG("===================================================================\n");
			return p_memory_node->memory_addr;
		} else if (is_free_mem_size_bigger == true) {
			cg_memory_node_t *p_new_free_mem_node = (cg_memory_node_t *)(p_memory_node->memory_addr + size);
			p_new_free_mem_node->memory_addr = p_memory_node->memory_addr + size + sizeof(cg_memory_node_t);
			p_new_free_mem_node->size = p_memory_node->size - size;
			p_new_free_mem_node->is_used = false;
			p_new_free_mem_node->prev_memory_node_addr = p_memory_node;
			cg_add_one_p_memory_node(p_var, p_new_free_mem_node);
			p_memory_node->size = size;
			p_memory_node->is_used = true;
			p_var->free_size -= node_and_mem_size;
			PRINT_LOG("============================memory pool============================\n");
			PRINT_LOG("memory_pool = %p;\n", p_var->memory_pool);
			PRINT_LOG("memory_pool_size = %zu;\n", p_var->size);
			PRINT_LOG("memory block size = %zu;\n", size);
			PRINT_LOG("memory block addr = %p;\n", p_memory_node->memory_addr);
			PRINT_LOG("free_size = %zu;\n", p_var->free_size);
			PRINT_LOG("===================================================================\n");
			return p_memory_node->memory_addr;
		}
	}

	return nullptr;
}

void cg_free_memory(cg_memory_pool_var_t *p_var, void *memory_addr) {
	if (memory_addr < p_var->memory_pool || memory_addr >= p_var->memory_pool + p_var->size || memory_addr == nullptr) {
		PRINT_ERROR("this memory is not in the memory pool!\n");
		return;
	}
	cg_memory_node_t *p_memory_node = (cg_memory_node_t *)(memory_addr - sizeof(cg_memory_node_t));
	cg_memory_node_t *p_prev_memory_node = p_memory_node->prev_memory_node_addr;
	cg_memory_node_t *p_next_memory_node = (cg_memory_node_t *)(memory_addr + p_memory_node->size);
	size_t free_size = p_memory_node->size;
	int32_t memory_node_index = cg_get_memory_node_index(p_var, memory_addr);

	// 如果该内存块排在最后尾
	if (p_memory_node->end_addr == p_var->last_memory_end_addr) {
		// 而且该内存块的前一个内存块已被释放
		if (p_previous_mem_node->is_used == false) {
			int32_t previous_mem_node_index = -1;
			cg_get_memory_node_index(p_var, p_previous_mem_node->addr, &previous_mem_node_index);
			p_previous_mem_node->end_addr = p_memory_node->end_addr;
			cg_rm_one_memory_node(p_var, memory_node_index);
			cg_rm_one_memory_node(p_var, previous_mem_node_index);
			p_var->free_size += free_size;
			return;
		} else if (p_previous_mem_node->is_used == true) {
			p_var->last_memory_end_addr = p_memory_node->addr;
			cg_rm_one_memory_node(p_var, memory_node_index);
			p_var->free_size += free_size;
			return;
		}
	}

	// 如果该内存块的前一个内存块已被释放
	if (p_previous_mem_node->is_used == false) {
		p_previous_mem_node->end_addr = p_memory_node->end_addr;
		cg_rm_one_memory_node(p_var, memory_node_index);
		p_var->free_size += free_size;
		return;
	}

	// 如果该内存块的后一个内存块已被释放
	if (p_next_mem_node->is_used == false) {
		p_memory_node->end_addr = p_next_mem_node->end_addr;
		int32_t next_memory_node_index = -1;
		cg_get_memory_node_index(p_var, p_next_mem_node->addr, &next_memory_node_index);
		cg_rm_one_memory_node(p_var, next_memory_node_index);
		p_memory_node->is_used = false;
		p_var->free_size += free_size;
		return;
	}

	// 如果该内存块的前后内存块都没被释放
	p_memory_node->is_used = false;
	p_var->free_size += free_size;
	return;
}

void *cg_realloc_memory(cg_memory_pool_var_t *p_var, void *memory_addr, size_t size) {
	if (p_var->memory_pool == nullptr) {
		PRINT_ERROR("memory pool address must not be nullptr!\n");
		return memory_addr;
	} else if ((size + sizeof(cg_memory_node_t)) > p_var->free_size) {
		PRINT_ERROR("fail! the size is too large!\n");
		return nullptr;
	}
	if (memory_addr < p_var->memory_pool || memory_addr >= p_var->memory_pool + p_var->size) {
		PRINT_ERROR("this memory is not in the memory pool!\n");
		return memory_addr;
	}
	if (memory_addr == nullptr) {
		PRINT_ERROR("this memory address must not be nullptr!\n");
		return memory_addr;
	}
	if (size == 0) {
		cg_free_memory(p_var, memory_addr);
		return nullptr;
	}
	cg_memory_node_t *p_memory_node = cg_get_memory_node_addr(p_var, memory_addr, nullptr);
	size_t old_size = (size_t)(p_memory_node->end_addr - p_memory_node->addr);
	if (size < old_size) {
		// 如果该内存块排在最后尾
		if (p_memory_node->end_addr == p_var->last_memory_end_addr) {
			p_memory_node->end_addr = p_var->last_memory_end_addr - (old_size - size);
			p_var->last_memory_end_addr = p_memory_node->end_addr;
			p_var->free_size += (old_size - size);
			return memory_addr;
		}
		cg_add_one_memory_node(
			p_var,
			(cg_memory_node_t){
				.addr = p_memory_node->end_addr - (old_size - size),
				.end_addr = p_memory_node->end_addr,
				.is_used = false});
		p_var->free_size += (old_size - size);
		return memory_addr;
	}
	if (size > old_size) {
		// 如果该内存块排在最后尾
		if (p_memory_node->end_addr == p_var->last_memory_end_addr) {
			p_memory_node->end_addr = p_var->last_memory_end_addr + (size - old_size);
			p_var->last_memory_end_addr = p_memory_node->end_addr;
			p_var->free_size -= (size - old_size);
			return memory_addr;
		}
		void *new_memory_addr = cg_alloc_memory(p_var, size);
		if (new_memory_addr != nullptr) {
			memmove(new_memory_addr, memory_addr, old_size);
			cg_free_memory(p_var, memory_addr);
			return new_memory_addr;
		}
	}

	return memory_addr;
}

size_t cg_get_memory_size(cg_memory_pool_var_t *p_var, void *memory_addr) {
	if (memory_addr < p_var->memory_pool || memory_addr >= p_var->memory_pool + p_var->size) {
		PRINT_ERROR("this memory is not in the memory pool!\n");
		return 0;
	}
	cg_memory_node_t *p_memory_node = (cg_memory_node_t *)(memory_addr - sizeof(cg_memory_node_t));
	return p_memory_node->size;
}

int32_t cg_get_memory_node_index(cg_memory_pool_var_t *p_var, void *memory_addr) {
	if (memory_addr < p_var->memory_pool || memory_addr >= p_var->memory_pool + p_var->size) {
		PRINT_ERROR("this memory is not in the memory pool!\n");
		return -1;
	}
	int32_t index = 0;
	for (index = 0; index < p_var->free_memory_node_count; index++) {
		if (p_var->free_memory_node_addr_arry[index]->memory_addr == memory_addr) {
			break;
		}
	}

	return index;
}

bool cg_add_one_p_memory_node(cg_memory_pool_var_t *p_var, cg_memory_node_t *p_memory_node) {
	if (p_var->free_memory_node_count == MAX_FREE_MEM_NODE_COUNT) {
		PRINT_ERROR("p_var->free_memory_node_count == MAX_FREE_MEM_NODE_COUNT!\n");
		return false;
	}
	p_var->free_memory_node_addr_arry[p_var->free_memory_node_count - 1] = p_memory_node;
	p_var->free_memory_node_count++;
	return true;
}

bool cg_rm_one_p_memory_node(cg_memory_pool_var_t *p_var, uint32_t index) {
	if (p_var->free_memory_node_count == 1) {
		p_var->free_memory_node_addr_arry[0] = nullptr;
		p_var->free_memory_node_count = 0;
		return true;
	}
	if (p_var->free_memory_node_count - 1 > index) {
		p_var->free_memory_node_addr_arry[index] = p_var->free_memory_node_addr_arry[p_var->free_memory_node_count - 1];
		p_var->free_memory_node_addr_arry[p_var->free_memory_node_count - 1] = nullptr;
	}
	if (p_var->free_memory_node_count - 1 == index) {
		p_var->free_memory_node_addr_arry[index] = nullptr;
	}

	return false;
}
