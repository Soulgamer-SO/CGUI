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

#ifndef CG_LOG_H
#define CG_LOG_H 1

#ifdef DEBUG
#include <stdio.h>
#define LOG_COLOR_NONE "\033[0m"
#define LOG_COLOR_RED "\033[0;31m"
#define PRINT_LOG(fmt, args...)                                  \
	printf(LOG_COLOR_NONE "%s(%d)LOG:", __FILE__, __LINE__); \
	printf(fmt, ##args);                                     \
	printf(LOG_COLOR_NONE);

#define PRINT_ERROR(fmt, args...)                                      \
	printf(LOG_COLOR_RED "%s(%d)LOG:ERROR! ", __FILE__, __LINE__); \
	printf(fmt, ##args);                                           \
	printf(LOG_COLOR_NONE);
#else
#define PRINT_LOG
#define PRINT_ERROR
#endif // DEBUG

#endif // CG_LOG_H 1
