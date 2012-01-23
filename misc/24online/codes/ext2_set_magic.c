#define _GNU_SOURCE
#define _LARGEFILE64_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>

int main(int argc, char **argv)
{
   int fd;
   unsigned long p1;
   unsigned long p2;
   uint16_t magic;

   if(argc < 2) {
      printf("Usage:\n");
      printf("%s [File] [Magic]\n", argv[0]);
      exit(EXIT_FAILURE);
   }


   fd = open(argv[1], O_RDWR | O_LARGEFILE);
   assert(fd != -1);

   p1 = (unsigned long) mmap(NULL, 0x450, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
   assert(((void*)p1) != MAP_FAILED);

   p2 = p1 + 0x430 + 8; /* offset for EXT2 Magic Number in SB */

   printf("Current Magic: 0x%04x\n", *((uint16_t*)p2));
   
   if(argc == 3) {
      magic = strtol(argv[2], NULL, 0);

      printf("Setting Magic to: 0x%04x\n", magic);

      *((uint16_t*)p2) = magic;
      msync((void*)p2, 2, MS_SYNC);
   }

   munmap((void*)p1, 0x450);
   close(fd);
 
}
