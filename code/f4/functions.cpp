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

/*
  BGSSaveLoadGame:
  
    // BGSSaveLoadGame:
    //  0x05A916D0 (1_10_26)
    //  0x???????? (1_10_40)
    // filename - save file name
    // unk2 - ???
    // unk3 - buffer?
    // address:
    //  0x00CDCA40 (1_10_26)
    //  0x00CDE390 (1_10_40)
    bool LoadGame(int64 BGSSaveLoadGame, char *filename, int32 unk2, int64 unk3);
*/

/*
  Global form list(?)
    
*/

#ifndef _F4_FUNCTIONS_
#define _F4_FUNCTIONS_

#include <string.h>

#define MAX_SCRIPT_SIZE 16384
#define MAX_SCRIPT_LINE 260
#define VM_OPCODE_LENGTH 4

internal const int DefaultCompiler = 0;
internal const int SysWindowCompileAndRun = 1;
internal const int DialogueCompileAndRun = 2;

typedef void (__fastcall *_TESForm_Constructor)(TESForm *tesForm);
internal _TESForm_Constructor TESForm_Constructor;

//FIX(adm244): GlobalScript(-object) is a temporary name until we figure out the real one
typedef bool (__fastcall *_TESGlobalScript_Compile)
(void *globalObject, TESScript *scriptObject, int32 unk02, int32 compilerTypeIndex);
internal _TESGlobalScript_Compile TESGlobalScript_Compile;

// ------ TESScript ------
//NOTE(adm244): TESScript object ctor
typedef TESScript * (__fastcall *_TESScript_Constructor)
(TESScript *tesScript);

typedef bool (__fastcall *_TESScript_Execute)
(TESScript *tesScript, uint64 unk01, uint64 unk02, uint64 unk03, bool unk04);

typedef void (__fastcall *_TESScript_MarkAsTemporary)
(TESScript *tesScript);

typedef bool (__fastcall *_TESScript_Compile)
(TESScript *tesScript, void *globalScript, int compilerTypeIndex, uint64 unk03);

typedef bool (__fastcall *_TESScript_CompileAndRun)
(TESScript *tesScript, void *globalScript, int compilerTypeIndex, void *unk03);

typedef void (__fastcall *_TESScript_SetText)
(TESScript *tesScript, char *scriptText);

typedef void (__fastcall *_TESScript_Destructor)
(TESScript *tesStript);

internal _TESScript_Constructor TESScript_Constructor;
internal _TESScript_Execute TESScript_Execute;
internal _TESScript_MarkAsTemporary TESScript_MarkAsTemporary;
internal _TESScript_Compile TESScript_Compile;
internal _TESScript_CompileAndRun TESScript_CompileAndRun;
internal _TESScript_SetText TESScript_SetText;
internal _TESScript_Destructor TESScript_Destructor;
// ------ #TESScript ------

// ------ TESPlayer ------
//NOTE(adm244): attempts to move player into tesCell or cell with cellName (performs searching)
// tesPlayer - TESPlayer object to move
// cellName - cell name to move to (if not null then searches for a cell)
// tesCell - TESCell object to move to (null if cellName is specified)
typedef bool (__fastcall *_TESPlayer_MoveToCell)
(TESPlayer *tesPlayer, char *cellName, TESCell *tesCell);

internal _TESPlayer_MoveToCell TESPlayer_MoveToCell;
// ------ #TESPlayer ------

// ------ TESWorldSpace ------
typedef TESCell * (__fastcall _TESWorldSpace_FindExteriorCellByCoordinates)
(TESWorldSpace *tesWorldSpace, unsigned int cellX, unsigned int cellY);

internal _TESWorldSpace_FindExteriorCellByCoordinates TESWorldSpace_FindExteriorCellByCoordinates;
// ------ #TESWorldSpace ------

// ------ Utils ------
//NOTE(adm244): prints out a c-style formated string into the game console
typedef void (__fastcall *_TESConsolePrint)
(uint64 consoleObject, char *format, ...);

//NOTE(adm244): displays a message in the top-left corner of a screen
// message - text to be displayed
// unk1 - should be 0
// unk2 - should be 1
// unk3 - should be true (1)
typedef void (__fastcall *_TESDisplayMessage)
(char *message, int32 unk1, int32 unk2, bool unk3);

//NOTE(adm244): searches for an interior cell with specified cell name
// cellName - an interior cell name to be found
typedef TESCell * (__fastcall *_TESFindInteriorCellByName)
(char *cellName);

