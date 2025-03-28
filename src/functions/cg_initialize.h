#ifndef CG_INITIALIZE_H
#define CG_INITIALIZE_H 1
#include "cg_var.h"
#include <stdio.h>
#include <stdlib.h>

// 初始化变量，最好在这层函数一次性初始化所有变量
bool cg_initialize_var(cg_var_t *p_var);

#endif // CG_INITIALIZE_H 1
