#include <heaplib.h>

int init_hl_table(struct hl_hash_table **table, size_t size)
{
   int i;
   struct hl_hash_table *tbl;

   debug_msg("initializing new hash table");
   tbl = (struct hl_hash_table*) malloc(sizeof(*tbl));
   assert(tbl);

   tbl->size = size;
   tbl->heads = (struct hl_hash_head**) malloc(sizeof(void*) * tbl->size);
   assert(tbl->heads);

   for(i = 0; i < tbl->size; i++) {
      tbl->heads[i] = (struct hl_hash_head*) malloc(sizeof(struct hl_hash_head));
      assert(tbl->heads[i]);

      LIST_INIT(tbl->heads[i]);
   }

   *table = tbl;
   
   debug_msg("hash table initialized -> 0x%08x", (unsigned long) tbl);
   return 0;
}

int insert_hl_table(struct hl_hash_table *table, struct mem_ctx *mem)
{
   unsigned long pos;
   struct hl_hash_entry *entry;

   assert(table);
   pos = ((unsigned long)mem->addr) % ((unsigned long)table->size);
   
   entry = malloc(sizeof(*entry));
   assert(entry);
   entry->mem = mem;

   LIST_INSERT_HEAD(table->heads[pos], entry, linker);

   return 0;
}

struct mem_ctx *find_hl_table(struct hl_hash_table *table, unsigned long addr)
{
   unsigned long pos;
   struct hl_hash_entry *entry;

   assert(table);
   pos = addr % table->size;

   if(LIST_EMPTY(table->heads[pos]))
      return NULL;

   LIST_FOREACH(entry, table->heads[pos], linker) {
      if((unsigned long)entry->mem->addr == addr)
         return entry->mem;
   }

   return NULL;
}

/* stderr dump */
void dump_hl_table(struct hl_hash_table *table)
{
   int i;
   struct hl_hash_head *head;
   struct hl_hash_entry *entry;

   assert(table);
   for(i = 0; i < table->size; i++) {
      head = table->heads[i];

      LIST_FOREACH(entry, head, linker) {
         fprintf(stderr, "mem: 0x%08x [ALLOC_FLAG: %02d REALLOC_FLAG: %02d FREE_FLAG: %02d]\n",
                  (unsigned long) entry->mem->addr,
                  entry->mem->flag & MEM_ALLOCATED ? 1 : 0,
                  entry->mem->flag & MEM_REALLOCATED ? 1 : 0,
                  entry->mem->flag & MEM_FREE ? 1 : 0
                );
      }
   }
}
