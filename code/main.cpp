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

//IMPORTANT(adm244): patch only AFTER executable is decrypted!
// (just wait a bit?)

/*
  IMPLEMENTED:
    - Main game loop hook
    - Load screen hook
    - Print to console
    - Print in-game message
    - Execute console command
    - Execute commands from *.txt file line by line (bat command)
    - Check if player is in interior or exterior
    - Teleport command
    - Remove random counters clear timer
    - Implement @timeout command to delay batch lines execution
    - Check if player is in dialogue
    - Check if menu is opened
    - Check if vats is active
    - @excluderandom command to exclude batch from random
  TODO:
    - Add comments support to batch files
    - Rewrite hooking mechanism (detours)
    - Rewrite batch file structure (meta data + actual commands)
    - Draw game overlay
    - Reload configuration on demand
    - Save game when it's minimized
    
    - Hook initialize function (clear DllMain)?
    - Save game function
  
    - Rewrite config file structure and implement new parser (read entire config data into a structure)
    
    - Get rid of C Runtime Library
    - Code cleanup
  DROPPED:
    none yet
*/

#include <assert.h>
#include <stdio.h>
#include <string>
#include <windows.h>

#include "common/types.h"
#include "common/utils.cpp"
#include "common/queue.cpp"
#include "common/win32_timer.cpp"

#include "f4/types.h"

extern "C" {
  void GameLoop_Hook();
  void LoadGameBegin_Hook();
  void LoadGameEnd_Hook();
  void HackingPrepare_Hook();
  void HackingQuit_Hook();
}

extern "C" uint64 baseAddress = 0;
internal HMODULE f4silver = 0;

internal bool IsInterior = 0;
internal bool ActualGameplay = false;

#include "silverlib/random/functions.c"

#include "f4/version.h"
#include "f4/functions.cpp"

//FIX(adm244): hack!
#define MAX_SECTION 32767
#define MAX_FILENAME 260
#define MAX_DESCRIPTION 255
#define MAX_BATCHES 50

#include "silverlib/config.cpp"
#include "silverlib/batch_processor.cpp"

internal bool Initialized = false;
internal HANDLE QueueHandle = 0;
internal DWORD QueueThreadID = 0;

internal Queue BatchQueue;
internal Queue InteriorPendingQueue;
internal Queue ExteriorPendingQueue;

//#define FILE_DEADCOUNT "_deathcount.txt"
//TODO(adm244): store game state in a struct?
internal bool gIsPlayerDead = false;
//internal uint gDeathCount = 0;

internal inline void DisplayMessageDebug(char *message)
{
  if( Settings.ShowMessagesDebug ) {
    //TESConsolePrint(TES_GetConsoleObject(), message);
    TESConsolePrint(message);
  }
}

internal inline void DisplayMessage(char *message)
{
  TESDisplayMessage(message, 0, 1, true);
}

internal void DisplayMessage(char *message, char *commandName)
{
  char buffer[256];
  sprintf_s(buffer, 256, message, commandName);

  DisplayMessageDebug(buffer);
  TESDisplayMessage(buffer, 0, 1, true);
}

internal inline void DisplaySuccessMessage(char *commandName)
{
  if( Settings.ShowMessages ) {
    DisplayMessage(Strings.Message, commandName);
  }
}

internal inline void DisplayFailMessage(char *commandName)
{
  if( Settings.ShowMessages ) {
    DisplayMessage(Strings.MessageFail, commandName);
  }
}

internal inline void DisplayRandomSuccessMessage(char *commandName)
{
  if( Settings.ShowMessagesRandom ) {
    DisplayMessage(Strings.MessageRandom, commandName);
  }
}

internal inline void MakePreSave()
{
  if( Settings.SavePreActivation ) {
    //TODO(adm244): implement this!
    //SaveGame("PreActivation", "pre");
  }
}

internal inline void MakePostSave()
{
  if( Settings.SavePostActivation ) {
  //TODO(adm244): implement this!
    //SaveGame("PostActivation", "post");
  }
}

