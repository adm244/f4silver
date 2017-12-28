/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/

//IMPORTANT(adm244): SCRATCH VERSION JUST TO GET IT UP WORKING

/*
  Compile as x64 code

  StandardCompile:
    // Executes script function
    
    address: 0x004E7C60
    
    // unk0 - some kind of value (seems to be number of arguments)
    // unk1 - ArgsType struct array
    // unk2 - some structure
    // unk3 - script line?
    int8 StandardCompile(int16 argsCount, ArgsType *argsType, Struct_4 *unk2, char *text);
  
  //NOTE(adm244): this is NOT a compile and run!
  CompileAndRun_Prob:
    address: 0x004EB830
    
    // unk0 - ???
    // unk1 - some kind of script object with script text
    // unk2 - some kind of structure that contains script text and reference,
    //        also pointers to unk1 and unk3
    // unk3 - structure with arguments or something ???
    (p.s. unk2 is allocated on stack and is probably a Script object)
    
    int64 CompileAndRun_Prob(void *unk0, TESScript *unk1, Struct_6 *unk2, Struct_4 *unk3);

  CreateTESString:
    address: 0x00044040
    
    void __fastcall CreateTESString(TESString *tesString, char *string, __int64 length);
  
  TESScript::Compile(?):
    address: 0x004E7B10
    
    // globalObject - *(0x05AF9720)
    // scriptObject - scriptObject(?)
    // unk02 - always 0
    // compilerTypeIndex - 1 for "SysWindowCompileAndRun"
    bool __fastcall TESScript::Compile(void *globalObject, TESScript *scriptObject, int32 unk02, int32 compilerTypeIndex);
  
  TESScript::Execute(?):
    address: 0x004E2440
    
    // scriptObject - previously compiled TESScript object
    // unk01 - should be 0
    // unk02 - should be 0
    // unk03 - should be 0
    // unk04 - must be 1
    bool __fastcall TESScript::Execute(TESScript *scriptObject, uint64 unk01, uint64 unk02, uint64 unk03, bool unk04);
  
  TESScript creation address: 0x004E2AC4
  TESScript vtable address: 0x02CADC08
  
  TESScript::Constructor(?):
    address: 0x00151E30
    
    // scriptObject - script object memory
    void __fastcall TESScript::Constructor(TESScript *scriptObject);
*/

/*
  Is current cell interior or exterior check:
    1) Get ObjectReference pointer (rax)
      player is an object reference
      we have an address of player objectreference pointer (0x05AC26F8 for 1_10_26)
    2) Get parent cell ([rax + 0xB8] = rax')
    3) Check cell flags ([rax' + 0x40])
      first bit is interior\exterior (set - interior)
*/

/*
  Script compile and run process:
    1) Allocate memory for Script object
    2) Initialize Script object
      2.1) Set vtable and flags
      2.2) Initialize the rest of a structure with zero
      2.3) Initialize script text at field70 (260 bytes long) with zeros (optional?)
    3) Allocate memory for script text
    4) Initialize memory for script text with zeros
    5) Copy script text from Script object into allocated memory
    6) Set scriptText pointer to point to allocated memory
    
    7) COMPILE: Pass globalObject, Script object, 0 and 1 into sub_004E7B10
    8) RUN: Pass Script object, 0, 0, 0 and 1 into sub_004E2440
*/

#ifndef _F4_FUNCTIONS_
#define _F4_FUNCTIONS_

#include <string.h>

#include "types.h"

#define MAX_SCRIPT_SIZE 16384
#define MAX_SCRIPT_LINE 260
#define VM_OPCODE_LENGTH 4

internal const int DefaultCompiler = 0;
internal const int SysWindowCompileAndRun = 1;
internal const int DialogueCompileAndRun = 2;

//NOTE(adm244): prints out a c-style formated string into the game console
typedef void (__fastcall *_TESConsolePrint)(uint64 consoleObject, char *format, ...);
internal _TESConsolePrint TESConsolePrint;

//NOTE(adm244): displays a message in the top-left corner of a screen
// message - text to be displayed
// unk1 - should be 0
// unk2 - should be 1
// unk3 - should be true (1)
typedef void (__fastcall *_TESDisplayMessage)(char *message, int32 unk1, int32 unk2, bool unk3);
internal _TESDisplayMessage TESDisplayMessage;

typedef void (__fastcall *_TESScript_Constructor)(TESScript *scriptObject);
internal _TESScript_Constructor TESScript_Constructor;

typedef bool (__fastcall *_TESScript_Compile)(void *globalObject, TESScript *scriptObject, int32 unk02, int32 compilerTypeIndex);
internal _TESScript_Compile TESScript_Compile;

typedef bool (__fastcall *_TESScript_Execute)(TESScript *scriptObject, uint64 unk01, uint64 unk02, uint64 unk03, bool unk04);
internal _TESScript_Execute TESScript_Execute;

internal bool ExecuteScriptLine(char *text)
{
  bool result = false;
  
  TESScript scriptObject = {0};
  TESScript_Constructor(&scriptObject);
  
  scriptObject.byte1A = 0x16;
  scriptObject.flags10 |= 0x4000;
  scriptObject.byte1B = 0x41;
  scriptObject.unk1C = 1;
  scriptObject.unk34 = 1;
  
  scriptObject.scriptText = text;
  memcpy(scriptObject.scriptLineText, text, strlen(text));
  
  //TODO(adm244): check bytecodeLength (should be > 0)
  if( TESScript_Compile((void *)GetGlobalScriptObject(), &scriptObject, 0, SysWindowCompileAndRun) ) {
    result = true;
    TESScript_Execute(&scriptObject, 0, 0, 0, 1);
  }
  
  return result;
}

#endif
