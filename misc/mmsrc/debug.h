#ifndef _DEBUG_H
#define _DEBUG_H

#include <main.h>

#ifdef DEBUG
#define DEBUG_MSG(x, ...)  fprintf(stderr, "DEBUG: " x "\n", ##__VA_ARGS__);
#else
#define DEBUG_MSG(x, ...)
#endif

#endif

