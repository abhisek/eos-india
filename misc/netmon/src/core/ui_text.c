/*
 * default text UI for netmon
 */
#include <netmon.h>

static pthread_t ui_thread;
static pthread_mutex_t ui_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t ui_list_mutex = PTHREAD_MUTEX_INITIALIZER;
static int initialized;

#define UI_LOCK         pthread_mutex_lock(&ui_mutex)
#define UI_UNLOCK       pthread_mutex_unlock(&ui_mutex)
#define UI_QUEUE_LOCK    pthread_mutex_lock(&ui_list_mutex)
#define UI_QUEUE_UNLOCK  pthread_mutex_unlock(&ui_list_mutex)

static struct generic_queue_head ui_msg_queue;
static int text_msg(int level, const char *msg); /* prototype */

struct ui_msg_elm {
   int level;
   char *msg;
};

static void text_ui_dispatcher(void *ptr)
{
   int ret;
   int oldstate;
   int oldtype;
   struct generic_qnode *n;
   
   DEBUG_MSG("Starting Text UI dispatcher");
   ret = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldstate);
   ret |= pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype);
   assert(ret == 0);
   
   DEBUG_MSG("Entering Text UI dispatcher loop");
   while(1) {
      UI_QUEUE_LOCK;

      if(!TAILQ_EMPTY(&ui_msg_queue)) {

         n = TAILQ_FIRST(&ui_msg_queue);
         TAILQ_REMOVE(&ui_msg_queue, n, link);

         text_msg(((struct ui_msg_elm*)n->ptr)->level, ((struct ui_msg_elm*)n->ptr)->msg); /* ooh */
         free(((struct ui_msg_elm*)n->ptr)->msg); /* aahh */
         free(n->ptr);
         free(n);

      }

      UI_QUEUE_UNLOCK;
      usleep(500);
   }
}

static int text_queue_msg(int level, const char *msg)
{
   struct generic_qnode *n;
   struct ui_msg_elm *elm;

   n = malloc(sizeof(*n));
   assert(n != NULL);

   elm = malloc(sizeof(*elm));
   assert(elm != NULL);

   elm->level = level;
   elm->msg = strdup(msg);

   n->ptr = elm;

   TAILQ_INSERT_TAIL(&ui_msg_queue, n, link);

   return 0;
}

static int text_msg(int level, const char *msg)
{
   UI_LOCK;
   fprintf(stdout, "[UI MSG]: %d- %s\n", level, msg);
   UI_UNLOCK;
}

static int text_init(void *ptr)
{
   int ret;

   if(initialized) {
      DEBUG_MSG("Text UI already initialized");
      return;
   }

   TAILQ_INIT(&ui_msg_queue);

   ret = pthread_create(&ui_thread, NULL, (void*) &text_ui_dispatcher, NULL);
   assert(ret == 0);

   initialized = 1;
   DEBUG_MSG("Text UI initialized");
   
   return 0;
}

static int text_deinit(void *ptr)
{
   DEBUG_MSG("Text UI deinitialized");
   
   /*
    * Kill the dispatcher thread and cleanup the msg list
    */

   return 0;
}

static struct netmon_ui ui = {
   .name          = "text",
   .ui_init       = text_init,
   .ui_deinit     = text_deinit,
   .ui_msg        = text_msg,
   .ui_queue_msg  = text_queue_msg
};

void ui_text_main()
{
   ui_register(&ui);

   DEBUG_MSG("Text UI registered");
}
