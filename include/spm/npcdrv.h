#pragma once

#include <common.h>
#include <wii/types.h>

#define NPCTEMPLATE_MAX 435

typedef struct
{
    s32 id;
    const char * animName;
} NPCTribeAnimDef;

typedef struct
{
    u8 unknown_0x0[0x8 - 0x0];
    u32 unknown_0x8;
    u8 unknown_0xc[0x14 - 0xc];
    s32 tribeId;
    u8 unknown_0x18[0x68 - 0x18];
} NPCEnemyTemplate;

extern NPCEnemyTemplate npcEnemyTemplates[NPCTEMPLATE_MAX];

typedef struct
{
    u8 unknown_0x0[0x8 - 0x0];
    u32 flags_8;
    u8 unknown_0xc[0x44 - 0xc];
    struct
    {
        s32 m_nPoseId;
        u8 unknown_0x4[0xe0 - 0x4];
    } m_Anim;
    u8 unknown_0x124[0x2a0 - 0x124];
    Vec3 position;
    u8 unknown_0x2ac[0x3ac - 0x2ac];
    f32 unknown_0x3ac;
    u8 unknown_0x3b0[0x574 - 0x3b0];
    s32 dropItemId;
    u8 unknown_0x578[0x748 - 0x578];
} NPCEntry;

typedef struct
{
    u32 flag;
    s32 num;
    NPCEntry * entries;
    u8 unknown_0xc[0x728 - 0xc];
    u8 unknown_0x728[0x400];
    u8 unknown_0xb28[0xb48 - 0xb28];
} NPCWork;

void npcInit();
void npcMain();

NPCWork * npcGetWorkPtr();

NPCEntry * npcNameToPtr();

void func_801c3694(const char *, const char *);
void func_801c37e4(const char *);
void func_801c3848(const char *);
