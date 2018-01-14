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
  struct ArgsType {
    char *type; // 8 bytes
    int32 unk01; // 4 bytes
    int8 unk02; // 1 byte
      int8 pad[3]; // 3 bytes
  }; // 16 bytes
  
  struct Struct_4 {
    int32 unk0;
    char string[512];
    int32 length;
    // ???
  };
  
  struct TESString {
    char *string;
    uint16 length;
    uint16 length_zero_terminated;
  };
*/

#pragma pack(push, 1)
struct TESCell {
  void *vtable;
  uint64 unk08;
  uint64 unk10;
  uint64 unk18;
  uint64 unk20;
  uint64 unk28;
  uint64 unk30;
  uint64 unk38;
  uint16 flags;
  //???
};
#pragma pack(pop)

#pragma pack(push, 1)
struct TESObjectReference {
  void *vtable;
  uint64 unk08;
  uint64 unk10;
  uint16 unk18;
  uint16 unk1A;
  uint32 unk1C;
  uint64 unk20;
  uint16 unk28;
  uint16 unk2A;
  uint16 unk2C;
  uint16 unk2E;
  uint64 unk30;
  uint64 unk38;
  uint64 unk40;
  uint64 unk48;
  uint64 unk50;
  uint64 unk58;
  uint64 unk60;
  uint64 unk68;
  uint32 unk70;
  uint64 unk74;
  uint32 unk7C;
  uint64 unk80;
  uint64 unk88;
  uint64 unk90;
  uint64 unk98;
  uint64 unkA0;
  uint64 unkA8;
  uint64 unkB0;
  TESCell *parentCell;
  // ???
};
#pragma pack(pop)

#pragma pack(push, 1)
struct TESFunction {
  char *name; // 0x00
  char *synonim; // 0x08
  uint64 opcode; // 0x10
  char *description; // 0x18
  uint16 unk20;
  uint16 unk22;
  uint32 pad24;
  char *argumentType;
  void *function; // 0x30
  void *standardCompileFunction; // 0x38
  void *unk40;
  void *unk48;
}; // 80 bytes (0x50)
#pragma pack(pop)

#pragma pack(push, 1)
struct TESScript { // struct_a2
  uint64 vtable; // 0x00
  uint64 unk08;
  uint16 flags10;
  uint16 unk12;
  uint16 unk14;
  uint16 unk16;
  uint16 unk18;
  uint8 byte1A;
  uint8 byte1B;
  uint32 unk1C;
  uint32 unk20;
  uint32 unk24;
  uint32 bytecodeLength; // 0x28
  uint32 unk2C;
  uint8 unk30;
  uint8 unk31;
  uint8 unk32;
    uint8 gap33;
  uint32 unk34;
  char *scriptText; // 0x38
  char *scriptBytecode; // 0x40
  uint64 unk48;
  uint32 unk50;
  uint32 unk54;
  uint64 unk58;
  uint64 reference01; // 0x60
  uint64 reference02; // 0x68
  uint64 unk70;
  uint64 unk78;
  char scriptLineText[260];
}; // 388 bytes (0x184)
#pragma pack(pop)
