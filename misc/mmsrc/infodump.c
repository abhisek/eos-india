/*
 * This implements the Information Dumper UI
 */
#include <main.h>

void dump_page_table()
{
   int i;

   UI_MSG("-- Start Page Table Dump --");

   for(i = 0; i < M; i++)
      UI_PRINT("\tPage Index: %8d \t Memory Index: %8d\n", i, page_table[i]);

   UI_MSG("-- End Page Table Dump --");
}

void dump_physical_memory()
{
   int i, k;

   UI_MSG("-- Start Physical Memory Dump --");

   for(i = 0; i < N; i++) {
      UI_PRINT("\tFrame Index: %8d Counter: %8u", i, memory[i].counter);

      for(k = 0; k < P; k++) {
         if(!(k % 20)) {
            UI_PRINT("\n");
            UI_PRINT("\t");
         }
         
         UI_PRINT("%d ", memory[i].content[k]);
      }

      UI_PRINT("\n");
   }

   UI_MSG("-- End Physical Memory Dump --");
}

void dump_virtual_memory()
{
   int i, k;

   UI_MSG("-- Start Virtual Memory Dump --");

   for(i = 0; i < M; i++) {
      UI_PRINT("\tPage Index: %8d", i);

      for(k = 0; k < P; k++) {
         if(!(k % 20)) {
            UI_PRINT("\n");
            UI_PRINT("\t");
         }

         UI_PRINT("%d ", vm[i][k]);
      }

      UI_PRINT("\n");
   }

   UI_MSG("-- End Virtual Memory Dump --");
}

void dump_pfqueue()
{
   struct pf_msg *pmsg;

   UI_MSG("-- Start PageFault Queue Dump --");

   UI_PRINT("\t-- Dumping PageFault request queue --\n");
   if(TAILQ_FIRST(&pf_requests) == NULL) {
      UI_PRINT("\t\t ** Empty **\n");
   }
   else {
      TAILQ_FOREACH(pmsg, &pf_requests, link) {
         UI_PRINT("\n");
         UI_PRINT("\t\tMode: %c\n", pmsg->mode);
         UI_PRINT("\t\tPage: %d\n", pmsg->page_idx);
         UI_PRINT("\t\tWord: %d\n", pmsg->word_idx);
         UI_PRINT("\t\tData: %d\n", pmsg->content);
      }
   }
   UI_PRINT("\n");

   UI_PRINT("\t-- Dumping PageFault response queue --\n");
   if(TAILQ_FIRST(&pf_responses) == NULL) {
      UI_PRINT("\t\t ** Empty **\n");
   }
   else {
      TAILQ_FOREACH(pmsg, &pf_responses, link) {
         UI_PRINT("\n");
         UI_PRINT("\t\tMode: %c\n", pmsg->mode);
         UI_PRINT("\t\tPage: %d\n", pmsg->page_idx);
         UI_PRINT("\t\tWord: %d\n", pmsg->word_idx);
         UI_PRINT("\t\tData: %d\n", pmsg->content);
      }
   }
   UI_PRINT("\n");

   UI_MSG("-- End PageFault Queue Dump --");
}

void infodump_ui()
{
   fd_set fds;
   FILE *infile;
   char command[32];
   int sl;
   int ret;
   
   infile = fdopen(0, "r");
   if(infile == NULL) {
      UI_MSG("Failed to open stdout for interaction");
      exit(EXIT_FAILURE);
   }

   for(;;) {
      memset(command, 0, sizeof(command));
      sl = 0;

      FD_ZERO(&fds);
      FD_SET(0, &fds);
      
      ret = select(0 + 1, &fds, NULL, NULL, NULL);
      if(ret < 0) {
         UI_MSG("Failed to poll input file");
         exit(EXIT_FAILURE);
      }

      if(FD_ISSET(0, &fds)) {
         fscanf(infile, "%7s %d", command, &sl);

         UI_MSG("command: %s sleep: %d", command, sl);
         
         if(!strncmp(command, "memory-", strlen("memory-"))) {
            dump_physical_memory();
         }
         else if(!strncmp(command, "process", strlen("process"))) {
            dump_virtual_memory();
         }
         else if(!strncmp(command, "pagetbl", strlen("pagetbl"))) {
            dump_page_table();
         }
         else if(!strncmp(command, "pfaultq", strlen("pfaultq"))) {
            dump_pfqueue();
         }
         else {
            UI_MSG("Invalid command");
            continue;
         }
         
         usleep(sl * 1000);
      }
   }
}

void infodump_init()
{
   infodump_ui(); 
}
