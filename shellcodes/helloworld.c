#include <stdio.h>

/* Keeping code in .bss segment */
char code[] =
"\xe8\x0d\x00\x00\x00\x48\x65\x6c\x6c\x6f\x20\x57\x6f\x72\x6c\x64"
"\x0a\x00\x59\xba\x0c\x00\x00\x00\xbb\x01\x00\x00\x00\xb8\x04\x00"
"\x00\x00\xcd\x80\x31\xc0\x31\xdb\x40\xcd\x80";

int main(int argc, char **argv)
{
  int (*funct)();
  char temp;
  
  funct = (int (*)()) code;

  printf("code is at 0x%x\n",&code);
  printf("sizeof code is 0x%x bytes\n",sizeof(code));

  printf("Checking read/write on code's segment\n");
  temp = code[10];   /* Read works */
  code[10] = 0xff;   /* Write works */
  code[10] = temp;   /* Restore the byte */
  printf("All works\n");
  
  printf("Executing code\n\n");
  (int)(*funct)();
}
