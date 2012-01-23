BITS 32

GLOBAL _start

_start:
   
   call getString
   db "Hello World",0x0a,0x00

getString:
   pop ecx
   mov edx, 12
   mov ebx, 1
   mov eax, 4
   int 0x80

   xor eax,eax
   xor ebx, ebx
   inc eax
   int 0x80
