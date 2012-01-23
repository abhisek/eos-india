/*
 * System memory allocator wrapper
 */

#include <main.h>

void *xmalloc(size_t size)
{
   void *p;

   if((int)size > (int)0) {
      p = (void*) malloc(size);

      if(p == NULL) {
         DEBUG_MSG("Memory allocation failed");
         exit(EXIT_FAILURE);
      }

      return p;
   }
   
   DEBUG_MSG("Invalid memory request of size %d", size);
   return NULL;
}

void xfree(void *p)
{
   if(p != NULL)
      free(p);
}
