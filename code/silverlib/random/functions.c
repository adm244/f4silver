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

#ifndef RANDOM_FUNCTIONS_C
#define RANDOM_FUNCTIONS_C

#include "randomlib.c"

internal uint64 random_seed = 0;

/*internal int randomGenerated = 0;
internal uint8 randomCounters[MAX_BATCHES];

internal void RandomClearCounters()
{
  randomGenerated = 0;
  for( int i = 0; i < MAX_BATCHES; ++i ) {
    randomCounters[i] = 0;
  }
}

internal int GetNextBatchIndex(int batchesCount)
{
  if( randomGenerated >= batchesCount ) {
    RandomClearCounters();
  }
  
  int value;
  for( ;; ) {
    value = RandomInt(0, batchesCount - 1);
    if( randomCounters[value] == 0 ) break;
  }
  
  ++randomGenerated;
  randomCounters[value] = 1;
  
  return value;
}*/

internal void RandomInitializeSeed(uint64 seed)
{
  uint64 ij = seed % 31328;
  uint64 kj = ((seed >> 3) ^ seed) % 30081;
  
  random_seed = seed;
  RandomInitialize(ij, kj);
}

#endif