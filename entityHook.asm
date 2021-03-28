bits 64
section .text
global entityHook

entityHook:
    movss xmm10,[rdi + 0x54]
    movss xmm11,[rdi + 0x58]
    movss xmm12,[rdi + 0x5c]
    
    ;%define entityObjStart qword [rcx]  ; Arg0 type: uintptr_t
    ;%define entsptr qword [rdx]         ; Arg1 type: classptr
    %define entsptr qword [rcx]         ; Arg0 type: classptr
    
    ;mov entityObjStart, rdi

    push rbp                            ; Stack alignment stuff
    mov rbp, rsp
    ;mov rax, entityObjStart
    mov rax, rdi
    ;mov [entsptr], rax
    mov entsptr, rax

    leave                               ; Short for mov rsp, rbp + pop rbp,
                                        ; returns the stack registers to how they were
                                        ; pre-alignment
    ret