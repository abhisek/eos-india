/*
 * Dummy netmon built in module
 */
#include <netmon.h>

static int dummy_init(void *ptr)
{
   DEBUG_MSG("Dummy module initialized");

   return 0;
}

static int dummy_deinit(void *ptr)
{
   DEBUG_MSG("Dummy module deinitialized");

   return 0;
}

static struct netmon_module dummy_mod = {
   .name          = "dummy",
   .module_init    = dummy_init,
   .module_deinit  = dummy_deinit
};

void dummy_main()
{
   module_register(&dummy_mod);

   DEBUG_MSG("Dummy module registered");
}
