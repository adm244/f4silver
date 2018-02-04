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
  TODO:
    - add check for game exe! (creation kit also using this library)
    
    - get path to system32 folder from windows
    - optional warning message if f4silver.dll is not found
    - remove c++ string
    - compile without c-runtime Library
*/

#include <string>
#include <windows.h>
#include <x3daudio.h>

#define internal static
#define CONFIG_FILE "x3daudio.ini"
#define CONFIG_SECTION_PROXY "proxy"
#define CONFIG_KEY_PROXY "proxy_library"

internal HMODULE f4silver = 0;
internal HMODULE proxylib = 0;

// LIBRARY FUNCTIONS
typedef void __stdcall X3DAudioInitializeFunc(UINT32 SpeakerChannelMask, FLOAT32 SpeedOfSound, X3DAUDIO_HANDLE Instance);
typedef void __stdcall X3DAudioCalculateFunc(const X3DAUDIO_HANDLE Instance, const X3DAUDIO_LISTENER* pListener,
  const X3DAUDIO_EMITTER* pEmitter, UINT32 Flags, X3DAUDIO_DSP_SETTINGS* pDSPSettings);

extern "C"
void __stdcall Fake_X3DAudioInitialize(UINT32 SpeakerChannelMask, FLOAT32 SpeedOfSound, X3DAUDIO_HANDLE Instance)
{
  X3DAudioInitializeFunc *X3DAudioInitialize = (X3DAudioInitializeFunc *)GetProcAddress(proxylib, "X3DAudioInitialize");
  X3DAudioInitialize(SpeakerChannelMask, SpeedOfSound, Instance);
}

extern "C"
void __stdcall Fake_X3DAudioCalculate(const X3DAUDIO_HANDLE Instance, const X3DAUDIO_LISTENER* pListener,
  const X3DAUDIO_EMITTER* pEmitter, UINT32 Flags, X3DAUDIO_DSP_SETTINGS* pDSPSettings)
{
  X3DAudioCalculateFunc *X3DAudioCalculate = (X3DAudioCalculateFunc *)GetProcAddress(proxylib, "X3DAudioCalculate");
  X3DAudioCalculate(Instance, pListener, pEmitter, Flags, pDSPSettings);
}
// END

std::string GetDirectoryFromPath(std::string path)
{
  return path.substr(0, path.rfind("\\") + 1);
}

int IniReadString(char *inifile, char *section, char *param, char *default, char *output, int size)
{
  char buffer[MAX_PATH];
  GetModuleFileNameA(0, buffer, sizeof(buffer));
  std::string fname = GetDirectoryFromPath(buffer) + inifile;

  return GetPrivateProfileStringA(section, param, default, output, size, fname.c_str());
}

internal HMODULE HookLibrary(char *key)
{
  char libraryName[MAX_PATH];
  HMODULE library = 0;
  
  int result = IniReadString(CONFIG_FILE, CONFIG_SECTION_PROXY, key, 0, libraryName, sizeof(libraryName));
  
  if( result ) {
    library = LoadLibrary(libraryName);
  }
  
  return library;
}

internal BOOL WINAPI DllMain(HANDLE procHandle, DWORD reason, LPVOID reserved)
{
  if( reason == DLL_PROCESS_ATTACH )
  {
    proxylib = HookLibrary(CONFIG_KEY_PROXY);
    if( !proxylib ) {
      proxylib = LoadLibrary("c:\\windows\\system32\\X3DAudio1_7.dll");
    }
    
    f4silver = LoadLibrary("f4silver.dll");
    if( !f4silver ) {
      MessageBox(0, "Plugin f4silver.dll was not found!", "Plugin dll not found", MB_OK | MB_ICONERROR);
      return FALSE;
    } else {
      //MessageBox(0, "f4silver.dll is loaded!", "Yay!", 0);
    }
  }

  return TRUE;
}
