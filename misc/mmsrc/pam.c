/*
 * The Page Aging Manager 
 */
#include <main.h>

void pam_handler(void *tmp)
{
   int i;
   
   UI_MSG("Page Aging Manager thread up and running");

   for(;;) {
      usleep(T * 1000);

      for(i = 0; i < N; i++) {
         lock_memory();
         memory[i].counter >>= 1;
         unlock_memory();
      }
   }
}

void pam_init()
{
   int ret;
   
   UI_MSG("Creating Page Aging Manager thread");
   ret = pthread_create(&pam_thread, NULL, (void*) &pam_handler, NULL);

   if(ret) {
      UI_MSG("Failed to create thread");
      exit(EXIT_FAILURE);
   }
}
