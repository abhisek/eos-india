#ifndef _NETMON_MODULES_H
#define _NETMON_MODULES_H
#include <netmon.h>

struct netmon_module {
   char *name;
   int (*module_init)(void *ptr);
   int (*module_deinit)(void *ptr);

   LIST_ENTRY(netmon_module) link;
};
#endif
