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
*/

/*
  ConsolePrint buffer size: 4095 bytes allocated on stack
*/

/*
  Get cell regions:
    1) Call TESCell__sub_1403B4A10
    2) [eax + 0x8] - first region
    3) [eax + 0x10] - pointer to RegionUnk struct
    
      struct RegionUnk {
        TESRegion *region;
        RegionUnk *nextRegionUnk;
      };
*/

/*
  Check if cell is within region border:
    1) Get cell regions
    2) If there's border region the cell is within border region,
      otherwise it is outside.
*/

/*
  Is current cell interior or exterior check:
    1) Get ObjectReference pointer (rax)
      player is an object reference
      we have an address of player objectreference pointer (0x05AC26F8 for 1_10_26)
    2) Get parent cell ([rax + 0xB8] = rax')
    3) Check cell flags ([rax' + 0x40])
      first bit is interior\exterior (set - interior)
  END
*/

/* DEPRECATED
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
  END
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
    
    // 0x00CDC540 (1_10_26)
    // 0x00CDDE90 (1_10_40)
    bool SaveGame(void *BGSSaveLoadGame, char *filename, uint32 unk2);
*/

/*
  Console command "CenterOnExterior" algorithm:
    1) Takes first WorldSpace from list of all worldspaces (stored in array)
    2) Searches for cell by given coordinates
    3) Moves player to that cell
  END
*/

