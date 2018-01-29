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

//TODO(adm244): assert structure sizes

/*
  RE notes:
    - TESScript_Compile function sets 0x02 bit for TESScript
*/

/*
  struct ArgsType {
    char *type; // 8 bytes
    int32 unk01; // 4 bytes
    int8 unk02; // 1 byte
      int8 pad[3]; // 3 bytes
  // }; // 16 bytes
  
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

enum FormTypes {
  FormType_Form = 0,
  FormType_Script = 0x16,
  FormType_ObjectReference = 0x40,
};

enum TESFlags {
  TESForm_Default = (1 << 3),
  TESForm_IsDeleted = (1 << 5),
  TESForm_IsPersistent = (1 << 16),
  TESForm_IsDisabled = (1 << 17),

  TESCell_IsInterior = (1 << 0),
  TESCell_HasWater = (1 << 1),
  
  TESScript_IsTemporary = (1 << 14),
};

#pragma pack(push, 1)
struct TESForm {
  void *vtable; // 0x00
  uint64 unk08;
  uint32 flags; // 0x10
  uint32 formId; // 0x14
  uint16 unk18;
  uint8 formType; // 0x1A
  uint8 byte1B;
  uint32 unk1C;
};
#pragma pack(pop)

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
  uint16 flags; // 0x40
  //???
};
#pragma pack(pop)

#pragma pack(push, 1)
struct TESObjectReference {
  // TESForm
  void *vtable; // 0x00
  uint64 unk08;
  uint32 flags; // 0x10
  uint32 formId; // 0x14
  uint16 unk18;
  uint8 formType; // 0x1A
  uint8 byte1B;
  uint32 unk1C;
  // #TESForm
  
  void *unk20;
  uint32 unk28;
  uint16 unk2C;
  uint16 unk2E;
  void *unk30;
  void *unk38;
  void *unk40;
  void *unk48;
  void *unk50;
  void *unk58;
  uint32 unk60;
  uint32 unk64;
  uint64 unk68;
  uint32 unk70;
  uint32 unk74;
  uint32 unk78;
  uint32 unk7C;
  uint64 unk80;
  uint64 unk88;
  uint64 unk90;
  uint64 unk98;
  uint64 unkA0;
  uint64 unkA8;
  uint8 unkB0;
  uint8 padB1[7];
  TESCell *parentCell; // 0xB8
  uint64 unkC0;
  uint64 unkC8;
  uint64 unkD0;
  uint64 unkD8;
  uint64 unkE0;
  uint64 unkE8;
  uint64 unkF0;
  uint64 unkF8;
  void *unk100;
  uint16 unk108;
  uint16 unk10A;
  uint32 pad10C;
}; // 272 bytes (0x110)
#pragma pack(pop)

#pragma pack(push, 1)
struct ObScriptParam {
  char *type; // 0x00
  uint32 typeId; // 0x08
  uint32 isOptional; // 0x10
};
#pragma pack(pop)

#pragma pack(push, 1)
struct ObScriptCommand {
  char *name; // 0x00
  char *synonim; // 0x08
  uint32 opcode; // 0x10
  uint32 pad14;
  char *description; // 0x18
  uint16 unk20;
  uint16 paramsCount; // 0x22
  uint32 pad24;
  ObScriptParam *params; // 0x28
  void *function; // 0x30
  void *standardCompileFunction; // 0x38
  void *unk40;
  uint32 flags; // 0x48
  uint32 pad4C;
}; // 80 bytes (0x50)
#pragma pack(pop)

#pragma pack(push, 1)
struct TESScript {
  // TESForm
  void *vtable; // 0x00
  uint64 unk08;
  uint32 flags; // 0x10
  uint32 formId; // 0x14
  uint16 unk18;
  uint8 formType; // 0x1A
  uint8 byte1B;
  uint32 unk1C;
  // #TESForm
  
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
  uint64 unk50;
  uint64 unk58;
  uint64 unk60;
  uint64 unk68;
  char scriptLineText[260]; // 0x70
  uint32 unk174;
}; // 376 bytes (0x178)
#pragma pack(pop)
