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

;extern baseAddress: qword
;extern CompileAndRunAddress: qword
;extern ScriptObjectAddress: qword
;extern Unk1ObjectAddress: qword

extern ProcessWindowAddress: qword
extern Unk3ObjectAddress: qword

;extern ConsolePrintAddress: qword
extern Unk2ObjectAddress: qword
;extern mainloop_hook_patch_address: qword
extern mainloop_hook_return_address: qword

extern GameLoop: proc

.data
  unkval01 dd 1

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
    
    ;mov rax, qword ptr ds:[01455FF648h]
    ;mov rbx, qword ptr ds:[rax+030h]
    mov rcx, Unk3ObjectAddress
    call ProcessWindowAddress
    
    jmp [mainloop_hook_return_address]
  GameLoop_Hook endp
  
  GetConsoleObject proc
    mov rax, qword ptr [Unk2ObjectAddress]
    mov rax, qword ptr [rax]
    
    ret
  GetConsoleObject endp
  
  ;NOTE(adm244): compiles and runs passed in rcx script
  ;CompileAndRun proc
  ;  ; rcx: this, rdx: unk1, r8: scriptText, r9: unk3
  ;  mov r8, rcx
  ;  ;lea r9, unkval01
  ;  
  ;  mov rax, qword ptr [ScriptObjectAddress]
  ;  mov rcx, qword ptr [rax]
  ;  
  ;  ;this:
  ;  ; rax = [0x05A669B0]
  ;  ; r15 = [rax + 0xB8]
  ;  ; [r15 - 0x38]
  ;  
  ;  mov rax, qword ptr [Unk1ObjectAddress]
  ;  mov rdx, qword ptr [rax]
  ;  ;mov rax, qword ptr [rax + 0B8h]
  ;  ;mov rdx, qword ptr [rax - 038h]
  ;  
  ;  mov rax, CompileAndRunAddress
  ;  call rax
  ;  
  ;  ret
  ;CompileAndRun endp
  
  ;NOTE(adm244): prints string passed in rcx out to game console
  ;ConsolePrint proc
  ;  mov rdx, rcx
  ;  
  ;  mov rax, qword ptr [Unk2ObjectAddress]
  ;  mov rcx, qword ptr [rax]
  ;  
  ;  mov rax, ConsolePrintAddress
  ;  call rax
  ;  
  ;  ret
  ;ConsolePrint endp
end
