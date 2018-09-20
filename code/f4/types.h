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

/* Form types:
  NONE 0x00
  TES4 0x01
  GRUP 0x02
  GMST 0x03
  KYWD 0x04 BGSKeyword
  LCRT 0x05 BGSLocationRefType
  AACT 0x06 BGSAction
  TRNS 0x07 BGSTransform
  CMPO 0x08
  TXST 0x09
  MICN 0x0A
  GLOB 0x0B TESGlobalObject
  DMGT 0x0C BGSDamageType
  CLAS 0x0D
  FACT 0x0E
  HDPT 0x0F
  EYES 0x10
  RACE 0x11 TESRace
  SOUN 0x12 TESSound
  ASPC 0x13 TESAcousticSpace
  SKIL 0x14
  MGEF 0x15 EffectSetting(?)
  SCPT 0x16 Script
  LTEX 0x17 TESLandTexture
  ENCH 0x18
  SPEL 0x19
  SCRL 0x1A
  ACTI 0x1B TESActivator
  TACT 0x1C
  ARMO 0x1D TESArmor
  BOOK 0x1E
  CONT 0x1F
  DOOR 0x20
  INGR 0x21
  LIGH 0x22 TESLight
  MISC 0x23
  STAT 0x24
  SCOL 0x25
  MSTT 0x26
  GRAS 0x27
  TREE 0x28
  FLOR 0x29
  FURN 0x2A
  WEAP 0x2B TESWeapon
  AMMO 0x2C TESAmmo
  NPC_ 0x2D TESActor
  LVLN 0x2E
  KEYM 0x2F
  ALCH 0x30
  IDLM 0x31
  NOTE 0x32
  PROJ 0x33 TESProjectile
  HAZD 0x34 TESHazard
  BNDS 0x35
  SLGM 0x36
  TERM 0x37 BGSTerminal(?)
  LVLI 0x38 TESLeveledItem
  WTHR 0x39 TESWeather
  CLMT 0x3A TESClimate
  SPGD 0x3B BGSShaderParticleGeometryData
  RFCT 0x3C BGSReferenceEffect
  REGN 0x3D TESRegion
  NAVI 0x3E
  CELL 0x3F TESObjectCELL
  REFR 0x40 TESObjectREFR
  ACHR 0x41 Actor
  PMIS 0x42
  PARW 0x43
  PGRE 0x44
  PBEA 0x45
  PFLA 0x46
  PCON 0x47
  PBAR 0x48
  PHZD 0x49
  WRLD 0x4A TESWorldSpace
  LAND 0x4B TESObjectLAND
  NAVM 0x4C TESNavMesh
  TLOD 0x4D
  DIAL 0x4E TESDialog
  INFO 0x4F
  QUST 0x50 TESQuest
  IDLE 0x51
  PACK 0x52
  CSTY 0x53 TESCombatStyle
  LSCR 0x54
  LVSP 0x55
  ANIO 0x56 TESObjectANIO
  WATR 0x57 TESWaterForm
  EFSH 0x58 TESEffectShader
  TOFT 0x59
  EXPL 0x5A
  DEBR 0x5B BGSDebris
  IMGS 0x5C TESImageSpace
  IMAD 0x5D TESImageSpaceModifier
  FLST 0x5E
  PERK 0x5F TESPerk
  BPTD 0x60
  ADDN 0x61
  AVIF 0x62 TESActorValueInfo
  CAMS 0x63 BGSCameraShot
  CPTH 0x64 BGSCameraPath
  VTYP 0x65
  MATT 0x66
  IPCT 0x67
  IPDS 0x68
  ARMA 0x69
  ECZN 0x6A BGSEncounterZone
  LCTN 0x6B BGSLocation
  MESG 0x6C
  RGDL 0x6D
  DOBJ 0x6E BGSDefaultObjectManager
  DFOB 0x6F BGSDefaultObject
  LGTM 0x70 BGSLightingTemplate
  MUSC 0x71 BGSMusicType
  FSTP 0x72 BGSFootstep
  FSTS 0x73 BGSFootstepSet
  SMBN 0x74 BGSStoryManagerBranchNode
  SMQN 0x75
  SMEN 0x76
  DLBR 0x77
  MUST 0x78
  DLVW 0x79
  WOOP 0x7A
  SHOU 0x7B
  EQUP 0x7C
  RELA 0x7D
  SCEN 0x7E
  ASTP 0x7F
  OTFT 0x80 BGSOutfit
  ARTO 0x81
  MATO 0x82
  MOVT 0x83 BGSMovementType
  SNDR 0x84
  DUAL 0x85
  SNCT 0x86
  SOPM 0x87
  COLL 0x88 BGSCollisionLayer
  CLFM 0x89 BGSColorForm
  REVB 0x8A
  PKIN 0x8B BGSPackIn
  RFGP 0x8C BGSReferenceGroup
  AMDL 0x8D BGSAimModel
  LAYR 0x8E
  COBJ 0x8F BGSConstructibleObject
  OMOD 0x90 Mod
  MSWP 0x91 BGSMaterialSwap
  ZOOM 0x92 BGSZoomData
  INNR 0x93 BGSInstanceNamingRules
  KSSM 0x94 BGSSoundKeywordMapping
  AECH 0x95 BGSAudioEffectChain
  SCCO 0x96
  AORU 0x97 BGSAttractionRule
  SCSN 0x98 BGSSoundCategorySnapshot
  STAG 0x99 BGSSoundTagSet
  NOCM 0x9A NavMeshObstacleCoverManager
  LENS 0x9B BGSLensFlare
  LSPR 0x9C
  GDRY 0x9D BGSGodRays
  OVIS 0x9E BGSObjectVisibilityManager
*/