/*
  GetWorldSpaceArray,
  GetWorldSpaceCount,
  GetFormArray,
  GetFormCount:
  
    rcx = 0x058ED480 (1_10_26)
    dl = 0x4A (form type)
    
    rax = dl + dl*2 (= 0xDE)
      rcx + rax*8 + 0x68 (pointer to array of worldspaces)
      rcx + rax*8 + 0x78 (total worldspace count)  
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

// ------ TESObjectReference ------
//NOTE(adm244): attempts to move player into tesCell or cell with cellName (performs searching)
// objectRef - TESObjectReference object to move
// cellName - cell name to move to (if not null then searches for a cell)
// tesCell - TESCell object to move to (null if cellName is specified)
typedef bool (__fastcall *_TESObjectReference_MoveToCell)
(TESObjectReference *objectRef, char *cellName, TESCell *tesCell);
typedef TESLocation * (__fastcall *_TESObjectReference_GetCurrentLocation)
(TESObjectReference *objectRef);

internal _TESObjectReference_MoveToCell TESObjectReference_MoveToCell;
internal _TESObjectReference_GetCurrentLocation TESObjectReference_GetCurrentLocation;
// ------ #TESObjectReference ------

// ------ TESActor ------
typedef bool (__fastcall *_TESActor_IsDead)
(TESActor *actor, bool unk02);

//internal _TESActor_IsDead TESActor_IsDead;
// ------ #TESActor ------

// ------ TESCell ------
typedef TESCellUnk * (__fastcall *_TESCell_GetUnk)
(TESCell *cell, bool flag);

internal _TESCell_GetUnk TESCell_GetUnk;
// ------ #TESCell ------

// ------ TESWorldSpace ------
typedef TESCell * (__fastcall *_TESWorldSpace_FindExteriorCellByCoordinates)
(TESWorldSpace *tesWorldSpace, unsigned int cellX, unsigned int cellY);

internal _TESWorldSpace_FindExteriorCellByCoordinates TESWorldSpace_FindExteriorCellByCoordinates;
// ------ #TESWorldSpace ------

// ------ BSFixedString ------
typedef BSFixedString * (__fastcall *_BSFixedString_Constructor)
(BSFixedString **ptr, char *str);

typedef BSFixedString * (__fastcall *_BSFixedString_Set)
(BSFixedString **ptr, char *str);

typedef void (__fastcall *_BSFixedString_Release)(BSFixedString **ptr);

internal _BSFixedString_Constructor BSFixedString_Constructor;
internal _BSFixedString_Set BSFixedString_Set;
internal _BSFixedString_Release BSFixedString_Release;
// ------ #BSFixedString ------

// ------ TESUI ------
typedef bool (__fastcall *_TESUI_IsMenuOpen)(void *TESUIObject, BSFixedString **str);

internal _TESUI_IsMenuOpen TESUI_IsMenuOpen;
// ------ #TESUI ------

// ------ Utils ------
//NOTE(adm244): prints out a c-style formated string into the game console
typedef void (__fastcall *_TESFillConsoleBackbufferVA)
(void *consoleObject, char *format, va_list args);

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

typedef TESWorldSpace * (__fastcall *_TESFindCellWorldSpaceByName)
(void *unk0, char *cellName, unsigned int *cellX, unsigned int *cellY);

internal _TESFillConsoleBackbufferVA TESFillConsoleBackbufferVA;
internal _TESDisplayMessage TESDisplayMessage;
internal _TESFindInteriorCellByName TESFindInteriorCellByName;
internal _TESFindCellWorldSpaceByName TESFindCellWorldSpaceByName;
// ------ #Utils ------

// ------ Addresses ------
//IMPORTANT(adm244): it's getting messy, simplify addresses definition (macro?)

extern "C" {
  uint64 mainloop_hook_patch_address;
  uint64 mainloop_hook_return_address;

  uint64 loadgame_start_hook_patch_address;
  uint64 loadgame_start_hook_return_address;
  uint64 loadgame_end_hook_patch_address;
  uint64 loadgame_end_hook_return_address;

  uint64 ProcessWindowAddress;
  uint64 Unk3ObjectAddress;

  //uint64 TESConsolePrintAddress;
  uint64 TESConsoleObjectAddress;

  uint64 TESUIObjectAddress;
  //uint64 TESUIIsMenuOpenAddress;

  uint64 BSFixedStringConstructorAddress;
  uint64 BSFixedStringSetAddress;
  uint64 BSFixedStringReleaseAddress;

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

  uint64 TESObjectReferenceMoveToCellAddress;
  uint64 TESObjectReferenceGetCurrentLocationAddress;

  uint64 TESCellGetUnkAddress;

  uint64 TESWorldSpaceFindExteriorCellByCoordinatesAddress;

  uint64 TESFindInteriorCellByNameAddress;
  uint64 TESFindCellWorldSpaceByNameAddress;
  uint64 TESDisplayMessageAddress;

  uint64 PlayerReferenceAddress;

  uint64 GameDataAddress;

  uint64 UnknownObject01Address;
}

#include <assert.h>

#define PSAPI_VERSION 1
#include <psapi.h>

internal MODULEINFO gMainModuleInfo = {0};

//TODO(adm244): switch to multiple patterns search (Aho-Corasick?)
internal uint64 FindSignature(MODULEINFO *moduleInfo, char *pattern, char *mask, uint64 offset)
{
  uint64 startAddress = (uint64)moduleInfo->lpBaseOfDll;
  uint64 blockSize = moduleInfo->SizeOfImage;
  uint64 patternSize = strlen(mask);
  uint64 endAddress = startAddress + blockSize - patternSize;

  for (uint64 i = startAddress; i < endAddress; ++i) {
    bool matched = true;
    for (uint64 j = 0; j < patternSize; ++j) {
      if (mask[j] == '?') continue;
      if (pattern[j] != *((char *)(i + j))) {
        matched = false;
        break;
      }
    }
    
    if (matched) return (i + offset);
  }
  
  return 0;
}

internal uint64 ParseMemoryAddress(uint64 address, uint8 skipCount)
{
  int32 offset = *((int32 *)(address + skipCount));
  uint64 rip = (address + skipCount + sizeof(offset));
  
  return (rip + offset);
}

internal void InitHooks(MODULEINFO *moduleInfo)
{
  //TODO(adm244): advanced hooking mechanism
  // - Find start hook address by signature
  // - Pass manually specified instruction information (count, length, offsets, types (reljump, relmov, etc.))
  // - Write a detour
  // - Modify original instructions if necessary (mov relative to RIP)
  // - Add jump to the next instruction in the original function
  // - Write original instructions somewhere close in memory
  // There might be problem with saving registers
  
  mainloop_hook_patch_address = FindSignature(moduleInfo,
    "\xE8\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x8B\x58\x30",
    "x????xxx????xxxx", -0x7);
  
  mainloop_hook_return_address = mainloop_hook_patch_address + 12;
  
  loadgame_start_hook_patch_address = FindSignature(moduleInfo,
    "\x48\x8B\xF1\x4A\x8B\x04\xD0\xB9\xC0\x09\x00\x00\x45\x8B\xE8",
    "xxxxxxxxxxxxxxx", -0x39);
  
  loadgame_start_hook_return_address = loadgame_start_hook_patch_address + 15;
  
  //NOTE(adm244): use loadgame_start_hook_patch_address with an offset?
  //NOTE(adm244): distance is quite long, high chance of function change
  loadgame_end_hook_patch_address = FindSignature(moduleInfo,
    "\x89\x38\x41\x0F\xB6\xC5\x48",
    "xxxxxxx", 6);
  
  loadgame_end_hook_return_address = loadgame_end_hook_patch_address + 13;
  
  Unk3ObjectAddress = ParseMemoryAddress(mainloop_hook_patch_address, 3);
  assert(Unk3ObjectAddress != 0);
  
  ProcessWindowAddress = FindSignature(moduleInfo,
    "\x48\x89\x45\xFF\x48\x89\x45\x07\x48\x89\x45\x0F",
    "xxxxxxxxxxxx", -0x21);
  
  //assert(mainloop_hook_patch_address - (uint64)mainModule == 0x00D36707); //1_10_40
  //assert(mainloop_hook_return_address - (uint64)mainModule == 0x00D36713); //1_10_40
  //assert(loadgame_start_hook_patch_address - (uint64)mainModule == 0x00CED090); //1_10_40
  //assert(loadgame_start_hook_return_address - (uint64)mainModule == 0x00CED09F); //1_10_40
  //assert(loadgame_end_hook_patch_address - (uint64)mainModule == 0x00CED898); //1_10_40
  //assert(loadgame_end_hook_return_address - (uint64)mainModule == 0x00CED8A5); //1_10_40
  //assert(Unk3ObjectAddress - (uint64)mainModule == 0x05ADE288); //1_10_40
  //assert(ProcessWindowAddress - (uint64)mainModule == 0x00D384E0); //1_10_40
}

internal void InitTESConsole(uint64 memptr)
{
  TESConsoleObjectAddress = ParseMemoryAddress(memptr + 0x1A, 3);
  assert(TESConsoleObjectAddress != 0);
  
  uint64 memptr_fillconsole = ParseMemoryAddress(memptr + 0x28, 1);
  assert(memptr_fillconsole != 0);
  
  //TESFillConsoleBackbuffer = (_TESFillConsoleBackbuffer)memptr_fillconsole;
  //assert(TESFillConsoleBackbuffer != 0);
  
  TESFillConsoleBackbufferVA = (_TESFillConsoleBackbufferVA)ParseMemoryAddress(memptr_fillconsole + 0x18, 1);
  assert(TESFillConsoleBackbufferVA != 0);
}

internal void InitTESUI(uint64 memptr)
{
  TESUIObjectAddress = ParseMemoryAddress(memptr - 0x17A, 3);
  assert(TESUIObjectAddress != 0);
  
  TESUI_IsMenuOpen = (_TESUI_IsMenuOpen)ParseMemoryAddress(memptr - 0x161, 1);
  assert((uint64)TESUI_IsMenuOpen != 0);
}

internal void InitBSFixedString(MODULEINFO *moduleInfo)
{
  uint64 memptr = FindSignature(moduleInfo,
    "\x48\x8B\xFA\x4D\x8B\xF8\x4C\x89\x60\xD8",
    "xxxxxxxxxx", 0x59);
  assert(memptr != 0);
  
  BSFixedString_Set = (_BSFixedString_Set)ParseMemoryAddress(memptr, 1);
  assert((uint64)BSFixedString_Set != 0);
  
  BSFixedString_Constructor = (_BSFixedString_Constructor)ParseMemoryAddress(memptr + 0xF9, 1);
  BSFixedString_Release = (_BSFixedString_Release)ParseMemoryAddress((uint64)BSFixedString_Set + 0x23, 1);
  
  assert((uint64)BSFixedString_Constructor != 0);
  assert((uint64)BSFixedString_Release != 0);
}

internal void InitTESScript(MODULEINFO *moduleInfo, uint64 memptr_scriptstate)
{
  uint64 memptr = FindSignature(&gMainModuleInfo,
    "\x41\xB8\x01\x00\x00\x00\x48\x89\x44\x24\x78",
    "xxxxxxxxxxx", -0x4E);
  assert(memptr != 0);
  
  uint64 memptr_compilerun = ParseMemoryAddress(memptr + 0x59, 1);
  assert(memptr_compilerun != 0);
  
  GlobalScriptStateAddress = ParseMemoryAddress(memptr_scriptstate, 3);
  assert(GlobalScriptStateAddress != 0);
  
  //TESExecuteScriptLine = (_TESExecuteScriptLine)memptr;
  TESScript_Constructor = (_TESScript_Constructor)ParseMemoryAddress(memptr + 0x18, 1);
  TESScript_MarkAsTemporary = (_TESScript_MarkAsTemporary)ParseMemoryAddress(memptr + 0x22, 1);
  TESScript_SetText = (_TESScript_SetText)ParseMemoryAddress(memptr + 0x2F, 1);
  TESScript_Compile = (_TESScript_Compile)ParseMemoryAddress(memptr_compilerun + 0x47, 1);
  TESScript_Execute = (_TESScript_Execute)ParseMemoryAddress(memptr_compilerun + 0x75, 1);
  TESScript_CompileAndRun = (_TESScript_CompileAndRun)memptr_compilerun;
  TESScript_Destructor = (_TESScript_Destructor)ParseMemoryAddress(memptr + 0x63, 1);
  
  //assert((uint64)TESExecuteScriptLine != 0);
  assert((uint64)TESScript_Constructor != 0);
  assert((uint64)TESScript_MarkAsTemporary != 0);
  assert((uint64)TESScript_SetText != 0);
  assert((uint64)TESScript_Compile != 0);
  assert((uint64)TESScript_Execute != 0);
  assert((uint64)TESScript_CompileAndRun != 0);
  assert((uint64)TESScript_Destructor != 0);
  
  //assert(TESScriptConstructorAddress - (uint64)mainModule == 0x004E14F0); //1_10_40
  //assert(TESScriptMarkAsTemporaryAddress - (uint64)mainModule == 0x00153F00); //1_10_40
  //assert(TESScriptSetTextAddress - (uint64)mainModule == 0x004E20D0); //1_10_40*/
  //assert(TESScriptCompileAddress - (uint64)mainModule == 0x004E28E0); //1_10_40
  //assert(TESScriptExecuteAddress - (uint64)mainModule == 0x004E2460); //1_10_40
  //assert(TESScriptCompileAndRunAddress - (uint64)mainModule == 0x004E2830); //1_10_40
  //assert(TESScriptDestructorAddress - (uint64)mainModule == 0x004E1570); //1_10_40
}

