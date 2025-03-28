#ifndef CG_LOAD_LIBRARY_H
#define CG_LOAD_LIBRARY_H 1
#include "cg_var.h"
#ifdef __linux
#include <dlfcn.h>
#endif // __linux

// 加载函数库
bool cg_load_library(cg_var_t *p_var);

#endif // CG_LOAD_LIBRARY_H 1
