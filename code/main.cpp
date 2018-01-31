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
  TODO:
    - Hook initialize function (clear DllMain)?
    - Save game function
  
    - Rewrite config file structure and implement new parser (read entire config data into a structure)
    
    - Get rid of C Runtime Library
    - Code cleanup
  DROPPED:
    none yet
*/

#include <stdio.h>
#include <string>
#include <windows.h>

#include "common/types.h"
#include "common/utils.cpp"
#include "common/queue.cpp"

#include "f4/types.h"

extern "C" {
  void GameLoop_Hook();
  void LoadGameBegin_Hook();
  void LoadGameEnd_Hook();
  
  uint64 GetConsoleObject();
  uint64 GetGlobalScriptObject();
  TESObjectReference * GetPlayerReference();
}

extern "C" uint64 baseAddress = 0;
internal HMODULE f4silver = 0;

internal HANDLE TimerQueue = 0;

internal bool IsInterior = 0;
internal bool ActualGameplay = false;

#include "f4/version.h"
#include "f4/functions.cpp"

//FIX(adm244): hack!
#define MAX_SECTION 32767
#define MAX_FILENAME 260
#define MAX_DESCRIPTION 255
#define MAX_BATCHES 50

#include "silverlib/config.cpp"
#include "silverlib/batch_processor.cpp"
#include "silverlib/random/functions.cpp"

internal bool Initialized = false;
internal HANDLE QueueHandle = 0;
internal DWORD QueueThreadID = 0;

internal Queue BatchQueue;
internal Queue InteriorPendingQueue;
internal Queue ExteriorPendingQueue;

internal uint8 IsTimedOut = 0;

internal void DisplayMessage(char *message)
{
  TESConsolePrint(GetConsoleObject(), message);
  TESDisplayMessage(message, 0, 1, true);
}

internal void DisplaySuccessMessage(char *message)
{
  if( Settings.ShowMessages ) {
    DisplayMessage(message);
  }
}

internal void DisplayRandomSuccessMessage(char *message)
{
  if( Settings.ShowMessagesRandom ) {
    DisplayMessage(message);
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

internal VOID CALLBACK TimerQueueCallback(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
  RandomClearCounters();
  
  IsTimedOut = 1;
}

internal void TimerCreate(HANDLE timerQueue, uint timeout)
{
  HANDLE newTimerQueue;
  if( !CreateTimerQueueTimer(&newTimerQueue, TimerQueue,
                            (WAITORTIMERCALLBACK)TimerQueueCallback,
                            0, timeout, 0, 0) ) {
    DisplayMessage("Ooops... Timer is dead :'(");
    DisplayMessage("We're sad pandas now :(");
  }
}

internal void ProcessQueue(Queue *queue, bool checkExecState)
{
  pointer dataPointer;
  
  while( dataPointer = QueueGet(queue) ) {
    BatchData *batch = (BatchData *)dataPointer;
    bool isQueueEmpty = QueueIsEmpty(queue);
    
    if( checkExecState ) {
      uint8 executionState = GetBatchExecState(batch->filename);
    
      bool executionStateValid = ((executionState == EXEC_EXTERIOR_ONLY) && !IsInterior)
        || ((executionState == EXEC_INTERIOR_ONLY) && IsInterior)
        || (executionState == EXEC_DEFAULT);
      
      if( !executionStateValid ) {
        if( executionState == EXEC_EXTERIOR_ONLY ) {
          QueuePut(&ExteriorPendingQueue, dataPointer);
        } else {
          QueuePut(&InteriorPendingQueue, dataPointer);
        }
        
        return;
      }
    }
    
    if( isQueueEmpty ) MakePreSave();
  
    ExecuteBatch(batch->filename);
    DisplaySuccessMessage(batch->description);
    
    if( isQueueEmpty ) MakePostSave();
  }
}

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
        int index = GetNextBatchIndex(batches_count);
        QueuePut(&BatchQueue, (pointer)&batches[index]);
        DisplayRandomSuccessMessage(batches[index].description);
        
        /*TESObjectReference *player = GetPlayerReference();
        if( IsInInterior(player) ) {
          DisplayMessage("Interior");
        } else {
          DisplayMessage("Exterior");
        }*/
      }
    }
  }
}

internal bool IsPlayerInInterior()
{
  return IsInInterior(GetPlayerReference());
}

extern "C" void GameLoop()
{
  if( !Initialized ) {
    QueueHandle = CreateThread(0, 0, &QueueHandler, 0, 0, &QueueThreadID);
    CloseHandle(QueueHandle);
    
    Initialized = true;
  }

  if( IsTimedOut ) {
    IsTimedOut = 0;
    TimerCreate(TimerQueue, Settings.Timeout);
  }
  
  if( ActualGameplay ) {
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

extern "C" void LoadGameBegin(char *filename)
{
  ActualGameplay = false;
}

extern "C" void LoadGameEnd()
{
  ActualGameplay = true;
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
  DefineAddresses();
  ShiftAddresses();
  
  SettingsInitialize(module);
  InitilizeBatches(module);
  
  int batchesCount = InitilizeBatches(module);
  if( batchesCount <= 0 ) {
    MessageBox(0, "Batch files could not be located!", "Error", MB_OK | MB_ICONERROR);
  }
  
  QueueInitialize(&BatchQueue);
  QueueInitialize(&InteriorPendingQueue);
  QueueInitialize(&ExteriorPendingQueue);
  //QueueHandle = CreateThread(0, 0, &QueueHandler, 0, 0, &QueueThreadID);
  
  RandomGeneratorInitialize(batchesCount);
  TimerQueue = CreateTimerQueue();
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
