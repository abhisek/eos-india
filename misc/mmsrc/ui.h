#ifndef _UI_H
#define _UI_H

#include <main.h>

#define UI_FLUSH()         fflush(stdout)
#define UI_MSG(x, ...)     fprintf(stdout, "MSG: " x "\n", ##__VA_ARGS__);
#define UI_PRINT(x, ...)   fprintf(stdout, x, ##__VA_ARGS__);
#define UI_BUG(x, ...)     fprintf(stdout, "BUG: " x "\n", ##__VA_ARGS__);

#endif
