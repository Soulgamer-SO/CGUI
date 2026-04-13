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

#include "cg_event_loop.h"
#ifdef __linux
#include <xcb/xcb_event.h>

void cg_event_loop(cg_info_t *p_info) {
	p_info->event_loop.is_running = true;
	while (p_info->event_loop.is_running && (p_info->event_loop.event = xcb_wait_for_event(p_info->wsi.xcb_surface_create_info.connection))) {
		switch (XCB_EVENT_RESPONSE_TYPE(p_info->event_loop.event)) {
		case XCB_EXPOSE: {
			xcb_expose_event_t *expose_event = (xcb_expose_event_t *)p_info->event_loop.event;
			if (expose_event->window == p_info->wsi.xcb_surface_create_info.window) {
				PRINT_LOG(
					"expose_event x = %i; expose_event y = %i; expose_event width = %i; expose_event height = %i;\n",
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

void cg_event_loop(cg_info_t *p_info) {
	p_info->event_loop.is_running = true;
	p_info->event_loop.msg.message = WM_NULL;
	GetMessage(&p_info->event_loop.msg, nullptr, 0, 0);
	while (p_info->event_loop.is_running && (p_info->event_loop.msg.message != WM_QUIT)) {
		if (PeekMessage(&p_info->event_loop.msg, nullptr, 0, 0, PM_REMOVE) != false) {
			TranslateMessage(&p_info->event_loop.msg);
			DispatchMessage(&p_info->event_loop.msg);
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
