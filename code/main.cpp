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

#include <stdio.h>
#include <string>
#include <windows.h>

#include "common/types.h"
#include "common/utils.cpp"
#include "common/queue.cpp"

extern "C" {
  void GameLoop_Hook();
  //uint8 CompileAndRun(char *text);
  uint64 GetConsoleObject();
  uint64 GetGlobalScriptObject();
}

extern "C" uint64 baseAddress = 0;
internal HMODULE f4silver = 0;

internal HANDLE TimerQueue = 0;

internal uint8 IsInterior = 0;
internal uint8 ActualGameplay = 1;

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

internal HANDLE QueueHandle = 0;
internal DWORD QueueThreadID = 0;

internal Queue BatchQueue;
internal Queue InteriorPendingQueue;
internal Queue ExteriorPendingQueue;

internal uint8 IsTimedOut = 0;

//NOTE(adm244): addresses for hooks
extern "C" {
  uint64 mainloop_hook_patch_address;
  uint64 mainloop_hook_return_address;

  uint64 ProcessWindowAddress;
  uint64 Unk3ObjectAddress;
  
  uint64 ConsolePrintAddress;
  uint64 Unk2ObjectAddress;
  
  uint64 TESScriptConstructorAddress;
  
  uint64 TESScriptCompileAddress;
  uint64 GlobalScriptStateAddress;
  
  uint64 TESScriptExecuteAddress;
}

internal void DisplayMessage(char *message)
{
  //TODO(adm244): implement this!
  ConsolePrint(GetConsoleObject(), message);
}

internal void DisplaySuccessMessage(char *message)
{
  //TODO(adm244): implement this!
  DisplayMessage(message);
}

internal void DisplayRandomSuccessMessage(char *message)
{
  //TODO(adm244): implement this!
  DisplayMessage(message);
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
        //int index = RandomInt(0, batches_count - 1);
        int index = GetNextBatchIndex(batches_count);
        QueuePut(&BatchQueue, (pointer)&batches[index]);
        DisplayRandomSuccessMessage(batches[index].description);
      }
    }
  }
}

internal uint8 IsPlayerInInterior()
{
  //TODO(adm244): implement this!
  return 0;
}

extern "C" void GameLoop()
{
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

internal void HookMainLoop()
{
  WriteBranch(mainloop_hook_patch_address, (uint64)&GameLoop_Hook);
}

internal void DefineAddresses()
{
  if( F4_VERSION == F4_VERSION_1_10_40 ) {
    mainloop_hook_patch_address = 0x00D36707;
    mainloop_hook_return_address = 0x00D36713;

    ProcessWindowAddress = 0x00D384E0;
    Unk3ObjectAddress = 0x05ADE288;

    ConsolePrintAddress = 0x01262830;
    Unk2ObjectAddress = 0x0591AB30;

    TESScriptConstructorAddress = 0x00151E30;

    TESScriptCompileAddress = 0x004E7B30;
    GlobalScriptStateAddress = 0x05B15420;

    TESScriptExecuteAddress = 0x004E2460;
  } else if( F4_VERSION == F4_VERSION_1_10_26 ) {
    mainloop_hook_patch_address = 0x00D34DB7;
    mainloop_hook_return_address = 0x00D34DC3;

    ProcessWindowAddress = 0x00D36B90;
    Unk3ObjectAddress = 0x05AC25E8;
    
    ConsolePrintAddress = 0x01260EE0;
    Unk2ObjectAddress = 0x058FEEB0;
    
    TESScriptConstructorAddress = 0x00151E30;
    
    TESScriptCompileAddress = 0x004E7B10;
    GlobalScriptStateAddress = 0x05AF9720;
    
    TESScriptExecuteAddress = 0x004E2440;
  } else {
    //TODO(adm244): unsupported version
  }
}

internal void ShiftAddresses()
{
  baseAddress = (uint64)GetModuleHandle(0);
  
  mainloop_hook_patch_address += baseAddress;
  mainloop_hook_return_address += baseAddress;
  
  ProcessWindowAddress += baseAddress;
  Unk3ObjectAddress += baseAddress;
  
  ConsolePrint = (_ConsolePrint)(ConsolePrintAddress + baseAddress);
  Unk2ObjectAddress += baseAddress;
  
  TESScript_Constructor = (_TESScript_Constructor)(TESScriptConstructorAddress + baseAddress);
  
  TESScript_Compile = (_TESScript_Compile)(TESScriptCompileAddress + baseAddress);
  GlobalScriptStateAddress += baseAddress;
  
  TESScript_Execute = (_TESScript_Execute)(TESScriptExecuteAddress + baseAddress);
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
  QueueHandle = CreateThread(0, 0, &QueueHandler, 0, 0, &QueueThreadID);
  
  RandomGeneratorInitialize(batchesCount);
  TimerQueue = CreateTimerQueue();
}

/*internal bool enabled = true;
extern "C" void GameLoop()
{
  if( IsActivated(VK_HOME, &enabled) ) {
    ExecuteScriptLine("ToggleWireFrame");
    //CompileAndRun("ToggleWireFrame");
    ConsolePrint(GetConsoleObject(), "This is a test: %d", 10);
    //ConsolePrint(GetConsoleObject(), "This is a test");
  }
}*/

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
  
  return 0;
}

internal BOOL WINAPI DllMain(HMODULE instance, DWORD reason, LPVOID reserved)
{
  if(reason == DLL_PROCESS_ATTACH) {
    f4silver = instance;
    
    Initialize(instance);
    //HookMainLoop();
    
    CreateThread(0, 0, WaitForDecryption, 0, 0, 0);
    
    //MessageBox(0, "This is f4silver.dll speaking!", "Yay!", 0);
  }

  return TRUE;
}
