;;
;; BUBBLE SORT IN 32BIT x86 ASSEMBLY
;; SHOULD WORK OVER LINUX KERNEL
;;
;; POSITION INDEPENDENT ASSEMBLY COMPONENT
;; YOU CAN MAKE A RELIABLE SHELLCODE OUT OF THIS
;; EVIL HAXORZ CAN USE PWNED BOX FOR DOING BUBBLE SORT
;;
BITS 32

GLOBAL _start

%define FD_STDOUT    1
%define FD_STDIN     0

%define __NR_READ    3
%define __NR_WRITE   4
%define __NR_EXIT    1

_start:
   push ebp
   mov ebp, esp

   sub esp, 4

   call helloStr
   db "Enter the number of elements to sort: ",0x00
   
helloStr:      
   pop ecx
   push ecx
   call sub_PrintStr

   call sub_ReadDword
   mov ecx, eax
   mov dword [ebp - 4], ecx

elementReaderLoop:

   call getAskString
   db "Enter number: ",0x00
   
getAskString:
   pop esi
   push ecx
   
   push esi
   call sub_PrintStr
   pop esi
   pop ecx

   push ecx
   call sub_ReadDword
   pop ecx
   
   push eax

   loop elementReaderLoop

   mov eax, dword [ebp - 4]
   push eax             ; Number of elements to sort
   int3
   call sub_BUBBLES

   int3
   call sub_Exit

   ; sub_BUBBLES(n, A[n])
   ; This procedure will sort A[n] in descending order using the bubble sort algorithm
   ; The _list_ A is modified _in_ _place_ for sorting
sub_BUBBLES:
   push ebp,
   mov ebp, esp

   call startMsg
   db "Bubble sort start..",0x0a,0x00
startMsg:
   pop ecx
   push ecx
   call sub_PrintStr

   mov ebx, [ebp+8]
   dec ebx
   xor ecx, ecx

   add ebp, 12 ; EBP now points to param array base
   
bubbleSortLoop1:
   
   mov edx, ecx
   
bubbleSortLoop2:
   pusha
   mov eax, [ebp + edx * 4]
   mov ebx, [ebp + ecx * 4]
   cmp eax, ebx
   popa
   jl swap
   jmp loop2

swap:
   push dword [ebp + edx * 4]
   push dword [ebp + ecx * 4]
   pop dword [ebp + edx * 4]
   pop dword [ebp + ecx * 4]

loop2:
   inc edx
   cmp edx, ebx
   jl bubbleSortLoop2

   inc ecx
   cmp ecx, ebx
   jl bubbleSortLoop1

   call endMsg
   db "Bubble sort ends..",0x0a,0x00
endMsg:
   pop ecx
   push ecx
   call sub_PrintStr
      
   sub ebp, 12   ; Restore EBP
      
   mov esp, ebp
   pop ebp
   ret

   ; sub_Strlen(str_ptr)
sub_Strlen:
   push ebp
   mov ebp, esp

   mov edi, [ebp+8]
   xor ecx, ecx
   xor al, al
   not ecx
   cld
   repne scasb
   not ecx
   dec ecx 
   
   mov eax, ecx
   
   mov esp, ebp
   pop ebp
   ret
   
   ; sub_PrintStr(str_ptr)
sub_PrintStr:
   push ebp
   mov ebp, esp

   mov eax, [ebp+8]
   push eax
   call sub_Strlen

   mov ebx, FD_STDOUT
   mov ecx, [ebp+8]
   mov edx, eax
   mov eax, __NR_WRITE
   int 0x80

   mov esp, ebp
   pop ebp
   ret

   ; sub_PrintRaw(data_ptr, size)
sub_PrintRaw:
   push ebp,
   mov ebp, esp

   mov ebx, FD_STDOUT
   mov ecx, [ebp+8]
   mov edx, [ebp+12]
   mov eax, __NR_WRITE
   int 0x80

   mov esp, ebp
   pop ebp
   ret

   ; sub_ReadDword() 
sub_ReadDword:
   push ebp
   mov ebp, esp

   sub esp, 16
   
   mov ebx, FD_STDIN
   lea ecx, [ebp - 16]
   mov edx, 16
   mov eax, __NR_READ
   int 0x80
   
   xor ecx, ecx
   lea esi, [ebp - 16]
   
endFinderLoop:
   cmp byte [esi + ecx], 0x0a
   je endFinderLoopEnd
   inc ecx
   jmp endFinderLoop

endFinderLoopEnd:
   mov byte [esi + ecx], 0x00

   dec ecx

   xor eax, eax
   xor edx, edx
atoiLoop:
   xor ebx, ebx
   add bl, byte [esi + edx]
   sub bl, 0x30

   push edx

   push 10
   mul dword [esp]
   add eax, ebx

   add esp, 4

   pop edx
   
   cmp edx, ecx
   je atoiLoopOut
   
   inc edx
   jmp atoiLoop

atoiLoopOut:
  
   mov esp, ebp
   pop ebp
   ret

sub_Exit:
   mov eax, __NR_EXIT
   xor ebx, ebx
   int 0x80
