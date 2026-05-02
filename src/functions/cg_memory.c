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

#include "cg_memory.h"

bool cg_create_memory_pool(cg_memory_pool_info_t *p_mp) {
	if (p_mp->size < sizeof(cg_memory_node_t)) {
		PRINT_ERROR("memory pool size should be larger!\n");
		return false;
	}
	if (p_mp->memory_pool == nullptr || p_mp->free_memory_node_addr_array == nullptr) {
		PRINT_ERROR("create memory pool fail!\n");
		return false;
	}
	p_mp->free_size = p_mp->size;
	p_mp->memory_count = 0;
	PRINT_LOG("============================memory pool============================\n");
	PRINT_LOG("create memory_pool success!\n");
	PRINT_LOG("memory_pool = %p;\n", p_mp->memory_pool);
	PRINT_LOG("memory_pool_size = %zu;\n", p_mp->size);
	PRINT_LOG("free_size = %zu;\n", p_mp->free_size);
	PRINT_LOG("===================================================================\n");
	return true;
}

void *cg_alloc_memory(cg_memory_pool_info_t *p_mp, size_t size) {
	// 保存排在最后的内存块的尾地址
	void *last_memory_end_addr = nullptr;
	size_t node_and_mem_size = sizeof(cg_memory_node_t) + size;
	cg_memory_node_t *p_memory_node = nullptr;
	if (node_and_mem_size > p_mp->free_size) {
		PRINT_ERROR("fail! the size is too large!\n");
		return nullptr;
	}
	if (size == 0) {
		PRINT_ERROR("size must not be 0!\n");
		return nullptr;
	}
	if (p_mp->memory_count == 0) {
		p_memory_node = (cg_memory_node_t *)p_mp->memory_pool;
		p_memory_node->is_used = true;
		p_memory_node->memory_addr = p_mp->memory_pool + sizeof(cg_memory_node_t);
		p_memory_node->size = size;
		p_memory_node->prev_memory_node_addr = nullptr;
		p_mp->free_size -= node_and_mem_size;
		p_mp->memory_count = 1;
		p_mp->p_last_memory_node = p_memory_node;
		PRINT_LOG("============================memory pool============================\n");
		PRINT_LOG("memory_pool = %p;\n", p_mp->memory_pool);
		PRINT_LOG("memory_pool_size = %zu;\n", p_mp->size);
		PRINT_LOG("memory block addr = %p;\n", p_memory_node->memory_addr);
		PRINT_LOG("memory block size = %zu;\n", size);
		PRINT_LOG("free_size = %zu;\n", p_mp->free_size);
		PRINT_LOG("===================================================================\n");
		return p_memory_node->memory_addr;
	}
	if (p_mp->memory_count >= 1) {
		// 优先把last_memory_end_addr后面的内存空间分配给新内存块
		last_memory_end_addr = p_mp->p_last_memory_node->memory_addr + p_mp->p_last_memory_node->size;
		if ((p_mp->memory_pool + p_mp->size - last_memory_end_addr) >= node_and_mem_size) {
			p_memory_node = (cg_memory_node_t *)last_memory_end_addr;
			p_memory_node->is_used = true;
			p_memory_node->memory_addr = last_memory_end_addr + sizeof(cg_memory_node_t);
			p_memory_node->size = size;
			p_memory_node->prev_memory_node_addr = p_mp->p_last_memory_node;
			p_mp->free_size -= node_and_mem_size;
			p_mp->memory_count++;
			p_mp->p_last_memory_node = p_memory_node;
			PRINT_LOG("============================memory pool============================\n");
			PRINT_LOG("memory_pool = %p;\n", p_mp->memory_pool);
			PRINT_LOG("memory_pool_size = %zu;\n", p_mp->size);
			PRINT_LOG("memory block size = %zu;\n", size);
			PRINT_LOG("memory block addr = %p;\n", p_memory_node->memory_addr);
			PRINT_LOG("free_size = %zu;\n", p_mp->free_size);
			PRINT_LOG("===================================================================\n");
			return p_memory_node->memory_addr;
		} else {
			/*如果last_memory_end_addr后面的内存空间不够,而且之前已经释放的内存块的大小足够容纳新内存块的大小,就优先利用之前已经被释放的内存块*/
			bool is_free_mem_size_equ = false;
			bool is_free_mem_size_bigger = false;
			uint32_t i = 0;
			for (i = 0; i < p_mp->free_memory_node_count; i++) {
				if (p_mp->free_memory_node_addr_array[i] != nullptr && p_mp->free_memory_node_addr_array[i]->size == size) {
					is_free_mem_size_equ = true;
					break;
				} else if (p_mp->free_memory_node_addr_array[i] != nullptr && p_mp->free_memory_node_addr_array[i]->size > size) {
					is_free_mem_size_bigger = true;
					break;
				}
			}
			p_memory_node = p_mp->free_memory_node_addr_array[i];
			// 如果空闲内存块大小和即将申请的内存块大小一样
			if (is_free_mem_size_equ == true) {
				p_mp->free_memory_node_addr_array[i]->is_used = true;
				p_mp->free_size -= p_memory_node->size;
				p_mp->memory_count++;
				cg_rm_one_p_memory_node(p_mp, i);
				PRINT_LOG("============================memory pool============================\n");
				PRINT_LOG("memory_pool = %p;\n", p_mp->memory_pool);
				PRINT_LOG("memory_pool_size = %zu;\n", p_mp->size);
				PRINT_LOG("memory block size = %zu;\n", size);
				PRINT_LOG("memory block addr = %p;\n", p_memory_node->memory_addr);
				PRINT_LOG("free_size = %zu;\n", p_mp->free_size);
				PRINT_LOG("===================================================================\n");
				return p_memory_node->memory_addr;
			} else if (is_free_mem_size_bigger == true) {
				// 如果空闲内存块大小比即将申请的内存块只大一点点,且内存块被分割后剩余容量不够放内存信息节点和空闲内存块
				if (p_memory_node->size - size <= sizeof(cg_memory_node_t)) {
					p_memory_node->is_used = true;
					p_mp->free_size -= p_memory_node->size;
					p_mp->memory_count++;
					cg_rm_one_p_memory_node(p_mp, i);
					PRINT_LOG("============================memory pool============================\n");
					PRINT_LOG("memory_pool = %p;\n", p_mp->memory_pool);
					PRINT_LOG("memory_pool_size = %zu;\n", p_mp->size);
					PRINT_LOG("memory block size = %zu;\n", size);
					PRINT_LOG("memory block addr = %p;\n", p_memory_node->memory_addr);
					PRINT_LOG("free_size = %zu;\n", p_mp->free_size);
					PRINT_LOG("===================================================================\n");
					return p_memory_node->memory_addr;
				}
				// 如果空闲内存块大小比即将申请的内存块大很多
				cg_memory_node_t *p_new_free_mem_node = (cg_memory_node_t *)(p_memory_node->memory_addr + size);
				p_new_free_mem_node->memory_addr = p_memory_node->memory_addr + size + sizeof(cg_memory_node_t);
				p_new_free_mem_node->size = p_memory_node->size - sizeof(cg_memory_node_t) - size;
				p_new_free_mem_node->is_used = false;
				p_new_free_mem_node->prev_memory_node_addr = p_memory_node;
				cg_add_one_p_memory_node(p_mp, p_new_free_mem_node);
				// 更新这个新空闲块的节点信息以及后一个内存块的节点信息
				cg_memory_node_t *p_new_free_mem_node_next_node = (cg_memory_node_t *)(p_new_free_mem_node->memory_addr + p_new_free_mem_node->size);
				p_new_free_mem_node_next_node->prev_memory_node_addr = p_new_free_mem_node;
				cg_rm_one_p_memory_node(p_mp, i);
				p_memory_node->size = size;
				p_memory_node->is_used = true;
				p_mp->free_size -= node_and_mem_size;
				PRINT_LOG("============================memory pool============================\n");
				PRINT_LOG("memory_pool = %p;\n", p_mp->memory_pool);
				PRINT_LOG("memory_pool_size = %zu;\n", p_mp->size);
				PRINT_LOG("memory block size = %zu;\n", size);
				PRINT_LOG("memory block addr = %p;\n", p_memory_node->memory_addr);
				PRINT_LOG("free_size = %zu;\n", p_mp->free_size);
				PRINT_LOG("===================================================================\n");
				return p_memory_node->memory_addr;
			}
		}
	}

	return nullptr;
}

