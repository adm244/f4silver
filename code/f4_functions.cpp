/*
  Compile as x64 code
  
  Fallout 4 RE:
  (all addresses are relative to base)
    SaveGame: 0x00CDC4D0
    LoadGame: 0x00CDC9D0
    DeleteSaveGame: 0x00CEAF30
    
    GameDataReady: 0x0082F8D0
    
    FirstConsoleCommand: 0x03724DC0
    FirstObScriptCommand: 0x03714DD0
    
    Console Manager: 0x058FEEB0
    Console Pointer: 0x05AF97A8
    
    Load Game Event: 0x00442D20
    Init Script Event: 0x00442BE0
    Death Event: 0x004423C0
    Combat Event: 0x00441F60
    
    IsMenuOpen: 0x020406C0
    
    GameVM: 0x058F1708
  
  Print to console:
    this: 0x0586C4C0
    method: 0x012490E0
  
  CompileAndRun(?):
    address: 0x004EAE20
  
  Maximum script length 16384 bytes?
*/

/*
  ToggleSky command checks if player is in interior
  
  //this: 0x05A66918
  this: 
    rax = [0x005A669B0]
    r15 = [rax + 0xB8]
    [r15 - 0x38]
  
  unk1: 0x02C425A8
  scriptText: text of a script command
  unk3: flags(???) (seems to be 1)
  int CompileAndRun(void *this, void *unk1, char *scriptText, int32 *unk3)
    address: 0x004E98F0
  
  rdi + 0xE0
*/

/*
  struct X {
    uint64 ptrCommandName; // 0
    uint64 ptrCommandNameSynonym; // 8
    uint32 opcode; // 12
      uint8 pad[4]; // 16
    uint64 ptrCommandDescription; // 24
    
    uint8 unk4; // 25
      uint8 pad; // 26
    uint16 unk5; // 28
      uint8 pad[4]; // 32
    
    uint64 ptrArgumentType; // 40
    uint64 ptrCommandFunction; // 48
    uint64 ptrStandartCompileFunc; // 56
    uint64 ptrOtherFunc; // 64
      uint8 pad[8]; // 72
  }; // sizeof(X) = 72
*/

#ifndef _F4_FUNCTIONS_
#define _F4_FUNCTIONS_

//NOTE(adm244): prints out a c-style formated string into the game console
typedef void (__fastcall *_ConsolePrint)(uint64 obj, char *format, ...);
internal _ConsolePrint ConsolePrint;

#endif
