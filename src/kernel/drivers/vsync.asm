global vsync_wait

vsync_wait:
    push ebp
    mov  ebp, esp

    push eax
    push edx
    mov dx, 0x3DA
.l1:
    in al, dx
    test al, 8
    jnz .l1
.l2:
    in al, dx
    test al, 8
    jz .l2
   
    pop edx
    pop eax

    pop  ebp
    ret