bool cg_free_memory(cg_memory_pool_info_t *p_mp, void *memory_addr) {
	if (memory_addr < p_mp->memory_pool || memory_addr >= p_mp->memory_pool + p_mp->size || memory_addr == nullptr) {
		PRINT_ERROR("this memory is not in the memory pool!\n");
		return false;
	}
	cg_memory_node_t *p_memory_node = (cg_memory_node_t *)(memory_addr - sizeof(cg_memory_node_t));
	cg_memory_node_t *p_prev_memory_node = nullptr;
	cg_memory_node_t *p_next_memory_node = nullptr;
	if (p_memory_node->prev_memory_node_addr != nullptr) {
		p_prev_memory_node = p_memory_node->prev_memory_node_addr;
	}
	if (p_memory_node != p_mp->p_last_memory_node) {
		p_next_memory_node = (cg_memory_node_t *)(memory_addr + p_memory_node->size);
	}

	size_t free_size = p_memory_node->size;
	int32_t memory_node_index = cg_get_memory_node_index(p_mp, memory_addr);

	// 如果该内存块是唯一的非空闲内存块，内存池全部清0，并且更新内存池的相关信息
	if (p_memory_node == p_mp->p_last_memory_node && p_mp->memory_count == 1) {
		p_mp->free_size = p_mp->size;
		p_mp->memory_count = 0;
		p_mp->p_last_memory_node = nullptr;
		memset(p_mp->memory_pool, 0, p_mp->size);
		p_mp->free_memory_node_count = 0;
		memset(p_mp->free_memory_node_addr_array, 0, p_mp->free_memory_node_addr_max_count * sizeof(cg_memory_node_t *));
		return true;
	}

	// 如果该内存块排在最后尾
	if (p_memory_node == p_mp->p_last_memory_node) {
		// 而且该内存块的前一个内存块已被释放
		if (p_prev_memory_node->is_used == false) {
			free_size = p_prev_memory_node->size + 2 * sizeof(cg_memory_node_t) + p_memory_node->size;
			p_mp->p_last_memory_node = p_prev_memory_node->prev_memory_node_addr;
			memset(p_prev_memory_node, 0, free_size);
			int32_t prev_memory_node_index = cg_get_memory_node_index(p_mp, p_prev_memory_node->memory_addr);
			if (prev_memory_node_index == -1) {
				PRINT_ERROR("fail!\n");
				return false;
			}
			cg_rm_one_p_memory_node(p_mp, prev_memory_node_index);
			p_mp->memory_count -= 2;
			p_mp->free_size += free_size;
			return true;
		} else {
			free_size = sizeof(cg_memory_node_t) + p_memory_node->size;
			p_mp->p_last_memory_node = p_prev_memory_node;
			memset(p_memory_node, 0, free_size);
			p_mp->memory_count--;
			p_mp->free_size += free_size;
			return true;
		}
	}

	// 如果该内存块排在内存池最前面,且后一个内存块存在
	if (p_prev_memory_node == nullptr && p_next_memory_node != nullptr) {
		// 而且该内存块的后一个内存块已被释放
		if (p_next_memory_node->is_used == false) {
			p_memory_node->is_used = false;
			size_t new_p_memory_node_size = p_memory_node->size + sizeof(cg_memory_node_t) + p_next_memory_node->size;
			p_memory_node->size = new_p_memory_node_size;
			cg_add_one_p_memory_node(p_mp, p_memory_node);
			int32_t next_memory_node_index = cg_get_memory_node_index(p_mp, p_next_memory_node->memory_addr);
			if (next_memory_node_index == -1) {
				PRINT_ERROR("cg_get_memory_node_index fail!\n");
				return false;
			}
			cg_rm_one_p_memory_node(p_mp, next_memory_node_index);
			memset(p_next_memory_node, 0, sizeof(cg_memory_node_t) + p_next_memory_node->size);
			p_mp->memory_count--;
			free_size += sizeof(cg_memory_node_t) + p_memory_node->size;
			p_mp->free_size += free_size;
			return true;
		} else if (p_next_memory_node->is_used == true) {
			p_memory_node->is_used = false;
			p_mp->memory_count--;
			cg_add_one_p_memory_node(p_mp, p_memory_node);
			free_size = p_memory_node->size;
			p_mp->free_size += free_size;
			memset(p_memory_node->memory_addr, 0, p_memory_node->size);
			return true;
		}
	}

	// 如果该内存块不排在最后尾,也不是排在内存池最前面的内存块,且该内存块的前后内存块都没被释放
	if (p_prev_memory_node != nullptr && p_next_memory_node->is_used == true && p_prev_memory_node->is_used == true) {
		p_memory_node->is_used = false;
		cg_add_one_p_memory_node(p_mp, p_memory_node);
		p_mp->free_size += free_size;
	}

	// 如果该内存块不排在最后尾,也不是排在内存池最前面的内存块,且该内存块的前一个内存块是空闲块,后一个不是
	if (p_prev_memory_node != nullptr && p_prev_memory_node->is_used == false && p_next_memory_node->is_used == true) {
		free_size = sizeof(cg_memory_node_t) + p_memory_node->size;
		size_t prev_memory_new_size = p_prev_memory_node->size + sizeof(cg_memory_node_t) + p_memory_node->size;
		p_prev_memory_node->size = prev_memory_new_size;
		p_next_memory_node->prev_memory_node_addr = p_prev_memory_node;
		p_mp->memory_count--;
		p_mp->free_size += free_size;
		return true;
	}

	// 如果该内存块不排在最后尾,也不是排在内存池最前面的内存块,且该内存块的后一个内存块是空闲块,前一个不是
	if (p_next_memory_node != nullptr && p_next_memory_node->is_used == false && p_prev_memory_node->is_used == true) {
		free_size = p_memory_node->size + sizeof(cg_memory_node_t);
		cg_memory_node_t *p_next_and_next_memory_node = (cg_memory_node_t *)(p_next_memory_node->memory_addr + p_next_memory_node->size);
		size_t memory_new_size = p_memory_node->size + sizeof(cg_memory_node_t) + p_next_memory_node->size;
		int32_t next_memory_node_index = cg_get_memory_node_index(p_mp, p_next_memory_node);
		if (next_memory_node_index == -1) {
			PRINT_ERROR("cg_get_memory_node_index fail!\n");
			return false;
		}
		p_mp->free_memory_node_addr_array[next_memory_node_index] = p_memory_node;
		p_next_and_next_memory_node->prev_memory_node_addr = p_memory_node;
		p_memory_node->size = memory_new_size;
		p_memory_node->is_used = false;
		p_mp->memory_count--;
		p_mp->free_size += free_size;
		return true;
	}

	return true;
}

