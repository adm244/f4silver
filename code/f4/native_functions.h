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

#ifndef _NATIVE_FUNCTIONS_H
#define _NATIVE_FUNCTIONS_H

/*typedef void (__fastcall *_TESForm_Constructor)(TESForm *tesForm);
internal _TESForm_Constructor TESForm_Constructor;

//FIX(adm244): GlobalScript(-object) is a temporary name until we figure out the real one
typedef bool (__fastcall *_TESGlobalScript_Compile)
(void *globalObject, TESScript *scriptObject, int32 unk02, int32 compilerTypeIndex);
internal _TESGlobalScript_Compile TESGlobalScript_Compile;*/

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

typedef bool (__fastcall *_TESActor_IsInCombat)
(TESActor *actor);

//internal _TESActor_IsDead TESActor_IsDead;
// ------ #TESActor ------

// ------ TESCell ------
typedef TESCellUnk * (__fastcall *_TESCell_GetUnk)
(TESCell *cell, bool flag);

internal _TESCell_GetUnk TESCell_GetUnk;
// ------ #TESCell ------

// ------ TESWorldSpace ------
typedef TESCell * (__fastcall *_TESWorldSpace_FindExteriorCell)
(TESWorldSpace *tesWorldSpace, unsigned int cellX, unsigned int cellY);
typedef TESEncounterZone * (__fastcall *_TESWorldSpace_GetEncounterZone)
(TESWorldSpace *tesWorldSpace);
typedef TESLocation * (__fastcall *_TESWorldSpace_GetLocation)
(TESWorldSpace *tesWorldSpace);

internal _TESWorldSpace_FindExteriorCell TESWorldSpace_FindExteriorCell;
internal _TESWorldSpace_GetEncounterZone TESWorldSpace_GetEncounterZone;
internal _TESWorldSpace_GetLocation TESWorldSpace_GetLocation;
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
typedef bool (__fastcall *_Native_IsMenuOpen)
(BSInputEventReceiver *inputEventReceiver, BSFixedString **str);

internal _Native_IsMenuOpen Native_IsMenuOpen;
// ------ #TESUI ------

// ------ ExtraDataList ------
typedef ExtraLockData * (__fastcall *_ExtraDataList_GetExtraLockData)(ExtraDataList *list);
typedef BSExtraData * (__fastcall *_ExtraDataList_Find)(ExtraDataList *list, uint8 extraType);

internal _ExtraDataList_GetExtraLockData ExtraDataList_GetExtraLockData;
internal _ExtraDataList_Find ExtraDataList_Find;
// ------ #ExtraDataList ------

// ------ BSReadWriteLock ------
typedef void (__fastcall *_BSReadWriteLock_Lock)(BSReadWriteLock *lock);
typedef void (__fastcall *_BSReadWriteLock_Unlock)(BSReadWriteLock *lock);

internal _BSReadWriteLock_Lock BSReadWriteLock_Lock;
internal _BSReadWriteLock_Unlock BSReadWriteLock_Unlock;
// ------ #BSReadWriteLock ------

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

//FIX(adm244): looks like this is actually FindFormByName
// (it returns BGSMusicType as well)
//NOTE(adm244): searches for an interior cell with specified cell name
// cellName - an interior cell name to be found
typedef TESCell * (__fastcall *_TESFindInteriorCellByName)
(char *cellName);

typedef TESWorldSpace * (__fastcall *_TESFindCellWorldSpaceByName)
(void *gameData, char *cellName, unsigned int *cellX, unsigned int *cellY);

internal _TESFillConsoleBackbufferVA TESFillConsoleBackbufferVA;
internal _TESDisplayMessage TESDisplayMessage;
internal _TESFindInteriorCellByName TESFindInteriorCellByName;
internal _TESFindCellWorldSpaceByName TESFindCellWorldSpaceByName;
// ------ #Utils ------

#endif
