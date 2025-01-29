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
