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

#ifndef _SIGNATURES_H
#define _SIGNATURES_H

#define PSAPI_VERSION 1
#include <psapi.h>

internal MODULEINFO gMainModuleInfo = {0};

#ifdef F4_VERSION_1_10_40
internal HMODULE gMainModule = GetModuleHandle(0);
#endif

extern "C" {
  uint64 mainloop_hook_patch_address;
  uint64 mainloop_hook_return_address;

  uint64 loadgame_start_hook_patch_address;
  uint64 loadgame_start_hook_return_address;
  uint64 loadgame_end_hook_patch_address;
  uint64 loadgame_end_hook_return_address;

  uint64 ProcessWindowAddress;
  uint64 Unk3ObjectAddress;
  uint64 TESConsoleObjectAddress;
  BSInputEventReceiver **BSInputEventReceiverPtr;
  uint64 GlobalScriptStateAddress;
  uint64 PlayerReferenceAddress;
  uint64 GameDataAddress;
  
  // terminal
  uint64 prepare_hacking_hook_addr;
  uint64 quit_hacking_hook_addr;
  
  TESObjectReference **gActiveTerminalREFR;
  BGSTerminal **gActiveTerminalForm;
  int32 *gTerminalTryCount;
  
  // obscript
  ObScriptCommand *gObScriptCommands;
}

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

//NOTE(adm244): rename to "ParseRIPRelativeAddress" or something like that
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
  assert(mainloop_hook_patch_address != 0);
  
  mainloop_hook_return_address = mainloop_hook_patch_address + 12;
  
  loadgame_start_hook_patch_address = FindSignature(moduleInfo,
    "\x48\x8B\xF1\x4A\x8B\x04\xD0\xB9\xC0\x09\x00\x00\x45\x8B\xE8",
    "xxxxxxxxxxxxxxx", -0x39);
  assert(loadgame_start_hook_patch_address != 0);
  
  loadgame_start_hook_return_address = loadgame_start_hook_patch_address + 15;
  
  //NOTE(adm244): use loadgame_start_hook_patch_address with an offset?
  //NOTE(adm244): distance is quite long, high chance of function change
  loadgame_end_hook_patch_address = FindSignature(moduleInfo,
    "\x89\x38\x41\x0F\xB6\xC5\x48",
    "xxxxxxx", 6);
  assert(loadgame_end_hook_patch_address != 0);
  
  loadgame_end_hook_return_address = loadgame_end_hook_patch_address + 13;
  
  Unk3ObjectAddress = ParseMemoryAddress(mainloop_hook_patch_address, 3);
  assert(Unk3ObjectAddress != 0);
  
  ProcessWindowAddress = ParseMemoryAddress(mainloop_hook_patch_address + 0x7, 1);
  assert(ProcessWindowAddress != 0);
  
#ifdef F4_VERSION_1_10_40
  assert(mainloop_hook_patch_address - (uint64)gMainModule == 0x00D36707); //1_10_40
  assert(mainloop_hook_return_address - (uint64)gMainModule == 0x00D36713); //1_10_40
  assert(loadgame_start_hook_patch_address - (uint64)gMainModule == 0x00CED090); //1_10_40
  assert(loadgame_start_hook_return_address - (uint64)gMainModule == 0x00CED09F); //1_10_40
  assert(loadgame_end_hook_patch_address - (uint64)gMainModule == 0x00CED898); //1_10_40
  assert(loadgame_end_hook_return_address - (uint64)gMainModule == 0x00CED8A5); //1_10_40
  assert(Unk3ObjectAddress - (uint64)gMainModule == 0x05ADE288); //1_10_40
  assert(ProcessWindowAddress - (uint64)gMainModule == 0x00D384E0); //1_10_40
#endif
}

internal void PatchRandom(MODULEINFO *moduleInfo)
{
  uint64 memptr = FindSignature(moduleInfo,
    "\x33\xC9\x0F\xC6\xCA\xAA\x0F\xC6\xD2\xFF",
    "xxxxxxxxxx", 0x16);
  assert(memptr != 0);

  uint64 func_randomint = ParseMemoryAddress(memptr, 1);
  assert(func_randomint != 0);
  
  uint64 func_randomfloat = ParseMemoryAddress(memptr - 0x4CD, 1);
  assert(func_randomfloat != 0);
  
#ifdef F4_VERSION_1_10_40
  assert(func_randomint - (uint64)gMainModule == 0x01B12780); //1_10_40
  assert(func_randomfloat - (uint64)gMainModule == 0x01B127F0); //1_10_40
#endif
  
  WriteBranch(func_randomint, (uint64)RandomIntDefault);
  WriteBranch(func_randomfloat, (uint64)RandomFloatDefault);
}

