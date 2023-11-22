section .text
start:

xor esi, esi
xor eax, eax
xor ebx, ebx
xor ecx, ecx
xor edx, edx

inc si
mov al, 0x1
mov bl, [esi + array]
int 0x80

section .bss
array resb 0x10000
