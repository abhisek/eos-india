#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#undef memcpy
#define bos0(dest) __builtin_object_size (dest, 0)
#define memcpy(dest, src, n)  __builtin___memcpy_chk (dest, src, n, bos0 (dest))

main()
{
   char tmp[10];
   char buff[5];
   int x;

   printf("doing\n");

   memcpy(buff, "AAAAAA", 6);
   memcpy(tmp, "AAAAAA", 6);

   printf("done\n");
   
   x = buff[0];
   memcpy(buff, tmp, 6);
}
