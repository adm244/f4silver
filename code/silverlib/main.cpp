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

#ifndef _SILVERLIB_MAIN_CPP_
#define _SILVERLIB_MAIN_CPP_

struct GameState {
  bool IsInterior;
  bool IsPlayerDead;
  bool IsGameLoaded;
};

internal GameState gGameState;

#include "silverlib/batch_processor.cpp"
#include "silverlib/queue_processor.cpp"
#include "silverlib/game_hooks.cpp"

internal void HookMainLoop()
{
  WriteBranch(mainloop_hook_patch_address, (uint64)&GameLoop_Hook);
}

internal void HookLoadGame()
{
  WriteBranch(loadgame_start_hook_patch_address, (uint64)&LoadGameBegin_Hook);
  WriteBranch(loadgame_end_hook_patch_address, (uint64)&LoadGameEnd_Hook);
}

internal void InitGameState()
{
  gGameState.IsInterior = false;
  gGameState.IsPlayerDead = false;
  gGameState.IsGameLoaded = false;
}

internal void InitQueueHandler()
{
  DWORD QueueThreadID = 0;
  HANDLE QueueHandle = CreateThread(0, 0, &QueueHandler, 0, 0, &QueueThreadID);
  CloseHandle(QueueHandle);
  
#ifdef DEBUG
  //FIX(adm244): MessageBox call in DllMain
  MessageBoxA(0, "Injection is successfull!", "InjectDLL", MB_OK);
#endif
}

internal void Initialize(HMODULE module)
{
  InitSignatures();
  
  InitGameState();
  SettingsInitialize(module);
  
  int batchesCount = InitilizeBatches(module);
  if( batchesCount <= 0 ) {
    //FIX(adm244): MessageBox call in DllMain
    // https://docs.microsoft.com/en-us/windows/desktop/Dlls/dynamic-link-library-best-practices
    MessageBox(0, "Batch files could not be located!", "Error", MB_OK | MB_ICONERROR);
  }
  
  InitializeTimers();
  
  QueueInitialize(&gQueues.PrimaryQueue);
  QueueInitialize(&gQueues.InteriorPendingQueue);
  QueueInitialize(&gQueues.ExteriorPendingQueue);
  
  RandomInitializeSeed(&DefaultRandomSequence, GetTickCount64());
  
  InitQueueHandler();
  
  HookMainLoop();
  HookLoadGame();
}

#endif
