#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <linux/user.h>
#include <sys/mman.h>
#include <elf.h>

#define get_reg_value(pid,reg,var)     do { \
                                          struct user_regs_struct _regs;  \
                                          assert(ptrace(PTRACE_GETREGS, pid, NULL, &_regs) != -1); \
                                          if(!strncmp(reg, "eip", 3))         \
                                             var = _regs.eip;                 \
                                          else if(!strncmp(reg, "eax", 3))    \
                                             var = _regs.eax;                 \
                                          else if(!strncmp(reg, "ebx", 3))    \
                                             var = _regs.ebx;                 \
                                          else if(!strncmp(reg, "ecx", 3))    \
                                             var = _regs.ecx;                 \
                                       } while(0)

struct proc_state {
   unsigned int state_id;
   struct user_regs_struct regs;
};

static void record_proc_state(pid_t pid)
{
   struct proc_state *s;
   int ret;

   s = malloc(sizeof(*s));
   assert(s != NULL);
   
   ret = ptrace(PTRACE_GETREGS, pid, NULL, &(s->regs));
   assert(ret != -1);

   s->state_id = s->regs.eip; /* for now */

   //printf("EIP: %lx\n", s->regs.eip);
}

static unsigned long get_elf_entry_point(const char *elfdata)
{
   Elf32_Ehdr ehdr;
   int ret;
   
   ret = memcmp(elfdata, "\x7f\x45\x4c\x46", 4);
   assert(ret == 0);

   memcpy((char*)&ehdr, elfdata, sizeof(ehdr));
   return (unsigned long) ehdr.e_entry;
}

static void start_proc(char **args)
{
   int ret;

   ret = ptrace(PTRACE_TRACEME, 0, NULL, NULL);
   assert(ret != -1);

   execv(args[0], args);
}

int main(int argc, char **argv)
{
   char **args;
   int i;
   pid_t pid;
   int status;
   unsigned int data;
   unsigned int addr;
   unsigned int state_count;
   int ret;
   int mapfd;
   struct stat st;
   unsigned char *elfptr;
   unsigned long elfep;
   int trace_start;
   unsigned long eip;

   if(argc < 2) {
      puts("Usage:");
      puts("./proctrace bin args");
      exit(EXIT_FAILURE);
   }

   /*
    * setup arguments for child
    */
   args = malloc(argc * sizeof(char*));
   assert(args != NULL);

   i = 1;
   while(i < argc) {
      printf("args[%d] = %s\n", i - 1, argv[i]);
      args[i - 1] = strdup(argv[i]);
      i++;
   }

   /*
    * map the elf
    */
   mapfd = open(args[0], O_RDONLY);
   assert(mapfd != -1);

   fstat(mapfd, &st);
   elfptr = (unsigned char*) mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, mapfd, 0);
   assert((unsigned long) elfptr != (unsigned long) MAP_FAILED);
   
   elfep = get_elf_entry_point(elfptr);

   printf("Mapped %ld bytes at 0x%lx with ep at 0x%lx\n", st.st_size, (unsigned long)elfptr, elfep);
   /*
    * fork and exec
    */
   pid = fork();
   switch(pid) {
      case -1:
         perror("fork");
         exit(EXIT_FAILURE);
      case 0:
         start_proc(args);
         printf("*** This shouldn't have reached *** \n");
      default:
         printf("Child pid: %d up and running\n", pid);
   }
   
   sleep(1); /* settle down */
   state_count = 0;
   trace_start = 0;
   /*
    * enter control loop
    */
   for(;;) {
      ret = wait(&status);
      assert(ret != -1);
      
      if(WIFEXITED(status)) {
         printf("Child %d exited\n", pid);
         break;
      }

      /* start recording only when ep reached */
      get_reg_value(pid, "eip", eip);
      if(eip == elfep) {
         printf("Starting trace from ep 0x%08lx\n", elfep);
         trace_start = 1;
      }
      
      if(trace_start) {
         record_proc_state(pid);
         state_count++; /* need polynomial representation */
      }
      
      ret = ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL);
      assert(ret != -1);
   }
   
   printf("Total states recorded: %ld\n", state_count);
   return 0;
}