size_t cg_get_memory_size(cg_memory_pool_info_t *p_mp, void *memory_addr) {
	if (memory_addr < p_mp->memory_pool || memory_addr >= p_mp->memory_pool + p_mp->size) {
		PRINT_ERROR("this memory is not in the memory pool!\n");
		return 0;
	}
	cg_memory_node_t *p_memory_node = (cg_memory_node_t *)(memory_addr - sizeof(cg_memory_node_t));
	return p_memory_node->size;
}

int32_t cg_get_memory_node_index(cg_memory_pool_info_t *p_mp, void *memory_addr) {
	if (memory_addr < p_mp->memory_pool || memory_addr >= p_mp->memory_pool + p_mp->size) {
		PRINT_ERROR("this memory is not in the memory pool!\n");
		return -1;
	}
	int32_t index = 0;
	for (index = 0; index < p_mp->free_memory_node_count; index++) {
		if (p_mp->free_memory_node_addr_array[index]->memory_addr == memory_addr) {
			return index;
		}
	}

	return -1;
}

bool cg_add_one_p_memory_node(cg_memory_pool_info_t *p_mp, cg_memory_node_t *memory_node_addr) {
	if (p_mp->free_memory_node_count == p_mp->free_memory_node_addr_max_count) {
		PRINT_ERROR("p_mp->free_memory_node_count == memory_node_addr_max_count!\n");
		return false;
	}
	if (p_mp->free_memory_node_count == 0) {
		p_mp->free_memory_node_addr_array[0] = memory_node_addr;
		p_mp->free_memory_node_count = 1;
		return true;
	}

	p_mp->free_memory_node_addr_array[p_mp->free_memory_node_count - 1] = memory_node_addr;
	p_mp->free_memory_node_count++;
	return true;
}

bool cg_rm_one_p_memory_node(cg_memory_pool_info_t *p_mp, uint32_t index) {
	if (p_mp->free_memory_node_count == 1) {
		memset(p_mp->free_memory_node_addr_array, 0, p_mp->free_memory_node_addr_max_count * sizeof(cg_memory_node_t *));
		p_mp->free_memory_node_count = 0;
		return true;
	}
	if (p_mp->free_memory_node_count - 1 > index) {
		p_mp->free_memory_node_addr_array[index] = p_mp->free_memory_node_addr_array[p_mp->free_memory_node_count - 1];
		p_mp->free_memory_node_addr_array[p_mp->free_memory_node_count - 1] = nullptr;
		p_mp->free_memory_node_count--;
		return true;
	}
	if (p_mp->free_memory_node_count - 1 == index) {
		p_mp->free_memory_node_addr_array[index] = nullptr;
		p_mp->free_memory_node_count--;
		return true;
	}

	return false;
}