#ifndef _TYPES_H
#define _TYPES_H

enum FormTypes {
  FormType_Form = 0,
  FormType_Script = 0x16,
  FormType_Weather = 0x39,
  FormType_Climate = 0x3A,
  FormType_Cell = 0x3F,
  FormType_ObjectReference = 0x40,
  FormType_Actor = 0x41,
  FormType_WorldSpace = 0x4A,
  FormType_WaterType = 0x57,
  FormType_ActorValueInfo = 0x62,
  FormType_Location = 0x6B,
  FormType_MusicType = 0x71,
};

enum TESFlags {
  // (1 << 0) = ???
  FLAG_TESForm_Default = (1 << 3),
  FLAG_TESForm_IsDeleted = (1 << 5),
  FLAG_TESForm_IsBorderRegion = (1 << 6),
  // (1 << 13) = ???
  // (1 << 15) = ???
  FLAG_TESForm_IsPersistent = (1 << 16),
  FLAG_TESForm_IsDisabled = (1 << 17),
  // (1 << 18) = ???

  FLAG_TESCell_IsInterior = (1 << 0),
  FLAG_TESCell_HasWater = (1 << 1),
  
  FLAG_TESScript_IsTemporary = (1 << 14),
  
  FLAG_TESActor_IsInDialogue = (1 << 3),
};

#pragma pack(push, 1)

struct BSInputEventReceiver {
  void *vtable;
};

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

struct TESActorValueInfo {
  TESForm tesForm;
  
  //???
};

struct TESWeather {
  TESForm tesForm;

  //???
};

#pragma warning(push)
#pragma warning(disable : 4200)
//struct BSFixedStringData {
struct BSFixedString {
  void *unk00;
  uint16 unk08;
  uint16 unk0A;
  uint32 unk0C;
  uint32 strLength; // 0x10
  uint32 unk14;
  char str[0]; // 0x18
};
#pragma warning(pop)

/*struct BSFixedString {
  BSFixedStringData *data; // 0x0
};*/

struct TESTextureDataObject {
  void *unk00;
  BSFixedString *textureName;
};

struct TESWeatherType {
  TESWeather *weather;
  uint64 chance;
  void *globalVariable; // named 'Global' in editor
};

struct LinkedListSingle {
  void *item;
  void *next;
};

struct TESClimate {
  TESForm tesForm;
  
  TESTextureDataObject nightSky; // 0x20
  uint64 unk30;
  uint64 unk38;
  uint64 unk40;
  uint64 unk48;
  TESWeatherType *defaultWeather; //??? 0x50
  LinkedListSingle *weatherTypes; // 0x58
  TESTextureDataObject sunTexture; // 0x60
  TESTextureDataObject sunGlareTexture; // 0x70
  uint8 sunriseBegin; // 0x80
  uint8 sunriseEnd; // 0x81
  uint8 sunsetBegin; // 0x82
  uint8 sunsetEnd; // 0x83
  uint8 volatility; // 0x84
  uint8 unk85;
  uint16 unk86;
  
