#ifndef _NETMON_H
#define _NETMON_H
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <getopt.h>

#include <queue.h>
#include <modules.h>
#include <ui.h>
#include <debug.h>

struct generic_node {
   void *ptr;

   LIST_ENTRY(generic_node) link;
};
LIST_HEAD(generic_list_head, generic_node);

struct generic_qnode {
   void *ptr;

   TAILQ_ENTRY(generic_qnode) link;
};
TAILQ_HEAD(generic_queue_head, generic_qnode);

#ifdef DEBUG
#define THREAD_MUTEX_INITIALIZER    PTHREAD_MUTEX_ERRORCHECK
#else
#define THREAD_MUTEX_INITIALIZER    PTHREAD_MUTEX_NORMAL
#endif

#endif