internal void PatchTerminalHacking(MODULEINFO *moduleInfo)
{
  uint64 memptr = FindSignature(moduleInfo,
    "\x48\x89\x5D\x17\x3C\x0A",
    "xxxxxx", -0x1C);
  assert(memptr != 0);
  
  prepare_hacking_hook_addr = (memptr + 0x8);
  assert(prepare_hacking_hook_addr != 0);
  
  WriteBranch(prepare_hacking_hook_addr, (uint64)HackingPrepare_Hook);
  SafeWrite8(prepare_hacking_hook_addr + 12, 0x90);
  
  uint64 prepare_hacking_func_memptr = ParseMemoryAddress(memptr, 1);
  assert(prepare_hacking_func_memptr != 0);
  
  gActiveTerminalREFR = (TESObjectReference **)ParseMemoryAddress(prepare_hacking_func_memptr + 0x36, 3);
  assert(gActiveTerminalREFR != 0);
  gActiveTerminalForm = (BGSTerminal **)ParseMemoryAddress(prepare_hacking_func_memptr + 0x3D, 3);
  assert(gActiveTerminalForm != 0);
  
  uint64 initialize_hacking_func_memptr = ParseMemoryAddress(prepare_hacking_func_memptr + 0x31, 1);
  assert(initialize_hacking_func_memptr != 0);
  
  gTerminalTryCount = (int32 *)ParseMemoryAddress(initialize_hacking_func_memptr + 0x1E8, 2);
  assert(gTerminalTryCount != 0);
  
  memptr = FindSignature(moduleInfo,
    "\x48\x8D\x55\xF8\x48\x81\xC1\x20\x01\x00\x00",
    "xxxxxxxxxxx", 0x0);
  assert(memptr != 0);
  
  quit_hacking_hook_addr = memptr;
  WriteBranch(quit_hacking_hook_addr, (uint64)HackingQuit_Hook);
  SafeWrite8(quit_hacking_hook_addr + 12, 0x90);
  SafeWrite8(quit_hacking_hook_addr + 13, 0x90);
  SafeWrite8(quit_hacking_hook_addr + 14, 0x90);
}

internal void InitPatches(MODULEINFO *moduleInfo)
{
  PatchRandom(moduleInfo);
  PatchTerminalHacking(moduleInfo);
}

internal void InitObScript(MODULEINFO *moduleInfo)
{
  uint64 memptr = FindSignature(moduleInfo,
    "\x48\x8B\x43\x38\x0F\xB7\x4B\x22",
    "xxxxxxxx", -0xB7);
  assert(memptr != 0);
  
  gObScriptCommands = (ObScriptCommand *)ParseMemoryAddress(memptr, 3);
  assert(gObScriptCommands != 0);
}

enum ObScriptFunctions {
  ObScript_Lock = 114,
};

internal void InitObScriptDependentPointers()
{
  assert(gObScriptCommands != 0);
  
  // Lock(TESObjectREFR *, ...) - 114
  ObScriptCommand obLock = gObScriptCommands[ObScript_Lock];
  
  uint64 memptr = (uint64)(obLock.function);
  assert(memptr != 0);
  
  uint64 TESObjectReference_GetExistingExtraLockData = ParseMemoryAddress(memptr + 0x8E, 1);
  assert(TESObjectReference_GetExistingExtraLockData != 0);
  
  ExtraDataList_GetExtraLockData = (_ExtraDataList_GetExtraLockData)
    ParseMemoryAddress(TESObjectReference_GetExistingExtraLockData + 0x14, 1);
  assert(ExtraDataList_GetExtraLockData != 0);
  
  ExtraDataList_Find = (_ExtraDataList_Find)ParseMemoryAddress((uint64)ExtraDataList_GetExtraLockData + 0x6, 1);
  assert(ExtraDataList_Find != 0);
  
  BSReadWriteLock_Lock = (_BSReadWriteLock_Lock)ParseMemoryAddress((uint64)ExtraDataList_Find + 0x1C, 1);
  assert(BSReadWriteLock_Lock != 0);
  
  BSReadWriteLock_Unlock = (_BSReadWriteLock_Unlock)ParseMemoryAddress((uint64)ExtraDataList_Find + 0x39, 1);
  assert(BSReadWriteLock_Unlock != 0);
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
  
#ifdef F4_VERSION_1_10_40
  assert(TESConsoleObjectAddress - (uint64)gMainModule == 0x0591AB30); //1_10_40
  assert(memptr_fillconsole - (uint64)gMainModule == 0x01262830); //1_10_40
#endif
}