internal void InitSignatures()
{
  //NOTE(adm244): https://docs.microsoft.com/en-us/windows/desktop/api/processthreadsapi/nf-processthreadsapi-getcurrentprocess#remarks
  HANDLE currentProcess = GetCurrentProcess();
  HMODULE mainModule = GetModuleHandle(0);
  BOOL result;
  
  //FIX(adm244): https://docs.microsoft.com/en-us/windows/desktop/api/psapi/nf-psapi-getmoduleinformation#remarks
  result = GetModuleInformation(currentProcess, mainModule, &gMainModuleInfo, sizeof(gMainModuleInfo));
  assert(result != 0);
  
  uint64 memptr = FindSignature(&gMainModuleInfo,
    "\x75\xF7\x85\xC0\x74\x32",
    "xxxxxx", 0x6);
  assert(memptr != 0);
  
  InitHooks(&gMainModuleInfo);
  
  InitTESConsole(memptr);
  InitTESUI(memptr);
  
  InitBSFixedString(&gMainModuleInfo);
  InitTESScript(&gMainModuleInfo, memptr);
  
  //TODO(adm244): search for object constructor, get object and vtable addresses
  //TODO(adm244): get function pointers from vtable?
  
  //FIX(adm244): are those ever used?
  /*TESForm_Constructor = (_TESForm_Constructor)FindSignature(&gMainModuleInfo,
    "\x41\xBC\xC0\x09\x00\x00\x43\x8B\x1C\x3C\x48\x8B\xF9",
    "xxxxxxxxxxxxx", -0x36);
  //assert(TESFormConstructorAddress - (uint64)mainModule == 0x00151E30); //1_10_40
  
  TESGlobalScript_Compile = (_TESGlobalScript_Compile)FindSignature(&gMainModuleInfo,
    "\x48\x8B\xF1\x48\x8D\x4C\x24\x20\x41\x8B\xF9",
    "xxxxxxxxxxx", -0x12);
  //assert(TESGlobalScriptCompileAddress - (uint64)mainModule == 0x004E7B30); //1_10_40*/
  
  TESObjectReference_MoveToCell = (_TESObjectReference_MoveToCell)FindSignature(&gMainModuleInfo,
    "\xEB\x45\xF6\x43\x40\x01\x75\x3F",
    "xxxxxxxx", -0x94);
  //assert(TESObjectReferenceMoveToCellAddress - (uint64)mainModule == 0x00E9A330); //1_10_40
  
  TESObjectReference_GetCurrentLocation = (_TESObjectReference_GetCurrentLocation)FindSignature(&gMainModuleInfo,
    "\x5B\xC3\x48\x8B\x89\xB8\x00\x00\x00",
    "xxxxxxxxx", -0x21);
  //assert(TESObjectReferenceGetCurrentLocationAddress - (uint64)mainModule == 0x0040EE70); //1_10_40
  
  TESCell_GetUnk = (_TESCell_GetUnk)FindSignature(&gMainModuleInfo,
    "\xF6\x45\x40\x01\x74\x04\x33\xFF\xEB",
    "xxxxxxxxx", -0x45);
  //assert(TESCellGetUnkAddress - (uint64)mainModule == 0x003B4A30); //1_10_40
  
  TESWorldSpace_FindExteriorCellByCoordinates = (_TESWorldSpace_FindExteriorCellByCoordinates)FindSignature(&gMainModuleInfo,
    "\x84\xC0\x0F\x85\x00\x00\x00\x00\x83\xCA\xFF",
    "xxxx????xxx", -0x76);
  //assert(TESWorldSpaceFindExteriorCellByCoordinatesAddress - (uint64)mainModule == 0x004923E0); //1_10_40
  
  //FIX(adm244): wrapper function
  /*TESFindInteriorCellByNameAddress = FindSignature(&gMainModuleInfo,
    "\x48\x8B\xD1\x48\x8D\x4C\x24\x38\xE8\x00\x00\x00\x00\x48\x8B\xC8\xE8",
    "xxxxxxxxx????xxxx", -0x6);
  assert(TESFindInteriorCellByNameAddress - (uint64)mainModule == 0x00152EB0); //1_10_40*/
  
  TESFindCellWorldSpaceByName = (_TESFindCellWorldSpaceByName)FindSignature(&gMainModuleInfo,
    "\x0F\x94\xC0\x4D\x85\xE4\x75\x18",
    "xxxxxxxx", -0xB2);
  //assert(TESFindCellWorldSpaceByNameAddress - (uint64)mainModule == 0x00112FA0); //1_10_40
  
  TESDisplayMessage = (_TESDisplayMessage)FindSignature(&gMainModuleInfo,
    "\x48\x83\xEC\x50\xC6\x44\x24\x28\x00\x44\x88\x4C\x24\x20",
    "xxxxxxxxxxxxxx", -0x2);
  //assert(TESDisplayMessageAddress - (uint64)mainModule == 0x00AE1D10); //1_10_40
  
  //TODO(adm244): convert all addresses
  
  assert(mainloop_hook_patch_address != 0);
  assert(mainloop_hook_return_address != 0);
  assert(loadgame_start_hook_patch_address != 0);
  assert(loadgame_start_hook_return_address != 0);
  assert(loadgame_end_hook_patch_address != 0);
  assert(loadgame_end_hook_return_address != 0);
  assert(ProcessWindowAddress != 0);
  //assert((uint64)TESForm_Constructor != 0);
  //assert((uint64)TESGlobalScript_Compile != 0);
  assert((uint64)TESObjectReference_MoveToCell != 0);
  assert((uint64)TESObjectReference_GetCurrentLocation != 0);
  assert((uint64)TESCell_GetUnk != 0);
  assert((uint64)TESWorldSpace_FindExteriorCellByCoordinates != 0);
  assert((uint64)TESFindCellWorldSpaceByName != 0);
  assert((uint64)TESDisplayMessage != 0);
}

