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
  DROPPED:
    - make buffer cyclic (put data before start pointer)
      (no point in doing so, our queue buffer won't be filling faster than it can be processed)
*/

#ifndef QUEUE_CPP
#define QUEUE_CPP

#define QUEUE_SIZE 10

struct Queue {
  pointer data[QUEUE_SIZE];
  
  int start;
  int end;
};

internal void QueueInitialize(Queue *queue)
{
  queue->start = 0;
  queue->end = 0;
}

internal void QueuePut(Queue *queue, pointer dataPointer)
{
  if( queue->end < QUEUE_SIZE ) {
    queue->data[queue->end++] = dataPointer;
  }
}

internal pointer QueueGet(Queue *queue)
{
  pointer result = 0;
  
  if( queue->start < queue->end ) {
    result = queue->data[queue->start++];
  } else {
    QueueInitialize(queue);
  }
  
  return result;
}

internal pointer QueuePeek(Queue *queue)
{
  return queue->data[queue->start];
}

internal int QueueGetLength(Queue *queue)
{
  return queue->end - queue->start;
}

internal int QueueIsEmpty(Queue *queue)
{
  return QueueGetLength(queue) < 1;
}

#endif
