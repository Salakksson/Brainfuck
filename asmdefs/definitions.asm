start:


; xor si, si
; mov cx, 0xFFFF
;
; 	loop:
; 	mov [si], 0
; 	inc si
; 	cmp si, 0
; 	loop loop
;
xor si, si
xor eax, eax
xor ebx, ebx
xor ecx, ecx
xor edx, edx

; unsigned char opcodes[] =
; {
;     0x66, 0x31, 0xf6, 0x31, 0xc0, 0x31, 0xdb, 0x31,
;     0xc9, 0x31, 0xd2
; };

incptr:					;	0x66, 0x46
	inc si
	ret

decptr:					;	0x66, 0x4e
	dec si
	ret

plus:					;	0x67, 0x8a, 0x84, low, high,
						;	0xfe, 0xc0,
						;	0x67, 0x88, 0x84, low, high
	mov al, [esi + o]
	inc al
	mov [esi + o], al
	ret

minus:					;	0x67, 0x8a, 0x84, low, high,
						;	0xfe, 0xc8
						;	0x67, 0x88, 0x84, low, high
	mov al,	[esi + o]
	dec al
	mov [esi + o], al
	ret

out:					;	0xb0, 0x04,
						;	0xb3, 0x01,
						;	0x67, 0x8a, 0x8c, low, high,
						;	0xcd, 0x80
						;
	mov al, 4			; write syscall
	mov bl, 1 			; stdout
	mov cl, [esi + o]	; character to print

	int 0x80
	ret

in:						;	0xb0, 0x03,
						;	0xb3, 0x00,
						;	0x67, 0x8a, 0x8c, low, high,
						;	0xb2, 0x01,
						;	0xcd, 0x80
						;
	mov al, 3 			; read syscall
	mov bl, 0 			; stdin
	mov cl, [esi + o]	; write to [esi]
	mov dl, 1 			; bytes to read

	int 0x80
	ret

open:
	token:

close:
	cmp [si+o], 0			;	0x66, 0x3b, 0x06, 0x00, 0x00,
	je token			;	0x74, token low, token high


exit:					;	0xb0, 0x01
						;	0x67, 0x8a, 0x1c
	mov al, 1
	mov bl, [si]

array:
	times 0x10000 db 0