internal void InitTESUI(uint64 memptr)
{
  BSInputEventReceiverPtr = (BSInputEventReceiver **)ParseMemoryAddress(memptr - 0x17A, 3);
  assert((uint64)BSInputEventReceiverPtr != 0);
  
  Native_IsMenuOpen = (_Native_IsMenuOpen)ParseMemoryAddress(memptr - 0x161, 1);
  assert((uint64)Native_IsMenuOpen != 0);
  
#ifdef F4_VERSION_1_10_40
  assert((uint64)BSInputEventReceiverPtr - (uint64)gMainModule == 0x0590A918); //1_10_40
  assert((uint64)Native_IsMenuOpen - (uint64)gMainModule == 0x020420E0); //1_10_40
#endif
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
  
#ifdef F4_VERSION_1_10_40
  assert((uint64)BSFixedString_Set - (uint64)gMainModule == 0x01B41810); //1_10_40
  assert((uint64)BSFixedString_Constructor - (uint64)gMainModule == 0x01B416E0); //1_10_40
  assert((uint64)BSFixedString_Release - (uint64)gMainModule == 0x01B42970); //1_10_40
#endif
}

internal void InitTESScript(MODULEINFO *moduleInfo, uint64 memptr_scriptstate)
{
  uint64 memptr = FindSignature(moduleInfo,
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
  
#ifdef F4_VERSION_1_10_40
  assert(GlobalScriptStateAddress - (uint64)gMainModule == 0x05B15420); //1_10_40
  assert((uint64)TESScript_Constructor - (uint64)gMainModule == 0x004E14F0); //1_10_40
  assert((uint64)TESScript_MarkAsTemporary - (uint64)gMainModule == 0x00153F00); //1_10_40
  assert((uint64)TESScript_SetText - (uint64)gMainModule == 0x004E20D0); //1_10_40
  assert((uint64)TESScript_Compile - (uint64)gMainModule == 0x004E28E0); //1_10_40
  assert((uint64)TESScript_Execute - (uint64)gMainModule == 0x004E2460); //1_10_40
  assert((uint64)TESScript_CompileAndRun - (uint64)gMainModule == 0x004E2830); //1_10_40
  assert((uint64)TESScript_Destructor - (uint64)gMainModule == 0x004E1570); //1_10_40
#endif
}

internal void InitUtils(uint64 memptr)
{
  TESFindInteriorCellByName = (_TESFindInteriorCellByName)ParseMemoryAddress(memptr + 0x22, 1);
  assert((uint64)TESFindInteriorCellByName != 0);
  
  TESFindCellWorldSpaceByName = (_TESFindCellWorldSpaceByName)ParseMemoryAddress(memptr + 0x63, 1);
  assert((uint64)TESFindCellWorldSpaceByName != 0);
  
#ifdef F4_VERSION_1_10_40
  assert((uint64)TESFindInteriorCellByName - (uint64)gMainModule == 0x00152EB0); //1_10_40
  assert((uint64)TESFindCellWorldSpaceByName - (uint64)gMainModule == 0x00112FA0); //1_10_40
#endif
}

internal void InitTESWorldSpace(uint64 memptr)
{
  uint64 memptr_getenczone = ParseMemoryAddress(memptr + 0x73, 1);
  assert(memptr_getenczone != 0);
  
  uint64 memptr_findcell = ParseMemoryAddress(memptr + 0x8C, 1);
  assert(memptr_findcell != 0);
  
  TESWorldSpace_GetEncounterZone = (_TESWorldSpace_GetEncounterZone)memptr_getenczone;
  TESWorldSpace_GetLocation = (_TESWorldSpace_GetLocation)ParseMemoryAddress(memptr_getenczone + 0x12, 1);
  TESWorldSpace_FindExteriorCell = (_TESWorldSpace_FindExteriorCell)memptr_findcell;
  //TESWorldSpace_GetCellAt = (_TESWorldSpace_GetCellAt)ParseMemoryAddress(memptr_findcell + 0x5A, 1);
  //TESLocation_GetEncounterZone = (_TESLocation_GetEncounterZone)ParseMemoryAddress(memptr_getenczone + 0x26, 1);
  
  assert((uint64)TESWorldSpace_GetEncounterZone != 0);
  assert((uint64)TESWorldSpace_GetLocation != 0);
  assert((uint64)TESWorldSpace_FindExteriorCell != 0);
  //assert((uint64)TESWorldSpace_GetCellAt != 0);
  //assert((uint64)TESLocation_GetEncounterZone != 0);
  
#ifdef F4_VERSION_1_10_40
  assert((uint64)TESWorldSpace_GetEncounterZone - (uint64)gMainModule == 0x004932D0); //1_10_40
  assert((uint64)TESWorldSpace_GetLocation - (uint64)gMainModule == 0x00493710); //1_10_40
  assert((uint64)TESWorldSpace_FindExteriorCell - (uint64)gMainModule == 0x004923E0); //1_10_40
#endif
}

internal void InitGlobalVariables(uint64 memptr)
{
  uint64 memptr_unkfunc = ParseMemoryAddress(memptr + 0x15, 1);
  assert(memptr_unkfunc != 0);
  
  PlayerReferenceAddress = ParseMemoryAddress(memptr_unkfunc + 0x7D, 3);
  GameDataAddress = ParseMemoryAddress(memptr + 0x47, 3);
  
  assert(PlayerReferenceAddress != 0);
  assert(GameDataAddress != 0);
  
#ifdef F4_VERSION_1_10_40
  assert(PlayerReferenceAddress - (uint64)gMainModule == 0x05ADE398); //1_10_40
  assert(GameDataAddress - (uint64)gMainModule == 0x05909100); //1_10_40
#endif
}

internal void InitTESObjectReference(MODULEINFO *moduleInfo, uint64 memptr)
{
  TESObjectReference_MoveToCell = (_TESObjectReference_MoveToCell)memptr;
  assert((uint64)TESObjectReference_MoveToCell != 0);
  
  TESObjectReference_GetCurrentLocation = (_TESObjectReference_GetCurrentLocation)FindSignature(moduleInfo,
    "\x5B\xC3\x48\x8B\x89\xB8\x00\x00\x00",
    "xxxxxxxxx", -0x21);
  assert((uint64)TESObjectReference_GetCurrentLocation != 0);
  
#ifdef F4_VERSION_1_10_40
  assert((uint64)TESObjectReference_MoveToCell - (uint64)gMainModule == 0x00E9A330); //1_10_40
  assert((uint64)TESObjectReference_GetCurrentLocation - (uint64)gMainModule == 0x0040EE70); //1_10_40
#endif
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
  InitPatches(&gMainModuleInfo);
  
  InitObScript(&gMainModuleInfo);
  InitObScriptDependentPointers();
  
  InitTESConsole(memptr);
  InitTESUI(memptr);
  
  InitBSFixedString(&gMainModuleInfo);
  InitTESScript(&gMainModuleInfo, memptr);
  
  //TODO(adm244): get these pointers from MoveToCell function:
  // - FindInteriorCellByName (done)
  // - FindCellWorldSpaceByName (done)
  // - TESWorldspace::GetEncounterZone (done)
  // - TESWorldspace::FindExteriorCell (done)
  // - TESCell::GetEncounterZone
  // Also, these pointers from TESWorldspace::GetEncounterZone:
  // - TESWorldspace::GetLocation (done)
  // - TESLocation::GetEncounterZone (done)
  // Also, these pointers from TESWorldspace::FindExteriorCell:
  // - TESWorldspace::GetCellAt (done)
  // - TESCell::Constructor
  
  uint64 memptr_movetocell = FindSignature(&gMainModuleInfo,
    "\xEB\x45\xF6\x43\x40\x01\x75\x3F",
    "xxxxxxxx", -0x94);
  
  InitUtils(memptr_movetocell);
  InitTESWorldSpace(memptr_movetocell);
  InitGlobalVariables(memptr_movetocell);
  InitTESObjectReference(&gMainModuleInfo, memptr_movetocell);
  
  TESCell_GetUnk = (_TESCell_GetUnk)FindSignature(&gMainModuleInfo,
    "\xF6\x45\x40\x01\x74\x04\x33\xFF\xEB",
    "xxxxxxxxx", -0x45);
  assert((uint64)TESCell_GetUnk != 0);
  
  TESDisplayMessage = (_TESDisplayMessage)FindSignature(&gMainModuleInfo,
    "\x48\x83\xEC\x50\xC6\x44\x24\x28\x00\x44\x88\x4C\x24\x20",
    "xxxxxxxxxxxxxx", -0x2);
  assert((uint64)TESDisplayMessage != 0);
  
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

  //assert((uint64)TESForm_Constructor != 0);
  //assert((uint64)TESGlobalScript_Compile != 0);
  
#ifdef F4_VERSION_1_10_40
  assert((uint64)TESCell_GetUnk - (uint64)gMainModule == 0x003B4A30); //1_10_40
  assert((uint64)TESDisplayMessage - (uint64)gMainModule == 0x00AE1D10); //1_10_40
#endif
}

#endif
