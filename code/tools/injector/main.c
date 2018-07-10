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

//TODO(adm244): use NtQuerySystemInformation method if available
//TODO(adm244): check if target process is 64-bit

#include <stdio.h>

#include <windows.h>
#include <tlhelp32.h>

typedef unsigned int bool;
#define false 0
#define true 1

#define PROCNAME_SIZE 255

typedef struct InjectInfo {
  char procname[PROCNAME_SIZE];
  char dllname[MAX_PATH];
} InjectInfo;

bool GetInjectInfo(int argc, char **argv, InjectInfo *info)
{
  if (argc == 3) {
    strcpy(info->procname, argv[1]);
    strcpy(info->dllname, argv[2]);
    return true;
  }
  
  return false;
}

DWORD FindProcessId(char *procname)
{
  DWORD pid = -1;
  HANDLE proc_snapshot;
  PROCESSENTRY32 proc_entry;
  
  proc_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (proc_snapshot == INVALID_HANDLE_VALUE) {
    printf("Failed to get a snapshot of running processes.\n");
    return -1;
  }
  
  proc_entry.dwSize = sizeof(PROCESSENTRY32);
  if (!Process32First(proc_snapshot, &proc_entry)) {
    printf("Failed to get information on first process.\n");
    CloseHandle(proc_snapshot);
    return -1;
  }
  
  do {
    if (strcmp(procname, proc_entry.szExeFile) == 0) {
      pid = proc_entry.th32ProcessID;
      break;
    }
  } while (Process32Next(proc_snapshot, &proc_entry));
  
  CloseHandle(proc_snapshot);
  return pid;
}

bool InjectDLL(InjectInfo *info)
{
  DWORD pid = FindProcessId(info->procname);
  if (pid != -1) {
    HANDLE process;
    DWORD accessflags = PROCESS_CREATE_THREAD | PROCESS_VM_WRITE | PROCESS_VM_OPERATION;
    HMODULE kernel32lib;
    void *loadlibrary_func;
    void *mem;
    int pathlength;
    
    process = OpenProcess(accessflags, 0, pid);
    if (!process) {
      printf("Cannot open process \"%s\" (PID %d)\n", info->procname, pid);
      return false;
    }
    
    kernel32lib = GetModuleHandle("kernel32.dll");
    loadlibrary_func = (void *)GetProcAddress(kernel32lib, "LoadLibraryA");
    if (!loadlibrary_func) {
      printf("Cannot get LoadLibraryA function address.\n");
      return false;
    }
    
    pathlength = strlen(info->dllname) + 1;
    mem = VirtualAllocEx(process, 0, pathlength, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!mem) {
      printf("Cannot allocate %d bytes of memory.", pathlength);
      return false;
    }
    
    if (!WriteProcessMemory(process, mem, info->dllname, pathlength, 0)) {
      printf("Cannot write to a process memory.\n");
      return false;
    }
    
    if (!CreateRemoteThread(process, 0, 0, loadlibrary_func, mem, 0, 0)) {
      printf("Cannot create a remote thread.\n");
      return false;
    }
    
    return true;
  } else {
    printf("Could not find process \"%s\"\n", info->procname);
  }
  
  return false;
}

int main(int argc, char** argv)
{
  InjectInfo info = {0};
  if (GetInjectInfo(argc, argv, &info)) {
    printf("procname: %s\n", info.procname);
    printf("dllname: %s\n", info.dllname);
    
    if (InjectDLL(&info)) {
      printf("%s is injected into %s!\n", info.dllname, info.procname);
    } else {
      printf("Could not inject %s into %s!\n", info.dllname, info.procname);
    }
  } else {
    printf("dll name and\\or proccess name were NOT specified!\n");
  }
  
  return 0;
}
