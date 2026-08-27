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
#include "cg_render.h"

#ifdef LINUX
#include <xcb/xcb_event.h>

void cg_event_loop(cg_info_t *p_info) {
    xcb_connection_t *connection = p_info->wsi.xcb_surface_create_info.connection;
    xcb_window_t window = p_info->wsi.xcb_surface_create_info.window;

    p_info->event_loop.is_running = true;
    p_info->event_loop.fd = xcb_get_file_descriptor(connection);

    while (p_info->event_loop.is_running) {
        while ((p_info->event_loop.event = xcb_poll_for_event(connection)) != nullptr) {
            switch (XCB_EVENT_RESPONSE_TYPE(p_info->event_loop.event)) {
            case XCB_EXPOSE: {
                xcb_expose_event_t *expose_event = (xcb_expose_event_t *)p_info->event_loop.event;
                if (expose_event->window == window) {
                    PRINT_LOG(
                        "expose_event x = %i; expose_event y = %i; expose_event width = %i; expose_event height = %i;\n",
                        expose_event->x,
                        expose_event->y,
                        expose_event->width,
                        expose_event->height);
                }
                break;
            }
            case XCB_CLIENT_MESSAGE:
                if (((xcb_client_message_event_t *)p_info->event_loop.event)->window == window &&
                    ((xcb_client_message_event_t *)p_info->event_loop.event)->data.data32[0] == p_info->wsi.XCB_API_info.wm_delete_window_atom) {
                    p_info->event_loop.is_running = false;
                }
                break;
            case XCB_DESTROY_NOTIFY:
                if (((xcb_destroy_notify_event_t *)p_info->event_loop.event)->window == window) {
                    p_info->event_loop.is_running = false;
                }
                break;
            default:
                break;
            }

            free(p_info->event_loop.event);
            p_info->event_loop.event = nullptr;
            if (!p_info->event_loop.is_running) {
                break;
            }
        }

        if (!p_info->event_loop.is_running) {
            break;
        }

        if (xcb_connection_has_error(connection) != 0) {
            p_info->event_loop.is_running = false;
            break;
        }

        if (!cg_draw_frame(p_info)) {
            p_info->event_loop.is_running = false;
        }
    }

    return;
}
#endif // LINUX

#ifdef WINDOWS
#include "cg_input.h"

void cg_event_loop(cg_info_t *p_info) {
    p_info->event_loop.is_running = true;
    while (p_info->event_loop.is_running) {
        while (PeekMessage(&p_info->event_loop.msg, nullptr, 0, 0, PM_REMOVE) != false) {
            if (p_info->event_loop.msg.message == WM_QUIT) {
                p_info->event_loop.is_running = false;
                break;
            }

            TranslateMessage(&p_info->event_loop.msg);
            DispatchMessage(&p_info->event_loop.msg);
        }

        if (!p_info->event_loop.is_running) {
            break;
        }

        if (!cg_draw_frame(p_info)) {
            p_info->event_loop.is_running = false;
        }
    }

    return;
}

LRESULT CALLBACK window_proc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
    case WM_KEYDOWN:
        if (wparam == KEY_Q &&
            (GetKeyState(VK_CONTROL) & 0x8000) != 0) {
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
#endif // WINDOWS
