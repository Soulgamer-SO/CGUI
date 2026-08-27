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

#ifndef CG_RENDER_H
#define CG_RENDER_H 1
#include "cg_info.h"

bool cg_draw_frame(cg_info_t *p_info);
bool cg_create_render_resources(cg_info_t *p_info);
void cg_destroy_render_resources(cg_info_t *p_info);

#endif // CG_RENDER_H 1
