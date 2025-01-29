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
