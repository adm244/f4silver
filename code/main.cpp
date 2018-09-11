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
  
  int TES_GetWorldSpaceCount();
  TESWorldSpace ** TES_GetWorldSpaceArray();
}

extern "C" uint64 baseAddress = 0;
internal HMODULE f4silver = 0;

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
#include "silverlib/random/functions.c"
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
        
        int index = -1;
        //NOTE(adm244): temporary solution...
        do {
          index = RandomInt(0, batches_count - 1);
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

internal bool IsActivationPaused()
{
  return (Settings.IgnoreInDialogue && IsPlayerInDialogue())
      || (Settings.IgnoreInMenu && IsInMenuMode())
      || (Settings.IgnoreInVATS && IsMenuOpen("VATSMenu")
      || (Settings.IgnoreIfPlayerIsDead && IsActorDead((TESActor *)TES_GetPlayer())));
}

//#define SIGTEST

extern "C" void GameLoop()
{
#ifdef SIGTEST
  if (!Initialized) {
    MessageBoxA(0, "Injection is successfull!", "InjectDLL", MB_OK);
    
    Initialized = true;
  }
  
  if (IsActivated(&CommandRandom)) {
    /*bool isPlayerDead = IsActorDead((TESActor *)TES_GetPlayer());
    TESConsolePrint("Player is %s\n", isPlayerDead ? "dead" : "NOT dead");
    
    bool isVATSOpen = IsMenuOpen("VATSMenu");
    TESConsolePrint("VATS menu is %s\n", isVATSOpen ? "opened" : "closed");
    
    TES_ExecuteScriptLine("tgm");*/
    
    /*int value = 123;
    char *testStr = "Hello, World!";
    TESConsolePrint("This is a test message. %d %s", value, testStr);*/
    
    bool isConsoleOpen = IsMenuOpen("Console");
    TESConsolePrint("Console is %s", isConsoleOpen ? "opened" : "closed");
    
    //TODO(adm244): RE TESScript::Compile function and check if it works
    TES_ExecuteScriptLine("twf");
  }
#else
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
#endif
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
  //DefineAddresses();
  //ShiftAddresses();
  InitSignatures();
  
  SettingsInitialize(module);
  //NOTE(adm244): why are we initializing batches twice???
  //InitilizeBatches(module);
  
  int batchesCount = InitilizeBatches(module);
  if( batchesCount <= 0 ) {
    MessageBox(0, "Batch files could not be located!", "Error", MB_OK | MB_ICONERROR);
  }
  
  InitializeTimers();
  
  QueueInitialize(&BatchQueue);
  QueueInitialize(&InteriorPendingQueue);
  QueueInitialize(&ExteriorPendingQueue);
  //QueueHandle = CreateThread(0, 0, &QueueHandler, 0, 0, &QueueThreadID);
  
  //RandomGeneratorInitialize(batchesCount);
  RandomInitializeSeed(GetTickCount64());
  
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
