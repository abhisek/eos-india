#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

void nullderef()
{
   char *data = "AAAABBBBCCCCDDDD";
   void *ptr = NULL;
   int page_size = 4096;

   mmap(NULL, page_size, 
              PROT_READ | PROT_WRITE | PROT_EXEC,
              MAP_FIXED | MAP_ANONYMOUS | MAP_PRIVATE,
              0, 0);

   memcpy(ptr, data, strlen(data));

   asm("int3");
}

main()
{
   nullderef();
}
