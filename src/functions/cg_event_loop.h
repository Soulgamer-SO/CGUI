#ifndef CG_EVENT_LOOP_H
#define CG_EVENT_LOOP_H 1
#include "cg_var.h"

// 事件循环
void cg_event_loop(cg_var_t *p_var);

#ifdef _WIN32
LRESULT CALLBACK window_proc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);
#endif // _WIN32

#endif // CG_EVENT_LOOP_H 1
