#include <netmon.h>

/*
 * Apparently it might look like there is a scope of race here,
 * but I think UI will always be set before module initialization,
 * ie. firing of multiple threads. 
 */
static LIST_HEAD(netmon_ui_head, netmon_ui) netmon_ui_list;
static struct netmon_ui *current_ui;

int ui_queue_msg(int level, const char *fmt, ...)
{
   char ui_msg[4096];
   va_list ap;

   if(current_ui) {
      va_start(ap, fmt);
      vsnprintf(ui_msg, sizeof(ui_msg) - 1, fmt, ap);
      va_end(ap);

      current_ui->ui_queue_msg(level, ui_msg);
   }
}

int ui_msg(int level, const char *fmt, ...)
{
   char ui_msg[4096];
   va_list ap;

   if(current_ui) {
      va_start(ap, fmt);
      vsnprintf(ui_msg, sizeof(ui_msg) - 1, fmt, ap);
      va_end(ap);
      
      current_ui->ui_msg(level, ui_msg);
   }
}

struct netmon_ui *ui_find(const char *name)
{
   struct netmon_ui *u;
   struct netmon_ui *p;

   if(name == NULL)
      return;

   DEBUG_MSG("Searching for UI %s", name);

   p = NULL;
   LIST_FOREACH(u, &netmon_ui_list, link) {
      if(strncmp(u->name, name, strlen(u->name)) == 0) {
         DEBUG_MSG("Found");
         p = u;
      }
   }

   return p;
}

void ui_set_current(const char *name)
{
   struct netmon_ui *ui;

   if(name == NULL)
      return;

   ui = ui_find(name);
   if(ui) {
      DEBUG_MSG("Setting current UI to %s", ui->name);
      ui->ui_init(NULL);
      current_ui = ui;
   }
}

void ui_set_current_ui(struct netmon_ui *ui)
{
   if(ui == NULL)
      return;

   DEBUG_MSG("Setting current UI to %s", ui->name);
   ui->ui_init(NULL);
   current_ui = ui;
}
/*
 * Register a new UI to the list
 * ui must point to allocated and initialized block
 */
void ui_register(struct netmon_ui *ui)
{
   if(ui == NULL)
      return;

   DEBUG_MSG("Registering UI %s", ui->name);

   LIST_INSERT_HEAD(&netmon_ui_list, ui, link);
}

void ui_init()
{
   DEBUG_MSG("Initializing UI");

   LIST_INIT(&netmon_ui_list);
}

void ui_deinit()
{
   DEBUG_MSG("Deinitializing UI");
}
