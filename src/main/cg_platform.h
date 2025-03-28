#ifndef CG_PLATFORM_H
#define CG_PLATFORM_H 1
#ifdef __linux
#define MAIN int main(void)
#endif // Linux
#ifdef _WIN32
#include <windows.h>
#define MAIN int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
#endif // Windows
#endif // CG_PLATFORM_H 1