/*internal void DefineAddresses()
{
  if( F4_VERSION == F4_VERSION_1_10_40 ) {
    mainloop_hook_patch_address = 0x00D36707;
    mainloop_hook_return_address = 0x00D36713;
    
    loadgame_start_hook_patch_address = 0x00CED090;
    loadgame_start_hook_return_address = 0x00CED09F;
    loadgame_end_hook_patch_address = 0x00CED898;
    loadgame_end_hook_return_address = 0x00CED8A5;

    ProcessWindowAddress = 0x00D384E0;
    Unk3ObjectAddress = 0x05ADE288; // X

    TESConsolePrintAddress = 0x01262830;
    TESConsoleObjectAddress = 0x0591AB30; // X

    TESUIObjectAddress = 0x0590A918; // X
    TESUIIsMenuOpenAddress = 0x020420E0;
    
    BSFixedStringConstructorAddress = 0x01B416E0;
    BSFixedStringSetAddress = 0x01B41810;
    BSFixedStringReleaseAddress = 0x01B42970;
    
    TESFormConstructorAddress = 0x00151E30;

    TESGlobalScriptCompileAddress = 0x004E7B30;
    GlobalScriptStateAddress = 0x05B15420; // X

    TESScriptConstructorAddress = 0x004E14F0;
    TESScriptDestructorAddress = 0x004E1570;
    TESScriptMarkAsTemporaryAddress = 0x00153F00;
    TESScriptCompileAddress = 0x004E28E0;
    TESScriptExecuteAddress = 0x004E2460;
    TESScriptCompileAndRunAddress = 0x004E2830;
    TESScriptSetTextAddress = 0x004E20D0;
    
    TESObjectReferenceMoveToCellAddress = 0x00E9A330;
    TESObjectReferenceGetCurrentLocationAddress = 0x0040EE70;
    
    TESCellGetUnkAddress = 0x003B4A30;
    
    TESWorldSpaceFindExteriorCellByCoordinatesAddress = 0x004923E0;
    
    TESFindInteriorCellByNameAddress = 0x00152EB0;
    TESFindCellWorldSpaceByNameAddress = 0x00112FA0;
    TESDisplayMessageAddress = 0x00AE1D10;
    
    PlayerReferenceAddress = 0x05ADE398; // X
    
    GameDataAddress = 0x0590AB80; // X
    
    UnknownObject01Address = 0x05ADE288; // X
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
    
    TESUIObjectAddress = 0x058EEC98;
    TESUIIsMenuOpenAddress = 0x02040780;
    
    BSFixedStringConstructorAddress = 0x01B3FD80;
    BSFixedStringSetAddress = 0x01B3FEB0;
    BSFixedStringReleaseAddress = 0x01B41010;
    
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
    
    TESObjectReferenceMoveToCellAddress = 0x00E989E0;
    TESObjectReferenceGetCurrentLocationAddress = 0x0040EE50;
    
    TESCellGetUnkAddress = 0x003B4A10;
    
    TESWorldSpaceFindExteriorCellByCoordinatesAddress = 0x004923C0;
    
    TESFindInteriorCellByNameAddress = 0x00152EB0;
    TESFindCellWorldSpaceByNameAddress = 0x00112FA0;
    TESDisplayMessageAddress = 0x00AE1D00;
    
    PlayerReferenceAddress = 0x05AC26F8;
    
    GameDataAddress = 0x058ED480;
    
    UnknownObject01Address = 0x05AC25E8;
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
  
  TESUIObjectAddress += baseAddress;
  TESUI_IsMenuOpen = (_TESUI_IsMenuOpen)(TESUIIsMenuOpenAddress + baseAddress);
  
  BSFixedString_Constructor = (_BSFixedString_Constructor)(BSFixedStringConstructorAddress + baseAddress);
  BSFixedString_Set = (_BSFixedString_Set)(BSFixedStringSetAddress + baseAddress);
  BSFixedString_Release = (_BSFixedString_Release)(BSFixedStringReleaseAddress + baseAddress);
  
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
  
  TESObjectReference_MoveToCell = (_TESObjectReference_MoveToCell)(TESObjectReferenceMoveToCellAddress + baseAddress);
  TESObjectReference_GetCurrentLocation = (_TESObjectReference_GetCurrentLocation)(TESObjectReferenceGetCurrentLocationAddress + baseAddress);
  
  TESCell_GetUnk = (_TESCell_GetUnk)(TESCellGetUnkAddress + baseAddress);
  
  TESWorldSpace_FindExteriorCellByCoordinates = (_TESWorldSpace_FindExteriorCellByCoordinates)(TESWorldSpaceFindExteriorCellByCoordinatesAddress + baseAddress);
  
  TESFindInteriorCellByName = (_TESFindInteriorCellByName)(TESFindInteriorCellByNameAddress + baseAddress);
  TESFindCellWorldSpaceByName = (_TESFindCellWorldSpaceByName)(TESFindCellWorldSpaceByNameAddress + baseAddress);
  TESDisplayMessage = (_TESDisplayMessage)(TESDisplayMessageAddress + baseAddress);
  
  PlayerReferenceAddress += baseAddress;
  
  GameDataAddress += baseAddress;
  
  UnknownObject01Address += baseAddress;
}*/
// ------ #Addresses ------

