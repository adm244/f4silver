;This is free and unencumbered software released into the public domain.
;
;Anyone is free to copy, modify, publish, use, compile, sell, or
;distribute this software, either in source code form or as a compiled
;binary, for any purpose, commercial or non-commercial, and by any
;means.
;
;In jurisdictions that recognize copyright laws, the author or authors
;of this software dedicate any and all copyright interest in the
;software to the public domain. We make this dedication for the benefit
;of the public at large and to the detriment of our heirs and
;successors. We intend this dedication to be an overt act of
;relinquishment in perpetuity of all present and future rights to this
;software under copyright law.
;
;THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
;EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
;MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
;IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
;OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
;ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
;OTHER DEALINGS IN THE SOFTWARE.

;IMPORTANT(adm244): SCRATCH VERSION JUST TO GET IT UP WORKING

extern ProcessWindowAddress: qword

extern Unk3ObjectAddress: qword

extern mainloop_hook_return_address: qword
extern loadgame_start_hook_return_address: qword
extern loadgame_end_hook_return_address: qword
extern prepare_hacking_hook_addr: qword
extern quit_hacking_hook_addr: qword
extern activate_vats_addr: qword

extern GameLoop: proc
extern LoadGameBegin: proc
extern LoadGameEnd: proc
extern HackingPrepare: proc
extern HackingQuit: proc
extern VATSActivate: proc

pushregs MACRO
  push rbx
  push rbp
  push rdi
  push rsi
  push r12
  push r13
  push r14
  push r15
ENDM

popregs MACRO
  pop r15
  pop r14
  pop r13
  pop r12
  pop rsi
  pop rdi
  pop rbp
  pop rbx
ENDM

.code
  GameLoop_Hook proc
    pushregs
    call GameLoop
    popregs
    
    mov rcx, Unk3ObjectAddress
    call ProcessWindowAddress
    
    jmp [mainloop_hook_return_address]
  GameLoop_Hook endp
  
  LoadGameBegin_Hook proc
    pushregs
    
    push rcx
    mov rcx, rdx
    sub rsp, 8h
    call LoadGameBegin
    add rsp, 8h
    pop rcx
    
    popregs
    
    mov dword ptr [rsp+20h], r9d
    mov dword ptr [rsp+18h], r8d
    mov qword ptr [rsp+10h], rdx
    
    jmp [loadgame_start_hook_return_address]
  LoadGameBegin_Hook endp
  
  LoadGameEnd_Hook proc
    pushregs
    call LoadGameEnd
    popregs
    
    add rsp, 630h
    pop r15
    pop r14
    pop r13
    
    jmp [loadgame_end_hook_return_address]
  LoadGameEnd_Hook endp
  
  HackingPrepare_Hook proc
    pushregs
    call HackingPrepare
    popregs
    
    mov rdx, qword ptr [rdi+30h]
    mov rcx, qword ptr [rdi+20h]
    and eax, 8Fh
    
    mov rax, prepare_hacking_hook_addr
    add rax, 13
    jmp rax
  HackingPrepare_Hook endp
  
  HackingQuit_Hook proc
    pushregs
    push rcx
    call HackingQuit
    pop rcx
    popregs
    
    lea rdx, qword ptr [rbp-8h]
    add rcx, 120h
    mov qword ptr [rbp], r14
    
    mov rax, quit_hacking_hook_addr
    add rax, 15
    jmp rax
  HackingQuit_Hook endp
  
  VATSActivate_Hook proc
    pushregs
    call VATSActivate
    popregs
    
    cmp ax, 0
    jnz proceed
    ret
    
  proceed:
    mov byte ptr [rsp+8h], cl
    mov r11, rsp
    push rbp
    push rbx
    push rdi
    push r12
    
    mov rax, activate_vats_addr
    add rax, 12
    jmp rax
  VATSActivate_Hook endp
end