internal void ProcessQueue(Queue *queue, bool checkExecState)
{
  pointer dataPointer;
  
  while( dataPointer = QueueGet(queue) ) {
    BatchData *batch = (BatchData *)dataPointer;
    uint64 offset = batch->offset;
    bool isQueueEmpty = QueueIsEmpty(queue);
    
    //FIX(adm244): something wrong with a timer
    // test case: @timer used in @exterioronly batch file
    // activate batch in interior then go outside
    if( batch->timerIndex >= 0 ) {
      if( IsTimerStoped(batch->timerIndex) ) {
        FreeTimer(batch->timerIndex);
        
        batch->timerIndex = -1;
        batch->offset = 0;
      } else {
        QueuePut(queue, dataPointer);
        return;
      }
    }
    
    if( checkExecState ) {
      //uint8 executionState = GetBatchExecState(batch->filename);
    
      bool executionStateValid = ((batch->executionState == EXEC_EXTERIOR_ONLY) && !IsInterior)
        || ((batch->executionState == EXEC_INTERIOR_ONLY) && IsInterior)
        || (batch->executionState == EXEC_DEFAULT);
      
      if( !executionStateValid ) {
        if( batch->executionState == EXEC_EXTERIOR_ONLY ) {
          QueuePut(&ExteriorPendingQueue, dataPointer);
        } else {
          QueuePut(&InteriorPendingQueue, dataPointer);
        }
        
        return;
      }
    }
    
    if( isQueueEmpty ) MakePreSave();
  
    switch( ExecuteBatch(batch, offset) ) {
      case ExecuteBatch_Fail: {
        DisplayFailMessage(batch->description);
      } break;
      
      case ExecuteBatch_OnDelay: {
        DisplaySuccessMessage(batch->description);
        QueuePut(queue, dataPointer);
      } break;
      
      case ExecuteBatch_Success: {
        if( offset == 0 ) {
          DisplaySuccessMessage(batch->description);
        }
      } break;
    }
    
    if( isQueueEmpty ) MakePostSave();
  }
}

//TODO(adm244): better name
internal DWORD WINAPI QueueHandler(LPVOID data)
{
  for(;;) {
//internal void ProcessInput()
//{
    if( IsActivated(&CommandToggle) ) {
      keys_active = !keys_active;
      
      //TODO(adm244): display it somehow on loading screen
      if( ActualGameplay ) {
        DisplayMessage(keys_active ? Strings.MessageOn : Strings.MessageOff);
      }
    }
  
    if( keys_active ){
      for( int i = 0; i < batches_count; ++i ){
        if( IsActivated(batches[i].key, &batches[i].enabled) ){
          QueuePut(&BatchQueue, (pointer)&batches[i]);
        }
      }
      
      if( IsActivated(&CommandRandom) ) {
        /*BatchData *batch = GetRandomBatch(&randomBatchGroup);
        if( batch ) {
          QueuePut(&BatchQueue, (pointer)batch);
          DisplayRandomSuccessMessage(batch->description);
        } else {
          DisplayRandomFailureMessage();
        }*/
        
        if (!BatchRandomSequence.initialized) {
          RandomInitializeSeed(&BatchRandomSequence, GetTickCount64());
        }
        
        int index = -1;
        //NOTE(adm244): temporary solution...
        do {
          index = RandomInt(&BatchRandomSequence, 0, batches_count - 1);
        } while( batches[index].excludeRandom );
        
        QueuePut(&BatchQueue, (pointer)&batches[index]);
        DisplayRandomSuccessMessage(batches[index].description);
        
        /*if (IsActorDead((TESActor *)TES_GetPlayer())) {
          MessageBox(0, "Player is dead", "Info", MB_OK);
        } else {
          DisplayMessage("Player is NOT dead");
        }*/
      }
    }
  }
}

/*#define ENUM_ITEM(e) e,
#define _ENUM_FIRST(n) n ## FirstIndex
#define _ENUM_LAST(n) n ## LastIndex
#define ENUM(n, e) \
  enum n { \
    ENUM_ITEM(_ENUM_FIRST(n)) \
    e(ENUM_ITEM) \
    ENUM_ITEM(_ENUM_LAST(n)) \
  };
#define ENUM_FIRST(e) _ENUM_FIRST(e) + 1
#define ENUM_LAST(e) _ENUM_LAST(e) - 1

ENUM(TESMenus, TESMENUS)*/