// ------ Functions ------
internal bool TES_ExecuteScriptLine(char *text)
{
  bool result = false;
  
  TESScript scriptObject = {0};
  
  TESScript_Constructor(&scriptObject);
  TESScript_MarkAsTemporary(&scriptObject);
  TESScript_SetText(&scriptObject, text);
  result = TESScript_CompileAndRun(&scriptObject, *(void **)(GlobalScriptStateAddress), SysWindowCompileAndRun, 0);
  TESScript_Destructor(&scriptObject);
  
  return result;
}

internal TESPlayer * TES_GetPlayer()
{
  return *(TESPlayer **)(PlayerReferenceAddress);
}

internal inline bool TES_IsInterior(TESCell *cell)
{
  return cell->flags & FLAG_TESCell_IsInterior;
}

internal inline bool TES_IsInInterior(TESObjectReference *ref)
{
  bool result = false;

  if( ref ) {
    TESCell *parentCell = ref->parentCell;
    if( parentCell ) result = TES_IsInterior(parentCell);
  }
  
  return result;
}

internal inline TESLocation * TES_GetPlayerLocation()
{
  TESPlayer *player = TES_GetPlayer();
  return player->location;
}

//FIX(adm244): spell worldspace in types.h with non-capital 's'
internal TESWorldSpace * GetPlayerCurrentWorldSpace()
{
  TESWorldSpace *worldspace = 0;

  TESPlayer *player = TES_GetPlayer();
  TESCell *playerCell = player->tesActor.objectReference.parentCell;
  
  if( playerCell ) {
    worldspace = playerCell->worldSpace;
    
    if( worldspace ) {
      while( worldspace->parentWorldSpace ) {
        worldspace = worldspace->parentWorldSpace;
      }
    } else {
      TESLocation *location = TES_GetPlayerLocation();
      if( location ) {
        while( location->parent ) {
          location = location->parent;
        }
        
        int worldSpaceCount = TES_GetWorldSpaceCount();
        TESWorldSpace **worldspaceArray = TES_GetWorldSpaceArray();
        
        for( int i = 0; i < worldSpaceCount; ++i ) {
          //NOTE(adm244): convert it to be syntacticly an array?
          TESWorldSpace *p = *(worldspaceArray + i);
          
          if( p->location == location ) {
            //NOTE(adm244): get root worldspace?
            worldspace = p;
            break;
          }
        }
        
        if( !worldspace ) {
          //NOTE(adm244): location isn't attached to any of worldspaces, try encounter zone?
        }
      } else {
        //NOTE(adm244): no location attached to player's cell, try encounter zone?
      }
    }
  }
  
  return worldspace;
}

