format ELF executable 2
entry _start
segment readable executable


_start:

xor esi, esi
xor eax, eax
xor ebx, ebx
xor ecx, ecx
xor edx, edx

inc si
mov al, 0x1
mov bl, [esi + array]
int 0x80

segment readable writable

array rb 0x10000
