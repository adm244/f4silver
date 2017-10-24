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
  
  #define MAX_SCRIPT_SIZE 16384
  #define MAX_SCRIPT_LINE 260
  
  struct ArgsType {
    char *type; // 8 bytes
    int32 unk01; // 4 bytes
    int8 unk02; // 1 byte
      int8 pad[3]; // 3 bytes
  }; // 16 bytes
  
  StandardCompile:
    // Executes script function
    
    address: 0x004E7C60
    
    // unk0 - some kind of value (seems to be number of arguments)
    // unk1 - ArgsType struct array
    // unk2 - some structure
    // unk3 - script line?
    int8 StandardCompile(int16 argsCount, ArgsType *argsType, Struct_4 *unk2, char *text);
  
  struct Struct_4 {
    int32 unk0;
    char string[512];
    int32 length;
    // ???
  };
  
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
  
  struct TESString {
    char *string;
    uint16 length;
    uint16 length_zero_terminated;
  };
  
  CreateTESString:
    address: 0x00044040
    
    void __fastcall CreateTESString(TESString *tesString, char *string, __int64 length);
  
  ScriptCompile(?):
    address: 0x004E7B10
    
    // globalObject - *(0x05AF9720)
    // scriptObject - scriptObject(?)
    // unk02 - always 0
    // compilerTypeIndex - 1 for "SysWindowCompileAndRun"
    bool __fastcall ScriptCompile(void *globalObject, TESScript *scriptObject, int32 unk02, int32 compilerTypeIndex);
  
  
  TESScript creation address: 0x004E2AC4
  TESScript vtable address: 0x02CADC08
  
  TESScript::Constructor(?):
    address: 0x00151E30
    
    // scriptObject - script object memory
    void __fastcall TESScript::Constructor(TESScript *scriptObject);
*/

/*
  ToggleSky command checks if player is in interior ???
*/

/*
  struct X {
    uint64 ptrCommandName; // 0
    uint64 ptrCommandNameSynonym; // 8
    uint32 opcode; // 12
      uint8 pad[4]; // 16
    uint64 ptrCommandDescription; // 24
    
    uint8 unk4; // 25
      uint8 pad; // 26
    uint16 unk5; // 28
      uint8 pad[4]; // 32
    
    uint64 ptrArgumentType; // 40
    uint64 ptrCommandFunction; // 48
    uint64 ptrStandardCompileFunc; // 56
    uint64 ptrOtherFunc; // 64
      uint8 pad[8]; // 72
  }; // sizeof(X) = 72
*/

#ifndef _F4_FUNCTIONS_
#define _F4_FUNCTIONS_

#include <string.h>

struct TESScript { // struct_a2
  uint64 vtable; // 0x00
  uint64 unk08;
  uint16 flags10;
  uint16 unk12;
  uint16 unk14;
  uint16 unk16;
  uint16 unk18;
  uint8 byte1A;
  uint8 byte1B;
  uint32 unk1C;
  uint32 unk20;
  uint32 unk24;
  uint32 unk28;
  uint32 unk2C;
  uint8 unk30;
  uint8 unk31;
  uint8 unk32;
    uint8 gap33;
  uint32 unk34;
  char *scriptText; // 0x38
  uint64 unk40;
  uint64 unk48;
  uint32 unk50;
  uint32 unk54;
  uint64 unk58;
  uint64 unk60;
  uint64 unk68;
  uint64 unk70;
  uint64 unk78;
  char scriptLineText[260];
}; // 388 bytes (0x184)

//NOTE(adm244): prints out a c-style formated string into the game console
typedef void (__fastcall *_ConsolePrint)(uint64 obj, char *format, ...);
internal _ConsolePrint ConsolePrint;

typedef void (__fastcall *_TESScript_Constructor)(TESScript *scriptObject);
internal _TESScript_Constructor TESScript_Constructor;

typedef bool (__fastcall *_TESScript_Compile)(void *globalObject, TESScript *scriptObject, int32 unk02, int32 compilerTypeIndex);
internal _TESScript_Compile TESScript_Compile;

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
    
    7) Pass globalObject, Script object, 0 and 1 into sub_004E7B10
*/

//FIX(adm244): NOT WORKING!
internal bool ExecuteScriptLine(char *text)
{
  TESScript scriptObject = {0};
  TESScript_Constructor(&scriptObject);
  
  scriptObject.byte1A = 0x16;
  scriptObject.flags10 |= 0x4000;
  scriptObject.byte1B = 0x41;
  scriptObject.unk1C = 1;
  scriptObject.unk34 = 1;
  
  scriptObject.scriptText = text;
  memcpy(scriptObject.scriptLineText, text, strlen(text));
  
  //TODO(adm244): find out a way to execute compiled script!
  return TESScript_Compile((void *)GetGlobalScriptObject(), &scriptObject, 0, 1);
}

#endif
