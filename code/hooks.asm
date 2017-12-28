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

extern TESConsoleObjectAddress: qword

extern mainloop_hook_return_address: qword

extern GlobalScriptStateAddress: qword

extern GameLoop: proc

.code
  GameLoop_Hook proc
    push rbx
    push rbp
    push rdi
    push rsi
    push r12
    push r13
    push r14
    push r15
    
    call GameLoop
    
    pop r15
    pop r14
    pop r13
    pop r12
    pop rsi
    pop rdi
    pop rbp
    pop rbx
    
    mov rcx, Unk3ObjectAddress
    call ProcessWindowAddress
    
    jmp [mainloop_hook_return_address]
  GameLoop_Hook endp
  
  GetConsoleObject proc
    mov rax, qword ptr [TESConsoleObjectAddress]
    mov rax, qword ptr [rax]
    
    ret
  GetConsoleObject endp
  
  GetGlobalScriptObject proc
    mov rax, qword ptr [GlobalScriptStateAddress]
    mov rax, qword ptr [rax]
    
    ret
  GetGlobalScriptObject endp
end