//TODO(adm244): move it in a more appropriate place
#define SWITCH_STATE_NORMAL 0
#define SWITCH_STATE_CHANGED 1
#define SWITCH_STATE_WAIT 2
#define SKIP_FRAMES 30

internal bool IsActivationPaused()
{
  static uint8 switch_state = SWITCH_STATE_NORMAL;
  static bool state_before_change = false;
  static uint frames = 0;
  
  bool state = (Settings.IgnoreInDialogue && IsPlayerInDialogue())
      || (Settings.IgnoreInMenu && IsInMenuMode())
      || (Settings.IgnoreInVATS && IsMenuOpen("VATSMenu"))
      || (Settings.IgnoreInCooking && IsMenuOpen("CookingMenu"))
      || (Settings.IgnoreIfPlayerIsDead && IsActorDead((TESActor *)TES_GetPlayer()));

  switch (switch_state) {
    case SWITCH_STATE_NORMAL: {
      //NOTE(adm244): track state change only when leaving paused mode
      if ((!state) && (state != state_before_change)) {
        switch_state = SWITCH_STATE_CHANGED;
      } else {
        state_before_change = state;
      }
    } break;
    
    case SWITCH_STATE_CHANGED: {
      //DisplayMessage("CHANGED state");
      
      frames = 0;
      switch_state = SWITCH_STATE_WAIT;
    } break;
    
    case SWITCH_STATE_WAIT: {
      //DisplayMessage("WAIT state");
      
      if ((state == state_before_change) && (state_before_change)) {
        switch_state = SWITCH_STATE_NORMAL;
      } else {
        if (frames >= SKIP_FRAMES) {
          //DisplayMessage("back to NORMAL state");
          
          switch_state = SWITCH_STATE_NORMAL;
          state_before_change = state;
          return state;
        } else {
          ++frames;
        }
      }
    } break;
  }
  
  return state_before_change;
}

extern "C" void GameLoop()
{
  if( !Initialized ) {
    QueueHandle = CreateThread(0, 0, &QueueHandler, 0, 0, &QueueThreadID);
    CloseHandle(QueueHandle);
    
#ifdef DEBUG
    MessageBoxA(0, "Injection is successfull!", "InjectDLL", MB_OK);
#endif
    Initialized = true;
  }
  
  if( ActualGameplay ) {
    if (gIsPlayerDead != IsActorDead((TESActor *)TES_GetPlayer())) {
      gIsPlayerDead = !gIsPlayerDead;
      
      if (gIsPlayerDead) {
        //gDeathCount += 1;
        
        //FILE *deadCountFile = fopen(FILE_DEADCOUNT, "w");
        //if (deadCountFile) {
        //  fprintf(deadCountFile, "%d", gDeathCount);
        //  fclose(deadCountFile);
        //}
        
        //TODO(adm244): place into a separate function call?
        INPUT input = {0};
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = Keys.DeathEvent;
        input.ki.dwExtraInfo = GetMessageExtraInfo();
        
        SendInput(1, &input, sizeof(INPUT));
      }
    }
    
    if (!IsActivationPaused()) {
      if( IsInterior != IsPlayerInInterior() ) {
        IsInterior = !IsInterior;
        
        if( IsInterior ) {
          ProcessQueue(&InteriorPendingQueue, false);
        } else {
          ProcessQueue(&ExteriorPendingQueue, false);
        }
      }
      
      ProcessQueue(&BatchQueue, true);
    }
  }
}

extern "C" void LoadGameBegin(char *filename)
{
  ActualGameplay = false;
}

extern "C" void LoadGameEnd()
{
  ActualGameplay = true;
}

extern "C" void HackingPrepare()
{
  //TESConsolePrint("Terminal Hacking Entered");
  
  ExtraDataList *extrasList = (*gActiveTerminalREFR)->extraDataList;
  assert(extrasList != 0);
  
  ExtraLockData *lockData = ExtraDataList_GetExtraLockData(extrasList);
  assert(lockData != 0);
  
  //BSReadWriteLock_Lock(&extrasList->lock);
  
  //NOTE(adm244): unused flag, should be safe
  if (lockData->flags & 0x80) {
    //NOTE(adm244): using padding here, should be safe
    uint8 savedTries = lockData->pad01[0];
    if (savedTries == 0) {
      lockData->flags &= 0x7F;
    } else {
      *gTerminalTryCount = (int32)savedTries;
    }
  }
  
  //BSReadWriteLock_Unlock(&extrasList->lock);
}

