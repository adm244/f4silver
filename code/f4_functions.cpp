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
  
  Fallout 4 RE:
  (all addresses are relative to base)
    SaveGame: 0x00CDC4D0
    LoadGame: 0x00CDC9D0
    DeleteSaveGame: 0x00CEAF30
    
    GameDataReady: 0x0082F8D0
    
    FirstConsoleCommand: 0x03724DC0
    FirstObScriptCommand: 0x03714DD0
    
    Console Manager: 0x058FEEB0
    Console Pointer: 0x05AF97A8
    
    Load Game Event: 0x00442D20
    Init Script Event: 0x00442BE0
    Death Event: 0x004423C0
    Combat Event: 0x00441F60
    
    IsMenuOpen: 0x020406C0
    
    GameVM: 0x058F1708
  
  Print to console:
    this: 0x0586C4C0
    method: 0x012490E0
  
  CompileAndRun(?):
    address: 0x004EAE20
  
  //IMPORTANT(adm244): new are from here
  
  #define MAX_SCRIPT_SIZE 16384
  
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
  
  struct Struct_a2 {
    int64 vtable; // 0x00
    int64 unk08;
    int16 unk10;
    int16 unk12;
    int16 unk14;
    int16 unk16;
    int16 unk18;
    int16 scriptTextLength; // 0x1A
    int64 unk1C;
    int64 unk24;
    int32 unk2C;
    int8 unk30;
    int8 unk31;
    int8 unk32;
    int8 gap33;
    int32 unk34;
    char *scriptText; // 0x38
    int8 gap40[20];
    int32 unk54;
    int64 unk58;
    int64 unk60;
    int64 unk68;
    int64 unk70;
    int64 unk78;
  }; // 128 bytes (0x80)
  
  struct Struct_4 {
    int32 unk0;
    char string[512];
    int32 length;
    // ???
  };
  
  //NOTE(adm244): this is NOT a compile and run!
  CompileAndRun_Prob:
    // This is most likely a proper CompileAndRun function
    
    address: 0x004EB830
    
    // unk0 - ???
    // unk1 - some kind of script object with script text
    // unk2 - some kind of structure that contains script text and reference,
    //        also pointers to unk1 and unk3
    // unk3 - structure with arguments or something ???
    (p.s. unk2 is allocated on stack and is probably a Script object)
    
    int64 CompileAndRun_Prob(void *unk0, Struct_a2 *unk1, Struct_6 *unk2, Struct_4 *unk3);
  
  struct TESString {
    char *string;
    uint16 length;
    uint16 length_zero_terminated;
  };
  
  CreateTESString:
    address: 0x00044040
    
    void __fastcall CreateTESString(TESString *tesString, char *string, __int64 length);
  
  CompileAndRun(?):
    address: 0x004E7B10
    
    // globalObject - 0x05AF9720
    // scriptObject - scriptObject(?)
    // unk02 - always 0
    // compilerTypeIndex - 1 for "SysWindowCompileAndRun"
    bool __fastcall sub_004E7B10(void *globalObject, struct_a2 *scriptObject, int32 unk02, int32 compilerTypeIndex);
  
  
  struct_a2 creation address: 0x004E2AC4
  struct_a2 vtable address: 0x02CADC08
  
  Struct_a2_ctor(?):
    address: 0x00151E30
    
    // scriptObject - script object memory
    struct_a2 * __fastcall Struct_a2::ctor(struct_a2 *scriptObject);
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

//NOTE(adm244): prints out a c-style formated string into the game console
typedef void (__fastcall *_ConsolePrint)(uint64 obj, char *format, ...);
internal _ConsolePrint ConsolePrint;

#endif
