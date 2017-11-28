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

#ifndef _UTILS_CPP_
#define _UTILS_CPP_

internal void SafeReadBuf(uint64 addr, void *buffer, uint64 len)
{
  DWORD oldProtect;

  VirtualProtect((void *)addr, len, PAGE_EXECUTE_READWRITE, &oldProtect);
  memcpy(buffer, (void *)addr, len);
  VirtualProtect((void *)addr, len, oldProtect, &oldProtect);
}

internal void SafeWriteBuf(uint64 addr, void *data, uint64 len)
{
  DWORD oldProtect;

  if( VirtualProtect((void *)addr, len, PAGE_EXECUTE_READWRITE, &oldProtect) ) {
    memcpy((void *)addr, data, len);
    VirtualProtect((void *)addr, len, oldProtect, &oldProtect);
  } else {
    //TODO(adm244): cannot change page access, fatal!
  }
}

internal void WriteBranch(uint64 source, uint64 dest)
{
#define CODE_LENGTH 12
  uint8 code[CODE_LENGTH];
  
  code[0] = 0x48;
  code[1] = 0xB8;
  *((uint64 *)(&code[2])) = dest;
  
  code[10] = 0xFF;
  code[11] = 0xE0;
  
  SafeWriteBuf(source, code, CODE_LENGTH);
#undef CODE_LENGTH
}

//NOTE(adm244): returns whenever key is pressed or not
internal int GetKeyPressed(byte key)
{
  short keystate = (short)GetAsyncKeyState(key);
  return( (keystate & 0x8000) > 0 );
}

//NOTE(adm244): checks if key was pressed and locks its state
// returns true if key wasn't pressed in previous frame but it is in this one
// returns false if key is not pressed or is hold down
internal bool IsActivated(byte key, bool *enabled)
{
  if( GetKeyPressed(key) ){
    if( *enabled ){
      *enabled = false;
      return(true);
    }
  } else{
    *enabled = true;
  }
  return(false);
}

#endif
