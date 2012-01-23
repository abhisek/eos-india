#include <stdio.h>

static char voodoo_string[1024];

void voodoo()
{
   printf("%4$s\n");
}

main()
{
   char *str = (char*) voodoo_string;
   
   printf("Enter string: ");
   fgets(str, sizeof(voodoo_string) - 1, stdin);

   voodoo();
}