typedef TESWorldSpace * (__fastcall _TESFindCellWorldSpaceByName)
(void *unk0, char *cellName, unsigned int *cellX, unsigned int *cellY);

internal _TESConsolePrint TESConsolePrint;
internal _TESDisplayMessage TESDisplayMessage;
internal _TESFindInteriorCellByName TESFindInteriorCellByName;
internal _TESFindCellWorldSpaceByName TESFindCellWorldSpaceByName;
// ------ #Utils ------

// ------ Addresses ------
extern "C" {
  uint64 mainloop_hook_patch_address;
  uint64 mainloop_hook_return_address;
  
  uint64 loadgame_start_hook_patch_address;
  uint64 loadgame_start_hook_return_address;
  uint64 loadgame_end_hook_patch_address;
  uint64 loadgame_end_hook_return_address;

  uint64 ProcessWindowAddress;
  uint64 Unk3ObjectAddress;
  
  uint64 TESConsolePrintAddress;
  uint64 TESConsoleObjectAddress;
  
  uint64 TESFormConstructorAddress;
  
  uint64 TESGlobalScriptCompileAddress;
  uint64 GlobalScriptStateAddress;
  
  uint64 TESScriptConstructorAddress;
  uint64 TESScriptDestructorAddress;
  uint64 TESScriptMarkAsTemporaryAddress;
  uint64 TESScriptCompileAddress;
  uint64 TESScriptExecuteAddress;
  uint64 TESScriptCompileAndRunAddress;
  uint64 TESScriptSetTextAddress;
  
  uint64 TESPlayerMoveToCellAddress;
  
  uint64 TESWorldSpaceFindExteriorCellByCoordinatesAddress;
  
  uint64 TESFindInteriorCellByNameAddress;
  uint64 TESFindCellWorldSpaceByNameAddress;
  uint64 TESDisplayMessageAddress;
  
  uint64 PlayerReferenceAddress;
}

internal void DefineAddresses()
{
  if( F4_VERSION == F4_VERSION_1_10_40 ) {
    mainloop_hook_patch_address = 0x00D36707;
    mainloop_hook_return_address = 0x00D36713;
    
    loadgame_start_hook_patch_address = 0x00CED090;
    loadgame_start_hook_return_address = 0x00CED09F;
    loadgame_end_hook_patch_address = 0x00CED898;
    loadgame_end_hook_return_address = 0x00CED8A5;

    ProcessWindowAddress = 0x00D384E0;
    Unk3ObjectAddress = 0x05ADE288;

    TESConsolePrintAddress = 0x01262830;
    TESConsoleObjectAddress = 0x0591AB30;

    TESFormConstructorAddress = 0x00151E30;

    TESGlobalScriptCompileAddress = 0x004E7B30;
    GlobalScriptStateAddress = 0x05B15420;

    TESScriptConstructorAddress = 0x004E14F0;
    TESScriptDestructorAddress = 0x004E1570;
    TESScriptMarkAsTemporaryAddress = 0x00153F00;
    TESScriptCompileAddress = 0x004E28E0;
    TESScriptExecuteAddress = 0x004E2460;
    TESScriptCompileAndRunAddress = 0x004E2830;
    TESScriptSetTextAddress = 0x004E20D0;
    
    TESPlayerMoveToCellAddress = 0x00E9A330;
    
    TESWorldSpaceFindExteriorCellByCoordinatesAddress = 0x004923E0;
    
    TESFindInteriorCellByNameAddress = 0x00152EB0;
    TESFindCellWorldSpaceByNameAddress = 0x00112FA0;
    TESDisplayMessageAddress = 0x00AE1D10;
    
    PlayerReferenceAddress = 0x05ADE398;
  } else if( F4_VERSION == F4_VERSION_1_10_26 ) {
    mainloop_hook_patch_address = 0x00D34DB7;
    mainloop_hook_return_address = 0x00D34DC3;
    
    loadgame_start_hook_patch_address = 0x00CEB740;
    loadgame_start_hook_return_address = 0x00CEB74F;
    loadgame_end_hook_patch_address = 0x00CEBF48;
    loadgame_end_hook_return_address = 0x00CEBF55;

    ProcessWindowAddress = 0x00D36B90;
    Unk3ObjectAddress = 0x05AC25E8;
    
    TESConsolePrintAddress = 0x01260EE0;
    TESConsoleObjectAddress = 0x058FEEB0;
    
    TESFormConstructorAddress = 0x00151E30;
    
    TESGlobalScriptCompileAddress = 0x004E7B10;
    GlobalScriptStateAddress = 0x05AF9720;
    
    TESScriptConstructorAddress = 0x004E14D0;
    TESScriptDestructorAddress = 0x004E1550;
    TESScriptMarkAsTemporaryAddress = 0x00153F00;
    TESScriptCompileAddress = 0x004E28C0;
    TESScriptExecuteAddress = 0x004E2440;
    TESScriptCompileAndRunAddress = 0x004E2810;
    TESScriptSetTextAddress = 0x004E20B0;
    
    TESPlayerMoveToCellAddress = 0x00E989E0;
    
    TESWorldSpaceFindExteriorCellByCoordinatesAddress = 0x004923C0;
    
    TESFindInteriorCellByNameAddress = 0x00152EB0;
    TESFindCellWorldSpaceByNameAddress = 0x00112FA0;
    TESDisplayMessageAddress = 0x00AE1D00;
    
    PlayerReferenceAddress = 0x05AC26F8;
  } else {
    //TODO(adm244): unsupported version
  }
}