  //???
};

struct TESWaterType {
  TESForm testForm;
  
  //???
};

struct BGSMusicType {
  TESForm tesForm;
  
  //???
};

struct TESLocationRefType {
  TESForm tesForm;
  
  
}; // 80 bytes (0x50)

struct TESReferenceList {
  void *reference; // 0x00
  uint32 editorID; // 0x08
  uint32 size; // 0x0C
  uint32 unk10;
  uint32 unk14;
}; // 24 bytes (0x18)

struct TESEncounterZone {
  TESForm tesForm;
  
  //???
};

struct TESRegion_Unk01 {
  void *unk00;
  uint64 unk08;
  uint32 flag; // 0x10
  uint32 pad14;
}; // 24 bytes (0x18)

struct TESRegion_Unk02 {
  uint64 unk00;
  uint64 unk08;
}; // 16 bytes (0x10)

struct TESWorldSpace;

struct TESRegion {
  TESForm tesForm;
  
  TESRegion_Unk01 *unk20;
  TESRegion_Unk02 *unk28;
  TESWorldSpace *parentWorldSpace; // 0x30
  TESWeather *unk38;
  uint64 unk40;
  uint32 unk48;
  uint32 unk4C;
  float unk50;
  uint32 unk54;
}; // 88 bytes (0x58)

struct RegionUnk {
  TESRegion *region;
  RegionUnk *nextRegionUnk;
  
  // ???
};

struct TESCellUnk {
  void *unk00;
  TESRegion *region;
  RegionUnk *regionUnk;
  
  // ???
};

struct TESLocation {
  TESForm tesForm;
  
  void *unk20;
  void *unk28;
  void *unk30;
  void *unk38;
  void *unk40; // keywords???
  uint32 unk48;
  uint32 unk4C;
  TESLocation *parent; // 0x50
  uint64 unk58;
  uint64 unk60;
  TESEncounterZone *encounterZone; // 0x68
  uint64 unk70;
  real32 unk78; // actorFadeMult???
  uint32 unk7C;
  TESReferenceList *locationRefsList; // 0x80
  
  //???
}; // 320 bytes (0x140)

struct TESCell;

struct TESWorldSpace {
  TESForm tesForm;
  
  void *unk20;
  void *unk28;
  void *unk30;
  void *unk38;
  TESActorValueInfo *unk40;
  uint32 unk48;
  uint32 unk4C;
  uint32 unk50;
  uint32 unk54;
  void *unk58;
  TESActorValueInfo *unk60;
  void *unk68;
  TESCell *unk70; // center of worldspace???
  uint32 unk78; // array of TESKeyworld??? (void **)
  uint32 unk7C;
  TESClimate *climate; // 0x80
  uint32 flags; // 0x88
  uint32 unk8C;
  uint32 unk90;
  uint32 unk94;
  uint32 unk98;
  uint32 unk9C;
  uint32 unkA0;
  uint32 unkA4;
  uint32 unkA8;
  uint32 unkAC;
  void *unkB0;
  uint32 unkB8;
  uint32 unkBC;
  uint64 unkC0;
  void *unkC8; // array of TESObjectReference, Character, ...
  uint32 unkD0;
  uint32 unkD4;
  uint32 unkD8;
  uint32 unkDC;
  TESActorValueInfo *unkE0;
  uint32 unkE8;
  uint32 unkEC;
  uint64 unkF0;
  void *unkF8;
  TESActorValueInfo *unk100;
  uint64 unk108;
  TESCell *unk110;
  TESActorValueInfo *unk118;
  uint32 unk120;
  uint32 unk124;
  uint32 unk128;
  uint32 unk12C;
  void *unk130;
  uint32 unk138;
  uint32 unk13C;
  void *unk140;
  void *unk148; // array of TESObjectReference
  uint32 unk150;
  uint32 unk154;
  uint32 unk158;
  uint32 unk15C;
  void *unk160;
  void *unk168;
  void *unk170;
  void *unk178;
  void *unk180;
  TESWorldSpace *parentWorldSpace; // 0x188
  uint64 unk190;
  TESWaterType *unk198;
  TESWaterType *unk1A0;
  real32 unk1A8;
  uint32 unk1AC;
  uint32 unk1B0;
  int16 NWCellX; // 1B4
  int16 NWCellY; // 1B6
  int16 SECellX; // 1B8
  int16 SECellY; // 1BA
  real32 unk1BC;
  uint32 unk1C0;
  uint32 unk1C4;
  uint32 unk1C8;
  uint32 unk1CC;
  BGSMusicType *musicType; // 0x1D0
  real32 unk1D8;
  real32 unk1DC;
  real32 unk1E0;
  real32 unk1E4;
  void *unk1E8;
  uint32 unk1F0;
  uint32 unk1F4;
  void *unk1F8;
  uint32 unk200;
  uint32 unk204;
  char *editorID; // 0x208
  uint16 unk210;
  uint16 unk212;
  uint16 unk214;
  uint16 unk216;
  real32 landHeight; // 0x218
  real32 waterHeight; // 0x21C
  real32 unk220;
  uint32 unk224;
  TESEncounterZone *encounterZone; // 0x228
  TESLocation *location; // 0x230
  
