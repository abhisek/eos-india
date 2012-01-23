#include <heaplib.h>

static int fd;
static int hl_text_log_initialized;

int hl_text_log_write(char *str, ...);

void init_hl_text_log()
{
   char *ptr = NULL;
   char *path = NULL;

   debug_msg("initializing text logging");
   ptr = getenv("HEAPLIB_TEXT_LOG_PATH");
   if(ptr) {
      path = strdup(ptr);
   }

   if(!ptr) {
      ptr = (char*) get_current_dir_name();
      path = malloc(strlen(ptr) + 32);
      assert(ptr && path);

      sprintf(path, "%s/%s", ptr, "heaplib_log.txt");
      debug_msg("log file: %s", path);

      free(ptr);
      ptr == NULL;
   }

   fd = open(path, O_RDWR | O_CREAT | O_TRUNC, 0600);
   assert(fd > 0);
   
   hl_text_log_initialized = 1;
   hl_text_log_write("initialized");

   debug_msg("initialized text logging.. fd -> %d", fd);
}

int hl_text_log_write(char *str, ...)
{
   char buf[65535];
   char time_buf[128];
   time_t curr_time;
   char *time_ptr;
   va_list ap;

   assert(hl_text_log_initialized);
   
   curr_time = time(NULL);
   ctime_r(&curr_time, time_buf);
   time_buf[strlen(time_buf) - 1] = '\0'; /* strip trailing \n */

   va_start(ap, str);
   vsnprintf(buf, sizeof(buf) - 1, str, ap);
   buf[sizeof(buf) - 1] = '\0';
   va_end(ap);

   write(fd, time_buf, strlen(time_buf));
   write(fd, " - ", 3);
   write(fd, buf, strlen(buf));
   write(fd, "\n", 1);

   return 0; /* future */
}

int hl_text_log_write_with_backtrace(char *str, ...)
{
   char buf[65535];
   va_list ap;
   void *ptrs[64];
   char **ptr_strs;
   size_t size;
   int i;

   va_start(ap, str);
   vsnprintf(buf, sizeof(buf) - 1, str, ap);
   buf[sizeof(buf) - 1] = '\0';
   va_end(ap);
   
   size = backtrace(ptrs, sizeof(ptrs) / sizeof(void*));
   ptr_strs = (char**) backtrace_symbols(ptrs, size);

   hl_text_log_write("%s", buf);
   hl_text_log_write("** start backtrace **");

   for(i = 0; i < size; i++) {
      hl_text_log_write("\t %s", ptr_strs[i]);
   }

   hl_text_log_write("** end backtrace **");
   return 0;
}

int hl_text_log_close()
{
   assert(hl_text_log_initialized);
   close(fd);

   return 0;
}
