/*
 * The Page Fault Handler
 */
#include <main.h>

int find_pgt_index()
{
   int frame_idx;
   int low_count;
   int i;

   frame_idx = 0;
   low_count = memory[frame_idx].counter;

   for(i = 1; i < N; i++) {
      if((unsigned int)memory[i].counter < (unsigned int)low_count) {
         frame_idx = i;
         low_count = memory[i].counter;
      }
   }

   return frame_idx;
}

int find_vm_index(int frame_idx)
{
   int frame = frame_idx;
   int i;
   
   for(i = 0; i < M; i++) {
      if(page_table[i] == frame)
         return i;
   }

   UI_BUG("Page Table not consistant");
   exit(EXIT_FAILURE);

   return -1;
}

void swap_out(int page_idx, int frame_idx)
{
   verify_vm_access(page_idx, 0);
   verify_memory_access(frame_idx, 0);

   memcpy(vm[page_idx], memory[frame_idx].content, P * sizeof(int));
}

void swap_in(int page_idx, int frame_idx)
{
   verify_vm_access(page_idx, 0);
   verify_memory_access(frame_idx, 0);

   memcpy(memory[frame_idx].content, vm[page_idx], P * sizeof(int));
}

void do_pagefault(struct pf_msg *pmsg)
{
   int frame;
   int page;

   verify_vm_access(pmsg->page_idx, pmsg->word_idx);

   frame = find_pgt_index();
   page = find_vm_index(frame);

   lock_memory();
   lock_vm();
   lock_pagetable();

   //printf("Swapping out frame %d\n", frame);

   swap_out(page, frame);
   usleep(F);

   //printf("Swapping in page %d\n", pmsg->page_idx);
   swap_in(pmsg->page_idx, frame);
   usleep(F);

   page_table[page] = -1;
   page_table[pmsg->page_idx] = frame;

   unlock_memory();
   unlock_vm();
   unlock_pagetable();
}

void pfh_handler(void *tmp)
{
   struct pf_msg *pmsg;

   UI_MSG("Page Fault Handler thread up and running");

   for(;;) {
      pmsg = TAILQ_FIRST(&pf_requests);
      if(pmsg) {
         pf_request_queue_lock();
         TAILQ_REMOVE(&pf_requests, pmsg, link);
         pf_request_queue_unlock();

         do_pagefault(pmsg);
         
         pf_response_queue_lock();
         TAILQ_INSERT_TAIL(&pf_responses, pmsg, link);
         pf_response_queue_unlock();
      }

      usleep(1000);
   }
}

void pfh_init()
{
   int ret;

   UI_MSG("Creating Page Fault Handler thread");
   ret = pthread_create(&pfh_thread, NULL, (void*) &pfh_handler, NULL);

   if(ret) {
      UI_MSG("Failed to create thread");
      exit(EXIT_FAILURE);
   }
}
