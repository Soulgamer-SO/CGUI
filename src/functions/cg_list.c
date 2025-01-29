#include "cg_list.h"

#ifdef DEBUG
char *cg_create_string(
	cg_memory_pool_var_t *p_var,
	char *string) {
	if (string == NULL) {
		return NULL;
	}
	char *new_string = (char *)cg_alloc_memory(p_var, (strlen(string) + 1) * sizeof(char));
	if (new_string == NULL) {
		return NULL;
	} else {
		PRINT_LOG("alloc memory success!\n");
	}
	strcpy(new_string, string);
	PRINT_LOG("%s\n", new_string);
	return new_string;
}

void *cg_add_one_at_list(
	cg_memory_pool_var_t *p_var,
	void *element,
	size_t type_size,
	uint32_t *p_count,
	void *p_list) {
	char *e = NULL;
	element = (void *)e;
	p_list = "123456";
	char *string = p_list;
	PRINT_LOG("%s\n", string);

	return NULL;
}

bool_t cg_remove_one_string(
	char *string_element,
	uint32_t *p_string_count,
	char ***p_string_list) {
	if (*p_string_count == 0) {
		return FALSE;
	}

	// 获得要被删除的元素的索引 find_index
	uint32_t find_index = 0;
	char **old_string_list = *p_string_list;
	uint32_t old_string_count = *p_string_count;
	uint32_t new_string_count = *p_string_count;
	for (uint32_t i = 0; i < old_string_count; i++) {
		bool_t is_string_equ = cg_ret_is_string_equ(old_string_list[i], string_element);
		if (is_string_equ == TRUE) {
			new_string_count--;
			find_index = i;
			break;
		}
	}

	// 如果find_index不存在
	if (find_index == 0 && new_string_count == old_string_count) {
		return FALSE;
	}

	char **new_string_list = (char **)malloc(new_string_count * sizeof(char *));
	if (new_string_list == NULL) {
		return FALSE;
	}

	// 如果find_index在末尾
	if (find_index == old_string_count) {
		for (uint32_t i = 0; i < new_string_count; i++) {
			new_string_list[i] = old_string_list[i];
		}
	}

	// 如果find_index在中间
	if (find_index > 0 && find_index < old_string_count) {
		for (uint32_t i = 0; i < find_index; i++) {
			new_string_list[i] = old_string_list[i];
		}
		for (uint32_t i = 0; i < old_string_count - find_index; i++) {
			new_string_list[i + find_index] = old_string_list[i + find_index + 1];
		}
	}

	// 如果find_index在起点
	if (find_index == 0 && new_string_count < old_string_count) {
		for (uint32_t i = 0; i < old_string_count; i++) {
			new_string_list[i] = old_string_list[i + 1];
		}
	}

	*p_string_count = new_string_count;
	*p_string_list = realloc(*p_string_list, new_string_count * sizeof(char *));
	*p_string_list = new_string_list;
	return TRUE;
}

bool_t cg_ret_is_string_equ(
	const char *string_1, const char *string_2) {
	if (strcmp(string_1, string_2) == 0) {
		return TRUE;
	} else {
		return FALSE;
	}
}
#endif
