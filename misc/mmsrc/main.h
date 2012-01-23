#ifndef _MAIN_H
#define _MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/times.h>
#include <sys/select.h>
#include <signal.h>
#include <pthread.h>
#include <fcntl.h>
#include <queue.h>
#include <shared.h>
#include <ui.h>
#include <debug.h>

#ifdef DEBUG
#define THREAD_MUTEX_INITIALIZER    PTHREAD_MUTEX_ERRORCHECK
#else
#define THREAD_MUTEX_INITIALIZER    PTHREAD_MUTEX_NORMAL
#endif

/*
 * Memory Frame
 */
struct memory_frame {
   unsigned int counter;
   int *content;
};

/*
 * Page Fault Request/Response
 */
struct pf_msg {
   char mode;
   int page_idx;
   int word_idx;
   int content;

   TAILQ_ENTRY(pf_msg) link;
};

TAILQ_HEAD(pf_request_head, pf_msg);
TAILQ_HEAD(pf_response_head, pf_msg);

extern char *memfile;
extern char *access_file;
extern int P, M, N, T, F, R;
extern int *page_table;
extern int **vm;
extern struct memory_frame *memory;
extern struct pf_request_head pf_requests;
extern struct pf_response_head pf_responses;
extern pthread_t pam_thread;
extern pthread_t pfh_thread;
extern pthread_t arh_thread;


#endif
