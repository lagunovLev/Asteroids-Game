global trand

trand:
    push ebp
    mov  ebp, esp

    ; check rdseed 
    mov eax, 7     
    mov ecx, 0     
    cpuid
    shr ebx, 18    
    and ebx, 1     
    jz .fail

    ; generate number
    mov ecx, 100   ; number of retries
.retry:
    rdseed eax
    jc .done      ; carry flag is set on success
    loop .retry
.fail:
    mov eax, 1488  ; Пасхалка
.done:

    pop  ebp
    ret