internal void ShiftAddresses()
{
  baseAddress = (uint64)GetModuleHandle(0);
  
  mainloop_hook_patch_address += baseAddress;
  mainloop_hook_return_address += baseAddress;
  
  loadgame_start_hook_patch_address += baseAddress;
  loadgame_start_hook_return_address += baseAddress;
  loadgame_end_hook_patch_address += baseAddress;
  loadgame_end_hook_return_address += baseAddress;
  
  ProcessWindowAddress += baseAddress;
  Unk3ObjectAddress += baseAddress;
  
  TESConsolePrint = (_TESConsolePrint)(TESConsolePrintAddress + baseAddress);
  TESConsoleObjectAddress += baseAddress;
  
  TESForm_Constructor = (_TESForm_Constructor)(TESFormConstructorAddress + baseAddress);
  
  TESGlobalScript_Compile = (_TESGlobalScript_Compile)(TESGlobalScriptCompileAddress + baseAddress);
  GlobalScriptStateAddress += baseAddress;
  
  TESScript_Constructor = (_TESScript_Constructor)(TESScriptConstructorAddress + baseAddress);
  TESScript_Destructor = (_TESScript_Destructor)(TESScriptDestructorAddress + baseAddress);
  TESScript_MarkAsTemporary = (_TESScript_MarkAsTemporary)(TESScriptMarkAsTemporaryAddress + baseAddress);
  TESScript_Compile = (_TESScript_Compile)(TESScriptCompileAddress + baseAddress);
  TESScript_Execute = (_TESScript_Execute)(TESScriptExecuteAddress + baseAddress);
  TESScript_CompileAndRun = (_TESScript_CompileAndRun)(TESScriptCompileAndRunAddress + baseAddress);
  TESScript_SetText = (_TESScript_SetText)(TESScriptSetTextAddress + baseAddress);
  
  TESPlayer_MoveToCell = (_TESPlayer_MoveToCell)(TESPlayerMoveToCellAddress + baseAddress);
  
  TESWorldSpace_FindExteriorCellByCoordinates = (_TESWorldSpace_FindExteriorCellByCoordinates)(TESWorldSpaceFindExteriorCellByCoordinatesAddress + baseAddress);
  
  TESFindInteriorCellByName = (_TESFindInteriorCellByName)(TESFindInteriorCellByNameAddress + baseAddress);
  TESFindCellWorldSpaceByName = (_TESFindCellWorldSpaceByName)(TESFindCellWorldSpaceByNameAddress + baseAddress);
  TESDisplayMessage = (_TESDisplayMessage)(TESDisplayMessageAddress + baseAddress);
  
  PlayerReferenceAddress += baseAddress;
}
// ------ #Addresses ------

// ------ Functions ------
internal bool ExecuteScriptLine(char *text)
{
  bool result = false;
  
  TESScript scriptObject = {0};
  
  TESScript_Constructor(&scriptObject);
  TESScript_MarkAsTemporary(&scriptObject);
  TESScript_SetText(&scriptObject, text);
  result = TESScript_CompileAndRun(&scriptObject, (void *)GetGlobalScriptObject(), SysWindowCompileAndRun, 0);
  TESScript_Destructor(&scriptObject);
  
  return result;
}

internal bool IsInInterior(TESObjectReference *ref)
{
  bool result = false;

  if( ref ) {
    TESCell *parentCell = ref->parentCell;
    if( parentCell ) {
      if( parentCell->flags & TESCell_IsInterior ) result = true;
    }
  }
  
  return result;
}
// ------ #Functions ------

#endif
