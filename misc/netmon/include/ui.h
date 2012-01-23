#ifndef _NETMON_UI_H
#define _NETMON_UI_H
#include <netmon.h>

#define MSG_INFO     1
#define MSG_WARN     2
#define MSG_CRIT     3
struct netmon_ui {
   char *name;
   int (*ui_init)(void *ptr);
   int (*ui_deinit)(void *ptr);
   
   /*
    * We use two types of UI messaging:
    *
    * ui_msg:  Immediately shows the messages irrespective of
    * what other threads are doing.
    *
    * ui_queue_msg: Queues the message and the dispatcher routine
    * processes it.
    */
   int (*ui_msg)(int level, const char *msg);
   int (*ui_queue_msg)(int level, const char *msg);

   LIST_ENTRY(netmon_ui) link;
};
#endif
