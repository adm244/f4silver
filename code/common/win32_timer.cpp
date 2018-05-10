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

#ifndef WIN32_TIMER_POOL_CPP
#define WIN32_TIMER_POOL_CPP

#define WIN32_TIMERS_COUNT 10

enum Win32_TimerFlags {
  Win32_Timer_IsFree = (1 << 0),
  Win32_Timer_TimedOut = (1 << 1),
};

struct Win32_Timer {
  HANDLE handle;
  void *data;
  uint32 flags;
};

internal Win32_Timer Win32_Timers[WIN32_TIMERS_COUNT];

void CALLBACK Win32_TimerCallback(PVOID param, BOOLEAN TimerOrWaitFired)
{
  Win32_Timer *timer = (Win32_Timer *)param;
  timer->flags |= Win32_Timer_TimedOut;
}

internal int Win32_GetFreeTimerIndex()
{
  int result = -1;
  
  for( int i = 0; i < WIN32_TIMERS_COUNT; ++i ) {
    if( Win32_Timers[i].flags & Win32_Timer_IsFree ) {
      result = i;
      break;
    }
  }
  
  return result;
}

internal int Win32_StartTimer(int timerIndex, int delay, void *data)
{
  if( timerIndex < 0 ) return -1;
  
  Win32_Timer *timer = &Win32_Timers[timerIndex];
  
  timer->data = data;
  //TODO(adm244): implement SetFlag, GetFlag, ClearFlag functions
  timer->flags &= (~Win32_Timer_IsFree);
  timer->flags &= (~Win32_Timer_TimedOut);

  if( !CreateTimerQueueTimer(&timer->handle, 0, Win32_TimerCallback, timer, delay, 0, WT_EXECUTEONLYONCE) ) {
    return -2;
  }
  
  return timerIndex;
}

internal void InitializeTimers()
{
  for( int i = 0; i < WIN32_TIMERS_COUNT; ++i ) {
    Win32_Timers[i].data = 0;
    Win32_Timers[i].flags = Win32_Timer_IsFree;
  }
}

internal inline int StartTimer(int delay, void *data)
{
  return Win32_StartTimer(Win32_GetFreeTimerIndex(), delay, data);
}

internal inline int StartTimer(int delay)
{
  return StartTimer(delay, 0);
}

internal inline bool IsTimerStoped(int index)
{
  return Win32_Timers[index].flags & Win32_Timer_TimedOut;
}

internal inline void FreeTimer(int index)
{
  Win32_Timers[index].flags |= Win32_Timer_IsFree;
}

#endif
