#include <netmon.h>
#include <st_hash.h>

static st_table *conf_table;
static int initialized;

/*
 * for integer value, expect unsigned long in *val
 * for string, expect pointer to string in *val
 */
int conf_get_opt(const char *key, unsigned long *val)
{
   if(!initialized) {
      DEBUG_MSG("conf_get_opt: uninitialized subsystem");
      return -1;
   }

   return st_lookup(conf_table, (st_data_t)key, (st_data_t*) val);
}

/*
 * for integer value, it will be stored in val
 * for string, the pointer will be stored in val
 */
int conf_set_opt(const char *key, unsigned long val)
{
   if(!initialized) {
      DEBUG_MSG("conf_set_opt: uninitialized subsystem");
      return -1;
   }
   
   DEBUG_MSG("Setting configuration option %s to 0x%08x", key, val);

   return st_insert(conf_table, (st_data_t) key, (st_data_t) val);
}

void conf_init()
{
   DEBUG_MSG("Initializing configurations");

   conf_table = st_init_strtable();
   assert(conf_table != NULL);

   initialized = 1;
}

void conf_deinit()
{
   DEBUG_MSG("Deinitializing configurations");
   
   if(conf_table && initialized) {
      st_free_table(conf_table);
      conf_table = NULL;

      initialized = 0;
   }
}
