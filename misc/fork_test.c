#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void child_func()
{
   printf("I am child with pid: %d\n", getpid());
}

int main(int argc, char **argv)
{
   pid_t children[10];
   int i;
   int ret;
   int st;

   for(i = 0; i < 10; i++) {
      ret = children[i] = fork();

      if(!ret) {
         /* child */
         child_func();
         exit(0);
      }
   }

   /* wait for children */
   for(i = 0; i < 10; i++)
      waitpid(children[i], &st, WNOHANG);
}
