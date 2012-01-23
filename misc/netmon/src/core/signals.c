#include <netmon.h>
#include <signal.h>


static void abort_handler(int signo)
{
   DEBUG_MSG("Caught SIGABRT: Aborting program");
}

void signals_init()
{
   DEBUG_MSG("Setting up signal handlers");
   
   signal(SIGABRT, abort_handler);
   return;
}
