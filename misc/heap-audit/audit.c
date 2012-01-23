#include <heaplib.h>

/* abstraction layer for auditing */
void hl_do_audit(struct hl_hash_table *table, struct mem_ctx *mem, int flag)
{
   debug_msg("auditing mem 0x%08x with flag %02d", (unsigned long) mem->addr, flag);

   /* call other auditing functions here */
   dfree_audit(table, mem, flag);

   return;
}

/*
 * Audit funcs are called with 3 parameters:
 * states table, memory context, flag of operation
 *
 * Audit functions must depend on state table for its history/information needs
 * for computing output
 */
