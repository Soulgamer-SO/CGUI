#ifndef CG_COLOR_H
#define CG_COLOR_H 1
#include <stdint.h>

// 在Linux下顺序RGB
#ifdef __linux
#define CG_RGB_RED 0xFF0000U
#define CG_RGB_GREEN 0x00FF00U
#define CG_RGB_BLUE 0x0000FFU
#define CG_RGB_WHITE 0x000000U
#define CG_RGB_BLACK 0xFFFFFFU
#define CG_ARGB_RED 0x00FF0000U
#define CG_ARGB_GREEN 0x0000FF00U
#define CG_ARGB_BLUE 0x000000FFU
#define CG_ARGB_WHITE 0x00000000U
#define CG_ARGB_BLACK 0x00FFFFFFU
#endif // __linux

// 在Windos下顺序BGR
#ifdef _WIN32
#define CG_RGB_RED 0x0000FFU
#define CG_RGB_GREEN 0x00FF00U
#define CG_RGB_BLUE 0xFF0000U
#define CG_RGB_WHITE 0xFFFFFFU
#define CG_RGB_BLACK 0x000000U
#define CG_ARGB_RED 0x000000FFU
#define CG_ARGB_GREEN 0x0000FF00U
#define CG_ARGB_BLUE 0x00FF0000U
#define CG_ARGB_BLACK 0x00000000U
#define CG_ARGB_WHITE 0x00FFFFFFU
#endif // _WIN32

// 改变ARGB格式颜色
uint32_t cg_change_ARGB_color(uint32_t red, uint32_t green, uint32_t blue, uint32_t alpha);

// 改变RGB格式颜色
uint32_t cg_change_RGB_color(uint32_t red, uint32_t green, uint32_t blue);

// 改变AGBR格式颜色
uint32_t cg_change_AGBR_color(uint32_t red, uint32_t green, uint32_t blue, uint32_t alpha);

// 改变GBR格式颜色
uint32_t cg_change_GBR_color(uint32_t red, uint32_t green, uint32_t blue);

#endif // CG_COLOR_H 1
