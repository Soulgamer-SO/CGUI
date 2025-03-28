#include "cg_event_loop.h"
#ifdef __linux
#include <xcb/xcb_event.h>

void cg_event_loop(cg_var_t *p_var) {
	p_var->event_loop_var.is_running = true;
	while (p_var->event_loop_var.is_running && (p_var->event_loop_var.event = xcb_wait_for_event(p_var->wsi_var.xcb_surface_create_info.connection))) {
		switch (XCB_EVENT_RESPONSE_TYPE(p_var->event_loop_var.event)) {
		case XCB_EXPOSE: {
			xcb_expose_event_t *expose_event = (xcb_expose_event_t *)p_var->event_loop_var.event;
			if (expose_event->window == p_var->wsi_var.xcb_surface_create_info.window) {
				PRINT_LOG(
					"expose! expose_event x = %i; expose_event y = %i; expose_event width = %i; expose_event height = %i;\n",
					expose_event->x,
					expose_event->y,
					expose_event->width,
					expose_event->height);
			}
		} break;
		default:
			/* Unknown event type, ignore it */
			break;
		}
	}

	return;
}
#endif // __linux

#ifdef _WIN32
#include "cg_input.h"

void cg_event_loop(cg_var_t *p_var) {
	p_var->event_loop_var.is_running = true;
	p_var->event_loop_var.msg.message = WM_NULL;
	GetMessage(&p_var->event_loop_var.msg, nullptr, 0, 0);
	while (p_var->event_loop_var.is_running && (p_var->event_loop_var.msg.message != WM_QUIT)) {
		if (PeekMessage(&p_var->event_loop_var.msg, nullptr, 0, 0, PM_REMOVE) != false) {
			TranslateMessage(&p_var->event_loop_var.msg);
			DispatchMessage(&p_var->event_loop_var.msg);
		}
	}

	return;
}

LRESULT CALLBACK window_proc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch (msg) {
	case WM_KEYDOWN:
		if (wparam == (VK_CONTROL | KEY_Q)) {
			DestroyWindow(wnd);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(wnd, msg, wparam, lparam);
	}

	return 0;
}
#endif // _WIN32
