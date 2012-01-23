#ifndef _HEAP_LIB_H
#define _HEAP_LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <queue.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <errno.h>
#define _GNU_SOURCE
#include <unistd.h>
#include <string.h>

#define MEM_ALLOCATED   0x01
#define MEM_REALLOCATED 0x02
#define MEM_FREE        0x04

struct mem_ctx {
   void *addr;
   int free_count;
   int malloc_count;
   int realloc_count;
   int flag;
};

struct hl_hash_entry {
   struct mem_ctx *mem;
   LIST_ENTRY(hl_hash_entry) linker;
};

LIST_HEAD(hl_hash_head, hl_hash_entry);

struct hl_hash_table {
   size_t size;
   struct hl_hash_head **heads;
};

#ifdef DEBUG
   #define debug_msg(fmt, arg...)   do { fprintf(stderr, "heaplib: " fmt "\n", ##arg); } while(0)
#else
   #define debug_msg(fmt, arg...)   do {} while(0)
#endif

#define AUDIT_FUNC(x)               void x(struct hl_hash_table *table, struct mem_ctx *mem, int flag)

#endif
