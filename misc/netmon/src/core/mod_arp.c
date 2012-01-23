#include <netmon.h>
#include <st_hash.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/ethernet.h>

static int initialized;

static int arp_init(void *ptr)
{
   if(initialized) {
      DEBUG_MSG("ARP module already initialized");
      return 1;
   }

   DEBUG_MSG("ARP Monitor module initialized");
   return 0;
}

static int arp_deinit(void *ptr)
{
   if(!initialized)
      return 1;

   initialized = 0;
   DEBUG_MSG("ARP module deinitialized");
   return 0;
}

static struct netmon_module arp_mod = {
   .name          = "arp",
   .module_init   = arp_init,
   .module_deinit = arp_deinit,
};

void arp_main()
{
   module_register(&arp_mod);

   DEBUG_MSG("ARP Module registered");
}
