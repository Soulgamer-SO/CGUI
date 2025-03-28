#ifndef CG_WSI_H
#define CG_WSI_H 1
#include "cg_var.h"
#ifdef __linux
#include <xcb/xcb_icccm.h>
#include <xcb/xcb_util.h>
#include <xcb/xproto.h>
#endif // __linux
#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

// 创建窗口
bool cg_create_window(cg_var_t *p_var);

#endif // CG_WSI_H 1
