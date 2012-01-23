/*
 * The Memory Management Unit Simulation
 */

#include <main.h>

void clean_exit(int signo)
{
   UI_MSG("Exiting on signal %d", signo);

   exit(EXIT_SUCCESS);
}

void init_signals()
{
   signal(SIGTERM, clean_exit);
   signal(SIGINT, clean_exit);
}

void init_simulation()
{
   FILE *fptr;
   int i, k;
   int ret;

   UI_MSG("Initializing the simulator");

   fptr = fopen(memfile, "r");
   if(fptr == NULL) {
      UI_MSG("Failed to open memory input file");
      exit(EXIT_FAILURE);
   }

   /*
    * Read input
    */
   ret = fscanf(fptr, "%d %d %d %d %d %d", &N, &M, &P, &F, &T, &R);
   if(ret < 6) {
      UI_MSG("Unexpected/Insufficient input content");
      exit(EXIT_FAILURE);
   }
   
   /*
    * Initialize physical memory
    */
   memory = (struct memory_frame*) xmalloc(N * sizeof(struct memory_frame));
   for(i = 0; i < N; i++) {
      memory[i].content = (int*) xmalloc(P * sizeof(int));
   }

   /*
    * Initialize Process address space
    */
   vm = (int**) xmalloc(M * sizeof(void*));
   for(i = 0; i < M; i++) {
      vm[i] = (int*) xmalloc(P * sizeof(int));
   }

   /*
    * Fill process address space from input
    */
   for(i = 0; i < M; i++) {
      for(k = 0; k < P; k++) {
         if(feof(fptr)) {
            UI_MSG("Unexpected/Insufficient input content");
            exit(EXIT_FAILURE);
         }

         fscanf(fptr, "%d", &vm[i][k]);
      }
   }

   /*
    * Copy the initial working set
    */
   for(i = 0; i < N; i++) {
      memory[i].counter = 0;

      for(k = 0; k < P; k++) {
         memory[i].content[k] = vm[i][k];
      }
   }

   /*
    * Initialize Page Table
    */
   page_table = (int*) xmalloc(M * sizeof(int));

   for(i = 0; i < N; i++)
      page_table[i] = i;

   for(i = N; i < M; i++)
      page_table[i] = -1;

   /*
    * Initialize Request/Response queues
    */
   TAILQ_INIT(&pf_requests);
   TAILQ_INIT(&pf_responses);
}

int main(int argc, char **argv)
{
   init_signals();
   init_simulation();
   pam_init();
   pfh_init();
   arh_init();
   
   infodump_init();

   //dump_page_table();
   //dump_physical_memory();
   //dump_virtual_memory();

   asm("int3;");

   return 0;
}