  //0x2C0 - array of "cells data" (which can be imported\exported in creation kit)
  
  //???
};

struct TESLand {
  TESForm tesForm;
  
  //???
};

struct TESLightingTemplate {
  TESForm tesForm;
  
  //???
};

struct TESCellCoords {
  int32 x;
  int32 y;
};

struct TESCell {
  TESForm tesForm;
  
  uint64 unk20;
  uint64 unk28;
  uint64 unk30;
  uint64 unk38;
  uint32 flags; // 0x40
  uint32 unk44;
  void *unk48;
  TESCellCoords *coordinates; // 0x50
  TESLand *landData; // 0x58
  uint32 unk60;
  uint32 unk64;
  void *unk68;
  void *unk70; // ObjectReference array???
  uint32 unk78; // array size???
  uint32 unk7C;
  uint32 unk80; // array size???
  uint32 unk84;
  uint64 unk88;
  void *unk90;
  
  uint8 unk98[48];
  
  TESWorldSpace *worldSpace; // 0xC8
  void *unkD0;
  TESLightingTemplate *lightningTemplate; // 0xD8
  //???
};

struct Vector3 {
  float x; // 0x0
  float y; // 0x4
  float z; // 0x8
}; // 13 bytes (0xC)

struct TESObjectReference {
  TESForm tesForm;
  
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
  Vector3 rotation; // 0xC0
  uint8 padCC[4];
  Vector3 position; // 0xD0
  uint8 padDC[4];
  TESForm *baseForm; // E0
  uint64 unkE8;
  uint64 unkF0;
  uint64 unkF8;
  void *unk100;
  uint16 unk108;
  uint16 unk10A;
  uint32 pad10C;
}; // 272 bytes (0x110)

struct TESActor {
  TESObjectReference objectReference;
  
  uint8 unk110[32];
  uint32 flags; // 0x130
  uint8 unk134[0x300 - 0x134];
  uint64 unk300;
  uint8 unk308[0x43C - 0x308];
  uint32 flags_2; // 0x43C
  uint32 flags_3; // 0x440
  uint8 unk444[0x490 - 0x444];
}; // 1168 bytes (0x490)

struct TESPlayer {
  TESActor tesActor;
  
  uint8 unk490[0x700 - 0x490];
  Vector3 previousPosition; // 0x700
  uint8 unk70C[0x8E8 - 0x70C];
  TESWorldSpace *currentWorldSpace; // 0x8E8
  TESCell *currentCell; // 0x8F0
  uint64 unk8F8;
  Vector3 newPosition; // 0x900
  Vector3 newRotation; // 0x90C
  
  //TODO(adm244): double check that...
  //uint32 unk910;
  //uint32 unk914;
  
  uint64 unk918;
  uint64 unk920;
  uint64 unk928;
  uint32 unk930;
  uint32 unk934;
  uint32 unk938;
  uint8 unk93C;
  uint8 unk93D;
  uint8 unk93E;
  uint8 unk93F;
  uint8 unk940[0xCC8 - 0x940];
  TESLocation *location; // 0xCC8
  
  //0x0DFF, 0x0DFC - some kind of flags used in code since oblivion (deprecated?)
  
  //???
}; // 3600 bytes (0xE10)

struct ObScriptParam {
  char *type; // 0x00
  uint32 typeId; // 0x08
  uint32 isOptional; // 0x10
};

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

struct TESScript {
  TESForm tesForm;
  
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

#endif
