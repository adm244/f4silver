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

#ifndef _SILVERLIB_GAME_HOOKS_CPP_
#define _SILVERLIB_GAME_HOOKS_CPP_

extern "C" void GameLoop()
{
  if( gGameState.IsGameLoaded ) {
    if (gGameState.IsPlayerDead != IsActorDead((TESActor *)TES_GetPlayer())) {
      gGameState.IsPlayerDead = !gGameState.IsPlayerDead;
      
      if (gGameState.IsPlayerDead) {
        //TODO(adm244): place into a separate function call?
        INPUT input = {0};
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = Keys.DeathEvent;
        input.ki.dwExtraInfo = GetMessageExtraInfo();
        
        SendInput(1, &input, sizeof(INPUT));
      }
    }
    
    if (!IsActivationPaused()) {
      if( gGameState.IsInterior != IsPlayerInInterior() ) {
        gGameState.IsInterior = !gGameState.IsInterior;
        
        if( gGameState.IsInterior ) {
          ProcessQueue(&gQueues.InteriorPendingQueue, false);
        } else {
          ProcessQueue(&gQueues.ExteriorPendingQueue, false);
        }
      }
      
      ProcessQueue(&gQueues.PrimaryQueue, true);
    }
  }
}

extern "C" void LoadGameBegin(char *filename)
{
  gGameState.IsGameLoaded = false;
}

extern "C" void LoadGameEnd()
{
  gGameState.IsGameLoaded = true;
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

#endif
