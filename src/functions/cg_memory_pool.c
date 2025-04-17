#include "cg_memory_pool.h"
#include <src/functions/cg_log.h>
#include <stdint.h>
#include <string.h>

bool cg_create_memory_pool(cg_memory_pool_var_t *p_var) {
	if (p_var->size <= 0) {
		PRINT_ERROR("memory pool size should more bigger than 0!\n");
		return false;
	}
	p_var->last_memory_end_addr = nullptr;
	p_var->memory_node_count = 0;
	p_var->free_size = 0;
	if (p_var->memory_pool == nullptr || p_var->memory_node_list == nullptr) {
		PRINT_ERROR("create memory pool fail!\n");
		return false;
	}
	p_var->last_memory_end_addr = p_var->memory_pool;
	p_var->free_size = p_var->size;
	PRINT_LOG("============================memory pool============================\n");
	PRINT_LOG("create memory_pool success!\n");
	PRINT_LOG("memory_pool = %p;\n", p_var->memory_pool);
	PRINT_LOG("memory_pool_size = %zu;\n", p_var->size);
	PRINT_LOG("free_size = %zu;\n", p_var->free_size);
	PRINT_LOG("===================================================================\n");
	return true;
}

void *cg_alloc_memory(cg_memory_pool_var_t *p_var, size_t size) {

	if (p_var->memory_pool == nullptr) {
		PRINT_ERROR("memory pool address must not be nullptr!\n");
		return nullptr;
	} else if (size > p_var->free_size) {
		PRINT_ERROR("the size is too big!\n");
		return nullptr;
	}
	if (size == 0) {
		PRINT_ERROR("size can must not be 0!\n");
		return nullptr;
	}

	if (p_var->memory_node_count == 0) {
		if (cg_add_one_memory_node(
			    p_var,
			    ((cg_memory_node_t){
				    .addr = p_var->memory_pool,
				    .end_addr = p_var->memory_pool + size,
				    .is_used = true})) == false) {
			PRINT_ERROR("add_one_memory_node fail!\n");
			return nullptr;
		}
		p_var->last_memory_end_addr = p_var->memory_node_list[0].end_addr;
		p_var->free_size -= size;
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
		bool is_free_mem_size_equ = false;
		bool is_free_mem_size_bigger = false;
		uint32_t i = 0;
		for (i = 0; i < p_var->memory_node_count; i++) {
			size_t memory_size = (size_t)(p_var->memory_node_list[i].end_addr - p_var->memory_node_list[i].addr);
			if (p_var->memory_node_list[i].is_used == false && memory_size == size) {
				is_free_mem_size_equ = true;
				break;
			} else if (p_var->memory_node_list[i].is_used == false && memory_size > size) {
				is_free_mem_size_bigger = true;
				break;
			}
		}

		/*如果已经释放的内存块的大小足够容纳即将被申请的内存块的大小,就优先利用这块之前已经被释放的内存块。如果不够就把新的内存块放在最后尾部*/
		if (is_free_mem_size_equ == true) {
			p_var->memory_node_list[i].is_used = true;
			p_var->free_size -= size;
			PRINT_LOG("============================memory pool============================\n");
			PRINT_LOG("memory_pool = %p;\n", p_var->memory_pool);
			PRINT_LOG("last_memory_end_addr = %p;\n", p_var->last_memory_end_addr);
			PRINT_LOG("memory_pool_size = %zu;\n", p_var->size);
			PRINT_LOG("memory_node_count = %d;\n", p_var->memory_node_count);
			PRINT_LOG("memory block size = %zu;\n", size);
			PRINT_LOG("memory_node.addr = %p;\n", p_var->memory_node_list[i].addr);
			PRINT_LOG("memory_node.end_addr = %p;\n", p_var->memory_node_list[i].end_addr);
			PRINT_LOG("memory_node.is_used = %d;\n", p_var->memory_node_list[i].is_used);
			PRINT_LOG("free_size = %zu;\n", p_var->free_size);
			PRINT_LOG("===================================================================\n");
			return p_var->memory_node_list[i].addr;
		} else if (is_free_mem_size_bigger == true) {
			if (cg_add_one_memory_node(
				    p_var,
				    ((cg_memory_node_t){
					    .addr = p_var->memory_node_list[i].addr,
					    .end_addr = p_var->memory_node_list[i].addr + size,
					    .is_used = true})) == false) {
				PRINT_ERROR("add_one_memory_node fail!\n");
				return nullptr;
			}
			p_var->memory_node_list[i].addr = p_var->memory_node_list[p_var->memory_node_count - 1].end_addr;
		} else if (is_free_mem_size_equ == false && is_free_mem_size_bigger == false) {
			if (cg_add_one_memory_node(
				    p_var,
				    ((cg_memory_node_t){
					    .addr = p_var->last_memory_end_addr,
					    .end_addr = p_var->last_memory_end_addr + size,
					    .is_used = true})) == false) {
				PRINT_ERROR("add_one_memory_node fail!\n");
				return nullptr;
			}
			p_var->last_memory_end_addr = p_var->memory_node_list[p_var->memory_node_count - 1].end_addr;
		}
		p_var->free_size -= size;
		PRINT_LOG("============================memory pool============================\n");
		PRINT_LOG("memory_pool = %p;\n", p_var->memory_pool);
		PRINT_LOG("last_memory_end_addr = %p;\n", p_var->last_memory_end_addr);
		PRINT_LOG("memory_pool_size = %zu;\n", p_var->size);
		PRINT_LOG("memory_node_count = %d;\n", p_var->memory_node_count);
		PRINT_LOG("memory block size = %zu;\n", size);
		PRINT_LOG("memory_node.addr = %p;\n", p_var->memory_node_list[p_var->memory_node_count - 1].addr);
		PRINT_LOG("memory_node.end_addr = %p;\n", p_var->memory_node_list[p_var->memory_node_count - 1].end_addr);
		PRINT_LOG("memory_node.is_used = %d;\n", p_var->memory_node_list[p_var->memory_node_count - 1].is_used);
		PRINT_LOG("free_size = %zu;\n", p_var->free_size);
		PRINT_LOG("===================================================================\n");
		return p_var->memory_node_list[p_var->memory_node_count - 1].addr;
	}

	return nullptr;
}

