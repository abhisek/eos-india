#include <heaplib.h>

AUDIT_FUNC(dfree_audit) {
   struct mem_ctx *p;
   
   p = (struct mem_ctx*) find_hl_table(table, mem->addr);
   if(!p)
      return; /* no previous state */

   if((p->flag & MEM_FREE) && (flag & MEM_FREE)) {
      /* free on already free'd mem */
      hl_text_log_write_with_backtrace("dfree: possible double free on mem 0x%08x", (unsigned long) mem->addr);
   }
}
