/*
 * The Access Request Handler
 */
#include <main.h>

int do_operation(char mode, int page, int word, int content)
{
   int frame;
   int data;

   verify_vm_access(page, word);
   lock_pagetable();

   if((int)page_table[page] == (int)-1) {
      //UI_MSG("Page fault: %c %d %d %d\n", mode, page, word, content);
      unlock_pagetable();
      return 0;
   }

   frame = page_table[page];
   unlock_pagetable();

   verify_memory_access(frame, word);

   lock_memory();
   if(mode == 'r') {
      data = memory[frame].content[word];
      memory[frame].counter |= (1 << 31);

      UI_MSG("Read -\t PageIndex: %8d FrameIndex: %8d PageOffset: %8d Content: %8d", page, frame, word, data);
   } else if(mode == 'w') {
      memory[frame].content[word] = content;
      memory[frame].counter |= (1 << 31);

      UI_MSG("Write -\t PageIndex: %8d FrameIndex: %8d PageOffset: %8d Content: %8d", page, frame, word, content);
   } else {
      UI_MSG("Invalid Memory Access Request of mode %c", mode);

      exit(EXIT_FAILURE);
   }
   unlock_memory();

   return 1;
}

void arh_handler(void *tmp)
{
   FILE *fptr;
   struct pf_msg *pmsg;
   int ret;
   int page, word, content;
   char mode;
   int count = 0;

   UI_MSG("Access Request Handler thread up and running");
   
   fptr = fopen(access_file, "r");
   if(fptr == NULL) {
      UI_MSG("Failed to open access request input file");
      exit(EXIT_FAILURE);
   }

   do {
      ret = fscanf(fptr, "%c %d %d %d", &mode, &page, &word, &content);
      if(ret == 4) {
         ret = do_operation(mode, page, word, content);

         if(!ret) {
            pmsg = (struct pf_msg*) xmalloc(sizeof(struct pf_msg));
            pmsg->mode = mode;
            pmsg->page_idx = page;
            pmsg->word_idx = word;
            pmsg->content = content;

            pf_request_queue_lock();
            TAILQ_INSERT_TAIL(&pf_requests, pmsg, link);
            pf_request_queue_unlock();

            count++;
         }
      }
      
      if(count) {
         while(!(pmsg = TAILQ_FIRST(&pf_responses))) usleep(1000);
      
         //pmsg = TAILQ_FIRST(&pf_responses);
         if(pmsg) {
            pf_response_queue_lock();
            TAILQ_REMOVE(&pf_responses, pmsg, link);
            pf_response_queue_unlock();

            ret = do_operation(pmsg->mode, pmsg->page_idx, pmsg->word_idx, pmsg->content);
            if(!ret) {
               UI_BUG("Unexpected failure in Access Requent Handler");
            }

            xfree(pmsg);

            count--;
         }
      }
   } while((!feof(fptr)) || count);

   UI_MSG("Access Request Handler thread exiting");
   pthread_exit(NULL);
}

void arh_init()
{
   int ret;

   UI_MSG("Creating Access Request Handler thread");

   ret = pthread_create(&arh_thread, NULL, (void*) &arh_handler, NULL);

   if(ret) {
      UI_MSG("Failed to create thread");
      exit(EXIT_FAILURE);
   }
}
