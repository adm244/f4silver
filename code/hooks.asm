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
extern GameDataAddress: qword

extern mainloop_hook_return_address: qword
extern loadgame_start_hook_return_address: qword
extern loadgame_end_hook_return_address: qword

extern GameLoop: proc
extern LoadGameBegin: proc
extern LoadGameEnd: proc

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
  
  ;rcx = 0x058ED480 (1_10_26)
  ;dl = 0x4A (worldspace form type)
  ;
  ;rax = dl + dl*2 (= 0xDE)
  ;  rcx + rax*8 + 0x68 (pointer to array of worldspaces)
  ;  rcx + rax*8 + 0x78 (total worldspace count)
  TES_GetWorldSpaceCount proc
    mov rcx, qword ptr [GameDataAddress]
    
    mov r9, 4Ah
    
    lea rax, qword ptr [r9+r9*2]
    mov rax, qword ptr [rcx+rax*8+78h]
    
    ret
  TES_GetWorldSpaceCount endp
  
  TES_GetWorldSpaceArray proc
    mov rcx, qword ptr [GameDataAddress]
    
    mov r9, 4Ah
    
    lea rax, qword ptr [r9+r9*2]
    mov rax, qword ptr [rcx+rax*8+68h]
    
    ret
  TES_GetWorldSpaceArray endp
end
