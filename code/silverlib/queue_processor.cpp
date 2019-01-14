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

#ifndef _SILVERLIB_QUEUE_PROCESSOR_H
#define _SILVERLIB_QUEUE_PROCESSOR_H

struct Queues {
  Queue PrimaryQueue;
  Queue InteriorPendingQueue;
  Queue ExteriorPendingQueue;
};

internal Queues gQueues;

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
    
      bool executionStateValid = ((batch->executionState == EXEC_EXTERIOR_ONLY) && !gGameState.IsInterior)
        || ((batch->executionState == EXEC_INTERIOR_ONLY) && gGameState.IsInterior)
        || (batch->executionState == EXEC_DEFAULT);
      
      if( !executionStateValid ) {
        if( batch->executionState == EXEC_EXTERIOR_ONLY ) {
          QueuePut(&gQueues.ExteriorPendingQueue, dataPointer);
        } else {
          QueuePut(&gQueues.InteriorPendingQueue, dataPointer);
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
      if( gGameState.IsGameLoaded ) {
        DisplayMessage(keys_active ? Strings.MessageOn : Strings.MessageOff);
      }
    }
  
    if( keys_active ){
      for( int i = 0; i < batches_count; ++i ){
        if( IsActivated(batches[i].key, &batches[i].enabled) ){
          QueuePut(&gQueues.PrimaryQueue, (pointer)&batches[i]);
        }
      }
      
      if( IsActivated(&CommandRandom) ) {
        if (!BatchRandomSequence.initialized) {
          RandomInitializeSeed(&BatchRandomSequence, GetTickCount64());
        }
        
        //FIX(adm244): temporary solution...
        int index = -1;
        do {
          index = RandomInt(&BatchRandomSequence, 0, batches_count - 1);
        } while( batches[index].excludeRandom );
        
        QueuePut(&gQueues.PrimaryQueue, (pointer)&batches[index]);
        DisplayRandomSuccessMessage(batches[index].description);
      }
    }
  }
}

#endif
