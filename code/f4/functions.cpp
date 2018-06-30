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
(BSFixedString *ptr, char *str);

typedef BSFixedString * (__fastcall *_BSFixedString_Set)
(BSFixedString *ptr, char *str);

typedef void (__fastcall *_BSFixedString_Release)(BSFixedString *ptr);

internal _BSFixedString_Constructor BSFixedString_Constructor;
internal _BSFixedString_Set BSFixedString_Set;
internal _BSFixedString_Release BSFixedString_Release;
// ------ #BSFixedString ------

// ------ TESUI ------
typedef bool (__fastcall *_TESUI_IsMenuOpen)(void *TESUIObject, BSFixedString *str);

internal _TESUI_IsMenuOpen TESUI_IsMenuOpen;
// ------ #TESUI ------

// ------ Utils ------
//NOTE(adm244): prints out a c-style formated string into the game console
typedef void (__fastcall *_TESConsolePrint)
(void *consoleObject, char *format, ...);

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

internal _TESConsolePrint TESConsolePrint;
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
  
  uint64 TESConsolePrintAddress;
  uint64 TESConsoleObjectAddress;
  
  uint64 TESUIObjectAddress;
  uint64 TESUIIsMenuOpenAddress;
  
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

    TESUIObjectAddress = 0x0590A918;
    TESUIIsMenuOpenAddress = 0x020420E0;
    
    BSFixedStringConstructorAddress = 0x01B416E0;
    BSFixedStringSetAddress = 0x01B41810;
    BSFixedStringReleaseAddress = 0x01B42970;
    
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
    
    TESObjectReferenceMoveToCellAddress = 0x00E9A330;
    TESObjectReferenceGetCurrentLocationAddress = 0x0040EE70;
    
    TESCellGetUnkAddress = 0x003B4A30;
    
    TESWorldSpaceFindExteriorCellByCoordinatesAddress = 0x004923E0;
    
    TESFindInteriorCellByNameAddress = 0x00152EB0;
    TESFindCellWorldSpaceByNameAddress = 0x00112FA0;
    TESDisplayMessageAddress = 0x00AE1D10;
    
    PlayerReferenceAddress = 0x05ADE398;
    
    GameDataAddress = 0x0590AB80;
    
    UnknownObject01Address = 0x05ADE288;
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
}
// ------ #Addresses ------

// ------ Functions ------
internal bool TES_ExecuteScriptLine(char *text)
{
  bool result = false;
  
  TESScript scriptObject = {0};
  
  TESScript_Constructor(&scriptObject);
  TESScript_MarkAsTemporary(&scriptObject);
  TESScript_SetText(&scriptObject, text);
  result = TESScript_CompileAndRun(&scriptObject, TES_GetGlobalScriptObject(), SysWindowCompileAndRun, 0);
  TESScript_Destructor(&scriptObject);
  
  return result;
}

internal inline bool TES_IsInterior(TESCell *cell)
{
  return cell->flags & FLAG_TESCell_IsInterior;
}

internal bool TES_IsInInterior(TESObjectReference *ref)
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

  BSFixedString bsString = {0};
  BSFixedString_Constructor(&bsString, str);
  result = TESUI_IsMenuOpen(*((void **)TESUIObjectAddress), &bsString);
  BSFixedString_Release(&bsString);
  
  return result;
}
// ------ #Functions ------

#endif
