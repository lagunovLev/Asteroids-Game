org 0x7C00                     
bits 16       
[map all build/bootloader/zero_sector.map]                  

START_ADDRES equ 0x8000

jmp start
SECTORS_NUMBER_LINK dw SECTORS_NUMBER

start:
; Stack
mov bp, 0x7c00
mov sp, bp

cli
mov ax, 0x00
mov ds, ax
mov es, ax
mov ss, ax
sti

mov bx, MSG_REAL_MODE 
call print
call print_nl

; Reading disk
mov [DRIVE_NUMBER], dl
mov word [PROGRAM_SPACE], START_ADDRES
call diskread

mov bx, MSG_LOAD_KERNEL
call print
call print_nl

call video_init
; Switch to protected mode
cli
lgdt [gdt_descriptor]
mov eax, cr0
or eax, 0x1
mov cr0, eax 
jmp CODESEG:startPM

print: ; bx - адрес строки
    pusha

.start:
    mov al, [bx] 
    cmp al, 0 
    je .done

    mov ah, 0x0e
    int 0x10 

    add bx, 1
    jmp .start

.done:
    popa
    ret

print_nl:
    pusha
    
    mov ah, 0x0e
    mov al, 0x0a
    int 0x10
    mov al, 0x0d 
    int 0x10
    
    popa
    ret

align 8
GDT_Start:
	null_descriptor:
		dd 0
		dd 0
	code_descriptor:
		dw 0xffff  ; Limit
		dw 0x0000  ; Base
		db 0x00
		db 10011010b
		db 11001111b
		db 0x00
	data_descriptor:
		dw 0xffff  ; Limit
		dw 0x0000  ; Base
		db 0x00
		db 10010010b
		db 11001111b
		db 0x00
GDT_End:
	gdt_descriptor:
		dw GDT_End - GDT_Start - 1
		dd GDT_Start

CODESEG equ code_descriptor - GDT_Start
DATASEG equ data_descriptor - GDT_Start

video_init: 
    push bp          
    mov bp, sp
    sub sp, 0
    
    ; Установить режим 
    mov ah, 0 
    mov al, 0x13
    int 10h 

    mov sp, bp
    pop bp
    ret

diskread:
	mov ah, 2
	mov al, [SECTORS_NUMBER]  ; Количество секторов
	mov ch, 0
	mov cl, 2                 ; Номер сектора 
	mov dh, 0                 ; Номер головки 
	mov dl, [DRIVE_NUMBER]    ; Номер диска
	mov bx, [PROGRAM_SPACE]   ; Куда загрузить
	int 0x13

	jc .diskreadfailed
	jmp .end

	.diskreadfailed:
		mov bx, MSG_DISK_READ_FAILED
		call print
		call print_nl
		jmp $

	.end:
	ret

DRIVE_NUMBER db 0
PROGRAM_SPACE dw 0x0000
MSG_DISK_READ_FAILED db "Failed to read disk", 0
SECTORS_NUMBER db 1

MSG_REAL_MODE db "Started in 16-bit Real Mode", 0
MSG_LOAD_KERNEL db "Loaded kernel into memory", 0
MSG_PM db "Protected mode", 0

bits 32
startPM:
    mov ax, DATASEG
    mov ds, ax 
    mov ss, ax 
    mov es, ax 
    mov fs, ax 
    mov gs, ax 

    ; Enable A20
    in al, 0x92
	or al, 0x02
	out 0x92, al

    jmp START_ADDRES