internal bool IsCellWithinBorderRegion(TESCell *cell)
{
  //FIX(adm244): doesn't work for every cell (editor bug)
  bool result = false;

  TESCellUnk *cellUnk = TESCell_GetUnk(cell, 1);
  
  if( cellUnk ) {
    RegionUnk *regionUnk = (RegionUnk *)(&cellUnk->region);
    if( regionUnk ) {
      while( regionUnk ) {
        TESRegion *region = regionUnk->region;
        if( !region ) break;
        
        result = ((region->tesForm.flags) >> 6) && 1;
        if( result ) break;
        
        regionUnk = regionUnk->nextRegionUnk;
      }
    }
  }
  
  return result;
}

internal bool IsInDialogueWithPlayer(TESActor *actor)
{
  //NOTE(adm244): has potential problems with some dialogues
  return actor->flags_3 & FLAG_TESActor_IsInDialogue;
}

//internal bool TES_IsTalking(TESActor *actor)
//{
  // call 0x270 member function
//}

internal bool IsInMenuMode()
{
  //TODO(adm244): get class member macro?
  uint64 unkObject01 = *((uint64 *)UnknownObject01Address);
  return *((uint8 *)(unkObject01 + 0x1D0)) == 1;
}

internal inline bool IsPlayerInInterior()
{
  return TES_IsInInterior((TESObjectReference *)TES_GetPlayer());
}

