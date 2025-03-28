#ifndef CG_LIST_H
#define CG_LIST_H 1
#include "cg_memory_pool.h"
#include <stdlib.h>
#include <string.h>
typedef unsigned char byte_t;

#ifdef DEBUG
char *cg_create_string(
	cg_memory_pool_var_t *p_var,
	char *string);

// 在列表里添加一个元素到末尾
void *cg_add_one_at_list(
	cg_memory_pool_var_t *p_var,
	void *element,
	size_t type_size,
	uint32_t *p_count,
	void *p_list);

// 在字符串列表里删除一个指定的字符串元素
bool cg_remove_one_string(
	char *string_element,
	uint32_t *p_string_count,
	char ***p_string_list);

// 比较字符串是否相等
bool cg_ret_is_string_equ(
	const char *string_1,
	const char *string_2);
#endif
#endif // CG_LIST_H 1
