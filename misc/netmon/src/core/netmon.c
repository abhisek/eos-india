#include <netmon.h>

#define PROG_NAME    "NetMon - boring HIPS wannabe"
#define PROG_VER     "0.1"

/*
 * global control flags
 */
int daemonize = 0;
int debug = 0;
int verbose = 0;
char *enabled_ui = NULL;
struct generic_list_head enabled_modules;


static void interact()
{
   while(1) {
      sleep(1);
   }

   return;
}

static void show_help()
{
   char sbuf[65535];
   int c, i;
   char **modules;

   c  = 0;
   c += snprintf(sbuf + c, sizeof(sbuf) - c - 1, "\n");
   c += snprintf(sbuf + c, sizeof(sbuf) - c - 1, "%s %s\n", PROG_NAME, PROG_VER);
   c += snprintf(sbuf + c, sizeof(sbuf) - c - 1, "\n");
   c += snprintf(sbuf + c, sizeof(sbuf) - c - 1, "\tOptions:\n");
   c += snprintf(sbuf + c, sizeof(sbuf) - c - 1, "\t-d    --daemon               Runs in daemon mode\n");
   c += snprintf(sbuf + c, sizeof(sbuf) - c - 1, "\t-D    --debug                Runs in debug mode\n");
   c += snprintf(sbuf + c, sizeof(sbuf) - c - 1, "\t-m    --modules m1,m2 ...    Comma seperated list of modules to activate\n");
   c += snprintf(sbuf + c, sizeof(sbuf) - c - 1, "\t-u    --ui UI                Select UI to use for messaging\n");
   c += snprintf(sbuf + c, sizeof(sbuf) - c - 1, "\t-i    --interface DEVICE     Network Interface to monitor (Defaults to all)\n");
   c += snprintf(sbuf + c, sizeof(sbuf) - c - 1, "\t-f    --filter PCAP FILTER   Pcap style filter to use while packet capture\n");
   c += snprintf(sbuf + c, sizeof(sbuf) - c - 1, "\n");

   c += snprintf(sbuf + c, sizeof(sbuf) - c - 1, "\tAvailable Modules:\n");
   i = 0;
   modules = (char**) module_get_names();
   while(modules[i]) {
      c += snprintf(sbuf + c, sizeof(sbuf) - c - 1, "\t\t%s\n", modules[i]);
      i++;
   }
   c += snprintf(sbuf + c, sizeof(sbuf) - c - 1, "\n");

   c += snprintf(sbuf + c, sizeof(sbuf) - c - 1, "\tAvailable UI:\n");
   c += snprintf(sbuf + c, sizeof(sbuf) - c - 1, "\n");

   fprintf(stderr, "%s", sbuf);
}

static void process_cmdline(int argc, char **argv)
{
   static char *optstring = "hdDm:u:i:f:";
   static struct option options[] = {
      {"help", no_argument, NULL, 'h'},
      {"daemon", no_argument, NULL, 'd'},
      {"debug", no_argument, NULL, 'D'},
      {"modules", required_argument, NULL, 'm'},
      {"ui", required_argument, NULL, 'u'},
      {"interface", required_argument, NULL, 'i'},
      {"filter", required_argument, NULL, 'f'},
      {NULL, 0, NULL, 0},
   };
   int c;
   char *ptr;
   char *chp;
   struct generic_node *gn;

   if(argc == 1) {
      show_help();
      exit(EXIT_FAILURE);
   } else {
      while((c = getopt_long(argc, argv, optstring, options, NULL)) != -1) {
         switch(c) {
            case 'h':
               show_help();
               //conf_set_opt("help", 1);

               exit(EXIT_FAILURE);
            case 'd':
               daemonize = 1;
               conf_set_opt("daemonize", 1);

               break;
            case 'D':
               debug = 1;
               conf_set_opt("debug", 1);

               break;
            case 'm':
               ptr = strdup(optarg);
               assert(ptr != NULL);

               conf_set_opt("modules", (unsigned long) strdup(ptr));
               
               if(strchr(ptr, ',') == NULL) {
                  gn = malloc(sizeof(struct generic_node));
                  assert(gn != NULL);
                  gn->ptr = strdup(ptr);
                  assert(gn->ptr != NULL);

                  LIST_INSERT_HEAD(&enabled_modules, gn, link);
               }
               else {
                  while((chp = strchr(ptr, ',')) != NULL) {
                     *chp = '\0';
                     gn = malloc(sizeof(struct generic_node));
                     assert(gn != NULL);

                     gn->ptr = strdup(ptr);
                     assert(gn->ptr != NULL);
                     LIST_INSERT_HEAD(&enabled_modules, gn, link);

                     ptr = chp + 1;
                  }
               }
               break;
            case 'u':
               enabled_ui = (char*) strdup(optarg);
               conf_set_opt("ui", (unsigned long) strdup(enabled_ui));

               break;
            case 'i':
               conf_set_opt("interface", (unsigned long) strdup(optarg));

               break;
            case 'f':
               conf_set_opt("filter", (unsigned long) strdup(optarg));

               break;
            default:
               show_help();
               exit(EXIT_FAILURE);
         }
      }
   }
}

static void init()
{
   struct generic_node *n;
   struct netmon_module *m;
   struct netmon_ui *u;

   DEBUG_MSG("Initializing activated modules");
   LIST_FOREACH(n, &enabled_modules, link) {
      module_activate(n->ptr, NULL);
   }
   
   DEBUG_MSG("Initializing activated UI");
   ui_set_current(enabled_ui);

   return;
}

int main(int argc, char **argv)
{
   
   /*
    * initialize subcomponents
    */
   modules_init();
   ui_init();
   conf_init();

   /*
    * Initialize the signal handlers
    */
   signals_init();

   /*
    * register built in UIs
    */
   ui_text_main();
   ui_set_current("text"); /* default fall back */

   /*
    * register built in modules
    */
   dummy_main();
   arp_main();

   /*
    * process command line
    */
   LIST_INIT(&enabled_modules);
   process_cmdline(argc, argv);

   /*
    * initialize packet subsystem
    */
   packet_init();
   decoder_init();

   /*
    * initialize
    */
   init();

   /*
    * interactive control loop
    */
   interact();

   /*
    * deinitialize
    */
   modules_deinit();
   ui_deinit();
   conf_deinit();

   return 0;
}
