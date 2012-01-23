#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dlfcn.h>
#include <malloc.h>
#include <heaplib.h>
#ifndef RTLD_NEXT
#define RTLD_NEXT ((void *) -1l)
#endif
static struct hl_hash_table *hl_table;
static int heaplib_initialized;

unsigned long __real_malloc_hook;
unsigned long __real_realloc_hook;
unsigned long __real_free_hook;

#define MALLOC_WRAP_PRE    do {                                            \
                              __malloc_hook = (void*)__real_malloc_hook;   \
                              __realloc_hook = (void*)__real_realloc_hook; \
                              __free_hook = (void*)__real_free_hook;       \
                           } while(0)
#define MALLOC_WRAP_POST   do {                                            \
                              __malloc_hook = (void*)heaplib_malloc;       \
                              __realloc_hook = (void*)heaplib_realloc;     \
                              __free_hook = (void*)heaplib_free;           \
                           } while(0)

void hl_insert_mem_ctx_with_state(void **addr, int flag);
void *heaplib_malloc(size_t size, const void *caller);
void *heaplib_realloc(void *ptr, size_t size, void *caller);
void *heaplib_free(void *ptr, void *caller);

static init_heaplib() /* no loop plz k thx */
{
   if(heaplib_initialized)
      return; /* I don't want your babies */

   debug_msg("initializing heaplib");

   /* resolve the original addresses */
   __real_malloc_hook = (unsigned long)__malloc_hook;
   __real_realloc_hook = (unsigned long)__realloc_hook;
   __real_free_hook = (unsigned long)__free_hook;

   /* initialize the main hash table */
   init_hl_table(&hl_table, 128);

   /* initalize text logging */
   init_hl_text_log();
   
   __malloc_hook = (void*)heaplib_malloc;
   __realloc_hook = (void*)heaplib_realloc;
   __free_hook = (void*)heaplib_free;

   heaplib_initialized = 1;
}

void (*__malloc_initialize_hook) (void) = (void*) init_heaplib; /* lazy symbol */

void *heaplib_malloc(size_t size, const void *caller)
{
   void *ptr;
   
   init_heaplib();   /* I want your babies */
   
   MALLOC_WRAP_PRE;

   ptr = malloc(size);
   hl_text_log_write("malloc(%08ld) -> 0x%08x", size, (unsigned long) ptr);
   
   /* post state is valid here only */
   hl_insert_mem_ctx_with_state((void**)&ptr, MEM_ALLOCATED);

   MALLOC_WRAP_POST;
   return ptr;
}

void *heaplib_realloc(void *ptr, size_t size, void *caller)
{
   void *p;
   void *addr[2];

   init_heaplib();

   MALLOC_WRAP_PRE;

   p = realloc(ptr, size);
   hl_text_log_write("realloc(0x%08x, %08ld) -> 0x%08x", (unsigned long) ptr, size, (unsigned long) p);

   /* post state */
   addr[0] = p;   /* default */
   addr[1] = ptr; /* used only in realloc */
   hl_insert_mem_ctx_with_state(addr, MEM_ALLOCATED);

   MALLOC_WRAP_POST;
   return p;
}

void *heaplib_free(void *ptr, void *caller)
{
   init_heaplib();

   MALLOC_WRAP_PRE;

   /* pre state */
   hl_insert_mem_ctx_with_state((void**)&ptr, MEM_FREE);

   hl_text_log_write("free(0x%08x)", (unsigned long) ptr);
   free(ptr);

   MALLOC_WRAP_POST;
}

/*
 * we consider an array of addresses to avoid the apparent non-deterministic
 * nature of realloc
 */
void hl_insert_mem_ctx_with_state(void **addr, int flag)
{
   struct mem_ctx *mp = NULL;

   mp = (struct mem_ctx*) find_hl_table(hl_table, (unsigned long) addr[0]);
   if(!mp) {
      /* new chunk */
      mp = malloc(sizeof(*mp));
      assert(mp);
      
      mp->addr = addr[0];
      mp->flag |= flag;
      insert_hl_table(hl_table, mp);
      return;
   }
   
   if(mp->flag & flag) {
      hl_text_log_write("warning: 0x%08x already has %02d flag set", (unsigned long) mp->addr, flag);
   }
   
   /* audit without storing the new states */
   hl_do_audit(hl_table, mp, flag);
   
   /* stateful handlers */
   switch(flag) {
      case MEM_ALLOCATED:
         mp->flag &= ~MEM_FREE;
         mp->flag &= ~MEM_REALLOCATED;
         mp->flag |= MEM_ALLOCATED;
         mp->malloc_count++;
         break;

      case MEM_REALLOCATED:
         mp->flag = MEM_ALLOCATED;

         if(addr[0] == addr[1]) {
            /* same chunk */
         }
         else {
            /* new chunk */
            struct mem_ctx *tmp = (struct mem_ctx*) find_hl_table(hl_table, (unsigned long) addr[1]);
            if(tmp) {
               tmp->flag = MEM_REALLOCATED;
            }
         }
         break;

      case MEM_FREE:
         mp->flag &= ~MEM_ALLOCATED;
         mp->flag &= ~MEM_REALLOCATED;
         mp->flag |= MEM_FREE;
         mp->free_count++;
         break;
   }
}

void __attribute__((constructor)) hl_constructor()
{
   return;
}

void __attribute__((destructor)) hl_destructor()
{
#ifdef DEBUG
   dump_hl_table(hl_table);
#endif

   return;
}