extern "C" void HackingQuit()
{
  //TESConsolePrint("Terminal Hacking Quitted");
  
  ExtraDataList *extrasList = (*gActiveTerminalREFR)->extraDataList;
  assert(extrasList != 0);
  
  ExtraLockData *lockData = ExtraDataList_GetExtraLockData(extrasList);
  assert(lockData != 0);
  
  //FIX(adm244): locks game sometimes?
  //BSReadWriteLock_Lock(&extrasList->lock);
  
  lockData->flags |= 0x80;
  lockData->pad01[0] = (uint8)(*gTerminalTryCount);
  
  //BSReadWriteLock_Unlock(&extrasList->lock);
}

internal void HookMainLoop()
{
  WriteBranch(mainloop_hook_patch_address, (uint64)&GameLoop_Hook);
}

internal void HookLoadGame()
{
  WriteBranch(loadgame_start_hook_patch_address, (uint64)&LoadGameBegin_Hook);
  WriteBranch(loadgame_end_hook_patch_address, (uint64)&LoadGameEnd_Hook);
}

internal void Initialize(HMODULE module)
{
  //DefineAddresses();
  //ShiftAddresses();
  InitSignatures();
  
  SettingsInitialize(module);
  //NOTE(adm244): why are we initializing batches twice???
  //InitilizeBatches(module);
  
  int batchesCount = InitilizeBatches(module);
  if( batchesCount <= 0 ) {
    //FIX(adm244): MessageBox call in DllMain
    // https://docs.microsoft.com/en-us/windows/desktop/Dlls/dynamic-link-library-best-practices
    MessageBox(0, "Batch files could not be located!", "Error", MB_OK | MB_ICONERROR);
  }
  
  InitializeTimers();
  
  QueueInitialize(&BatchQueue);
  QueueInitialize(&InteriorPendingQueue);
  QueueInitialize(&ExteriorPendingQueue);
  //QueueHandle = CreateThread(0, 0, &QueueHandler, 0, 0, &QueueThreadID);
  
  //RandomGeneratorInitialize(batchesCount);
  RandomInitializeSeed(&DefaultRandomSequence, GetTickCount64());
  
  //FILE *deadCountFile = fopen(FILE_DEADCOUNT, "r");
  //if (deadCountFile) {
  //  fscanf(deadCountFile, "%d", &gDeathCount);
  //  fclose(deadCountFile);
  //}
}

internal DWORD WINAPI WaitForDecryption(LPVOID param)
{
  //48 8B 0D 2A D8 D8 04 E8  CD 1D 00 00
  /*uint8 orig_code[12] = { 0x48, 0x8B, 0x0D, 0x2A, 0xD8, 0xD8, 0x04, 0xE8, 0xCD, 0x1D, 0x00, 0x00 };
  uint8 cur_code[12];
  
  for (;;) {
    SafeReadBuf(mainloop_hook_patch_address, cur_code, ArraySize(cur_code));
    
    if( ArrayEquals(cur_code, ArraySize(cur_code), orig_code, ArraySize(orig_code)) )
    {
      break;
    }
    
    Sleep(1000);
  }
  
  HookMainLoop();*/
  
  Sleep(3000);
  
  HookMainLoop();
  //HookLoadGame();
  
  return 0;
}

internal BOOL WINAPI DllMain(HMODULE instance, DWORD reason, LPVOID reserved)
{
  if(reason == DLL_PROCESS_ATTACH) {
    f4silver = instance;
    
    Initialize(instance);
    
    HookMainLoop();
    HookLoadGame();
    
    //HANDLE decryptionThread = CreateThread(0, 0, WaitForDecryption, 0, 0, 0);
    //CloseHandle(decryptionThread);
  }

  return TRUE;
}