size_t cg_get_memory_size(cg_memory_pool_var_t *p_var, void *memory_addr) {
	size_t memory_size = 0;
	if (p_var->memory_pool == nullptr) {
		PRINT_ERROR("memory pool address must not be nullptr!\n");
		return 0;
	}
	if (memory_addr < p_var->memory_pool || memory_addr >= p_var->memory_pool + p_var->size) {
		PRINT_ERROR("this memory is not in the memory pool!\n");
		return 0;
	}
	if (memory_addr == nullptr) {
		PRINT_ERROR("this memory address must not be nullptr!\n");
		return 0;
	}
	for (uint32_t i = 0; i < p_var->memory_node_count; i++) {
		if (p_var->memory_node_list[i].addr == memory_addr) {
			memory_size = (size_t)(p_var->memory_node_list[i].end_addr - p_var->memory_node_list[i].addr);
			break;
		}
	}

	return memory_size;
}

int32_t cg_get_memory_node_index(cg_memory_pool_var_t *p_var, void *memory_addr) {
	if (p_var->memory_pool == nullptr) {
		PRINT_ERROR("memory pool address must not be nullptr!\n");
		return 0;
	}
	if (memory_addr < p_var->memory_pool || memory_addr >= p_var->memory_pool + p_var->size) {
		PRINT_ERROR("this memory is not in the memory pool!\n");
		return 0;
	}
	if (memory_addr == nullptr) {
		PRINT_ERROR("this memory address must not be nullptr!\n");
		return 0;
	}
	uint32_t i = 0;
	for (i = 0; i < p_var->memory_node_count; i++) {
		if (p_var->memory_node_list[i].addr == memory_addr) {
			break;
		}
	}

	return i;
}

cg_memory_node_t *cg_get_memory_node_addr(cg_memory_pool_var_t *p_var, void *memory_addr, void *memory_end_addr) {
	if (p_var->memory_pool == nullptr) {
		PRINT_ERROR("memory pool address must not be nullptr!\n");
		return nullptr;
	}
	if (memory_addr < p_var->memory_pool || memory_addr >= p_var->memory_pool + p_var->size) {
		PRINT_ERROR("this memory is not in the memory pool!\n");
		return nullptr;
	}
	if ((memory_addr == nullptr && memory_end_addr == nullptr) || (!(memory_addr == nullptr || memory_end_addr == nullptr))) {
		PRINT_ERROR("one of them must be nullptr and the other must not be nullptr!\n");
		return nullptr;
	}

	int32_t i = 0;
	if (memory_addr != nullptr && memory_end_addr == nullptr) {
		for (i = 0; i < p_var->memory_node_count; i++) {
			if (p_var->memory_node_list[i].addr == memory_addr) {
				break;
			}
		}
	} else if (memory_addr == nullptr && memory_end_addr != nullptr) {
		for (i = 0; i < p_var->memory_node_count; i++) {
			if (p_var->memory_node_list[i].end_addr == memory_end_addr) {
				break;
			}
		}
	}

	return &p_var->memory_node_list[i];
}

