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

#include "cg_color.h"

uint32_t cg_change_RGB_color(uint32_t red, uint32_t green, uint32_t blue) {
#ifdef __linux
	return ((red << 16) | (green << 8) | blue);
#endif // __linux
#ifdef _WIN32
	return (red | (green << 8) | (blue << 16));
#endif // _WIN32
}

uint32_t cg_change_ARGB_color(uint32_t red, uint32_t green, uint32_t blue, uint32_t alpha) {
#ifdef __linux
	return ((alpha << 24) | (red << 16) | (green << 8) | blue);
#endif // __linux
#ifdef _WIN32
	return ((alpha << 24) | red | (green << 8) | (blue << 16));
#endif // _WIN32
}

uint32_t cg_change_AGBR_color(uint32_t red, uint32_t green, uint32_t blue, uint32_t alpha) {
	return ((alpha << 24) | red | (green << 8) | (blue << 16));
}

uint32_t cg_change_GBR_color(uint32_t red, uint32_t green, uint32_t blue) {
	return (red | (green << 8) | (blue << 16));
}
