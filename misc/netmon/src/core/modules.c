#include <netmon.h>

static LIST_HEAD(netmon_module_head, netmon_module) netmon_modules_list;
static char **mod_names;
static int mod_count;

void module_free_names()
{
   char *ptr;
   int i;

   if(mod_names) {
      i = 0;
      while(mod_names[i]) {
         ptr = mod_names[i];
         free(ptr);
         i++;
      }

      free(mod_names);
      mod_names = NULL;
      mod_count = 0;
   }
}

/*
 * This function returns pointers to initialized and allocated data
 * Output should be considered read only
 */
const char **module_get_names()
{
   int c, i;
   struct netmon_module *m;
   char *ptr;

   if(!mod_names) {
      mod_names = (char**) malloc(sizeof(char**) * 1);
      mod_names[0] = NULL;
      mod_count = 0;
   }

   if(LIST_EMPTY(&netmon_modules_list))
      return (const char**) mod_names;

   c = 0;
   LIST_FOREACH(m, &netmon_modules_list, link) {
      c++;
   }

   if(c > mod_count) {
      i = 0;
      while(mod_names[i]) {
         ptr = mod_names[i];
         free(ptr);
         i++;
      }
      
      i = 0;
      mod_names = (char**) realloc(mod_names, c + 1);
      LIST_FOREACH(m, &netmon_modules_list, link) {
         mod_names[i] = (char*) strndup(m->name, strlen(m->name));
         i++;
      }
      mod_names[i] == NULL;
      mod_count = c;
   }

   return (const char**) mod_names;
}

struct netmon_module *module_find(const char *name)
{
   struct netmon_module *m;
   struct netmon_module *p;

   if(name == NULL)
      return;

   DEBUG_MSG("Searching for module %s", name);

   p = NULL;
   LIST_FOREACH(m, &netmon_modules_list, link) {
      if(strncmp(m->name, name, strlen(m->name)) == 0) {
         DEBUG_MSG("Found");
         p = m;
         break;
      }
   }

   return p;
}

int module_activate(const char *name, void *opts)
{
   struct netmon_module *m;
   int ret = 1;

   m = module_find(name);
   if(m != NULL) {
      DEBUG_MSG("Activating module %s", m->name);
      m->module_init(opts);
      ret = 0;
   }

   return ret;
}

/*
 * Register a new module to the list
 * nm must point to an allocated and initialized block
 */
int module_register(struct netmon_module *nm)
{
   if(nm == NULL)
      return;

   DEBUG_MSG("Registering module %s", nm->name);

   LIST_INSERT_HEAD(&netmon_modules_list, nm, link);
}

void modules_init()
{
   DEBUG_MSG("Initializing modules list");

   LIST_INIT(&netmon_modules_list);
   module_get_names();
}

void modules_deinit()
{
   struct netmon_module *m;

   DEBUG_MSG("Deinitializing modules list");
   LIST_FOREACH(m, &netmon_modules_list, link) {
      m->module_deinit(NULL);
   }

   module_free_names();
}
