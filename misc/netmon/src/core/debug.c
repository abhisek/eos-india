/* 
 * Implemented in debug.h
 */
#include <netmon.h>

void dbg_hexdump(const char *data, size_t len, int maxcol, FILE *fp)
{
   int i;

   if(!maxcol)
      maxcol = 80;

   if(!fp)
      fp = stderr;

   for(i = 0; i < len; i++) {
      fprintf(fp, "\\x%02x", data[i]);

      if(!(i % maxcol))
         fprintf(fp, "\n");
   }
}