internal inline bool IsPlayerInDialogue()
{
  return IsInDialogueWithPlayer((TESActor *)TES_GetPlayer());
}

internal inline bool IsMenuOpen(char *str)
{
  bool result = false;

  //BSFixedString bsString = {0};
  BSFixedString *bsString;
  BSFixedString_Constructor(&bsString, str);
  result = TESUI_IsMenuOpen(*((void **)TESUIObjectAddress), &bsString);
  BSFixedString_Release(&bsString);
  
  return result;
}

internal inline bool IsActorDead(TESActor *actor)
{
  //TODO(adm244): member function call macro?
  _TESActor_IsDead funcPtr = (_TESActor_IsDead)( *(((uint64 *)((TESForm *)actor)->vtable) + 192) );
  return funcPtr(actor, 1);
}

//FIX(adm244): it's actually more like FillConsoleBackbuffer,
// because it fills a buffer and adds some meta data to it (like text length, etc.)
// which then gets copied into real console buffer
internal inline void TESConsolePrint(char *format, ...)
{
  va_list args;
  va_start(args, format);
  //FIX(adm244): TES_GetConsoleObject is actually not an object, it's a structure
  TESFillConsoleBackbufferVA(*(void **)(TESConsoleObjectAddress), format, args);
  va_end(args);
}
// ------ #Functions ------

#endif
