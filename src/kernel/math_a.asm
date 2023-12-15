global sin
global cos

sin:
    push ebp
    mov  ebp, esp

    fld qword [ebp+8]
    fsin 

    pop  ebp
    ret


cos:
    push ebp
    mov  ebp, esp

    fld qword [ebp+8]
    fcos 

    pop  ebp
    ret