bool cg_add_one_memory_node(cg_memory_pool_var_t *p_var, cg_memory_node_t memory_node_info) {
#define EXT_NODE_COUNT 8
	if (p_var->memory_node_count == p_var->memory_node_max_count) {
		p_var->memory_node_list = realloc(p_var->memory_node_list, sizeof(cg_memory_node_t) * (p_var->memory_node_max_count + EXT_NODE_COUNT));
		if (p_var->memory_node_list == nullptr) {
			return false;
		}
		p_var->memory_node_max_count = p_var->memory_node_max_count + EXT_NODE_COUNT;
	};
	p_var->memory_node_list[p_var->memory_node_count] = memory_node_info;
	p_var->memory_node_count++;
	return true;
}

void cg_rm_one_memory_node(cg_memory_pool_var_t *p_var, int32_t index) {
	if (p_var->memory_node_count == 1) {
		memset(p_var->memory_node_list, 0, sizeof(cg_memory_node_t));
		p_var->memory_node_count = 0;
		return;
	}
	if (index == 0) {
		memmove(&p_var->memory_node_list[0], &p_var->memory_node_list[1], (p_var->memory_node_count - 1) * sizeof(cg_memory_node_t));
		memset(&p_var->memory_node_list[p_var->memory_node_count - 1], 0, sizeof(cg_memory_node_t));
		p_var->memory_node_count--;
		return;
	}
	cg_memory_node_t *new_memory_node_list = (cg_memory_node_t *)(p_var->memory_node_list + sizeof(cg_memory_node_t));
	if (index == p_var->memory_node_count) {
		p_var->memory_node_count--;
		memmove(new_memory_node_list, p_var->memory_node_list, p_var->memory_node_count * sizeof(cg_memory_node_t));
		return;
	}
	if (index > 0 && index < p_var->memory_node_count) {
		memmove(new_memory_node_list, p_var->memory_node_list, index * sizeof(cg_memory_node_t));
		p_var->memory_node_count--;
		return;
	}
}

void cg_free_memory(cg_memory_pool_var_t *p_var, void *memory_addr) {
	if (p_var->memory_pool == nullptr) {
		PRINT_ERROR("memory pool address must not be nullptr!\n");
		return;
	}
	if (memory_addr < p_var->memory_pool || memory_addr >= p_var->memory_pool + p_var->size) {
		PRINT_ERROR("this memory is not in the memory pool!\n");
		return;
	}
	if (memory_addr == nullptr) {
		PRINT_ERROR("this memory address must not be nullptr!\n");
		return;
	}

	cg_memory_node_t *p_memory_node = cg_get_memory_node_addr(p_var, memory_addr, nullptr);
	size_t free_size = (size_t)(p_memory_node->end_addr - p_memory_node->addr);
	uint32_t memory_index = cg_get_memory_node_index(p_var, memory_addr);

	// 如果该内存块排在最后尾
	if (p_memory_node->end_addr == p_var->last_memory_end_addr) {
		p_var->last_memory_end_addr = p_memory_node->addr;
		cg_rm_one_memory_node(p_var, memory_index);
		p_var->free_size += free_size;
		p_memory_node->is_used = false;
		return;
	}

	// 如果该内存块的前一个内存块已被释放
	cg_memory_node_t *p_previous_mem_node = cg_get_memory_node_addr(p_var, nullptr, memory_addr);
	if (p_previous_mem_node->is_used == false) {
		p_previous_mem_node->end_addr = p_memory_node->end_addr;
		cg_rm_one_memory_node(p_var, memory_index);
		p_var->free_size += free_size;
		p_memory_node->is_used = false;
		return;
	}

	// 如果该内存块的后一个内存块已被释放
	cg_memory_node_t *p_next_mem_node = cg_get_memory_node_addr(p_var, p_memory_node->end_addr, nullptr);
	if (p_next_mem_node->is_used == false) {
		p_memory_node->end_addr = p_next_mem_node->end_addr;
		uint32_t next_memory_index = cg_get_memory_node_index(p_var, p_next_mem_node->addr);
		cg_rm_one_memory_node(p_var, next_memory_index);
		p_var->free_size += free_size;
		p_memory_node->is_used = false;
		return;
	}

	// 如果该内存块的前后内存块都没被释放
	p_var->free_size += free_size;
	p_memory_node->is_used = false;
	return;
}

#ifdef DEBUG
void *cg_realloc_memory(cg_memory_pool_var_t *p_var, void *memory_addr, size_t size) {
	if (p_var->memory_pool == nullptr) {
		PRINT_ERROR("memory pool address must not be nullptr!\n");
		return memory_addr;
	} else if ((size + sizeof(cg_memory_node_t)) > p_var->free_size) {
		PRINT_ERROR("fail! the size is too big!\n");
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
		return memory_addr;
	}
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
	if (size == old_size) {
		return memory_addr;
	}

	return memory_addr;
}
#endif
