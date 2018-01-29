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

//NOTE(adm244): retrieves a folder path from full path
internal std::string GetDirectoryFromPath(std::string path)
{
  return path.substr(0, path.rfind("\\") + 1);
}

//NOTE(adm244): retrieves an integer value from specified section and value of ini file
internal int IniReadInt(HMODULE module, char *inifile, char *section, char *param, int def)
{
  char curdir[MAX_PATH];
  GetModuleFileNameA(module, curdir, sizeof(curdir));
  std::string fname = GetDirectoryFromPath(curdir) + inifile;
  
  return GetPrivateProfileIntA(section, param, def, fname.c_str());
}

//NOTE(adm244): retrieves a bool value from specified section and value of ini file
internal bool IniReadBool(HMODULE module, char *inifile, char *section, char *param, bool def)
{
  int value = IniReadInt(module, inifile, section, param, def ? 1 : 0);
  return(value != 0 ? true : false);
}

//NOTE(adm244): retrieves a string value from specified section and value of ini file and stores it in buffer
internal int IniReadString(HMODULE module, char *inifile, char *section, char *param, char *default, char *output, int size)
{
  char buffer[MAX_PATH];
  GetModuleFileNameA(module, buffer, sizeof(buffer));
  std::string fname = GetDirectoryFromPath(buffer) + inifile;

  return GetPrivateProfileStringA(section, param, default, output, size, fname.c_str());
}

//NOTE(adm244): retrieves all key-value pairs from specified section of ini file and stores it in buffer
internal DWORD IniReadSection(HMODULE module, char *inifile, char *section, char *buffer, DWORD bufsize)
{
  char curdir[MAX_PATH];
  GetModuleFileNameA(module, curdir, sizeof(curdir));
  std::string fname = GetDirectoryFromPath(curdir) + inifile;
  
  return GetPrivateProfileSectionA(section, buffer, bufsize, fname.c_str());
}

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
  uint16 keystate = (uint16)GetAsyncKeyState(key);
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
