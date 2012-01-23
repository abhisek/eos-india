#ifndef _NETMON_DEBUG_H
#define _NETMON_DEBUG_H

#include <netmon.h>

#ifdef DEBUG
extern int debug;
#define DEBUG_MSG(msg, args...) do { if(debug) fprintf(stderr, "[DEBUG]: " msg "\n", ##args); } while(0);
#else
#define DEBUG_MSG(msg, args...) do { } while(0)
#endif

#endif
