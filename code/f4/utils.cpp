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

#ifndef _F4_UTILS_H_
#define _F4_UTILS_H_

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

#endif
