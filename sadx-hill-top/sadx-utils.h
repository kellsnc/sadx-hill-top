#pragma once

#define TASKWK_PLAYERID(a) a->counter.b[0]
#define TASKWK_CHARID(a) a->counter.b[1]

static constexpr int MaxPlayers = 4;

enum ObjectFlags {
    ObjFlag_StopDynCol = 0x1,
    ObjFlag_Held       = 0x1000
};

struct Color3 {
    unsigned int c1, c2, c3;
};

typedef void(__cdecl* VoidFuncPtr)(void);

FunctionPointer(void, ReleaseModule, (unsigned int u32ModuleType), 0x4228F0);
DataPointer(VoidFuncPtr, ___epilogfunc, 0x3B291B4);
FunctionPointer(BOOL, EV_CheckCansel, (), 0x42FB00);
FunctionPointer(void, RedMountain_SetViewData_Clip, (int clipLevel), 0x6007E0);
VoidFunc(RedMountain_SetViewData, 0x600730);
DataArray(Color3, BackColorList, 0x90BFE8, 43);
FunctionPointer(void, KillPlayer, (uint8_t ID), 0x440CD0);
VoidFunc(EnableTimeThing, 0x426030);
FunctionPointer(void, PlayVoiceCheckSetting, (int voice), 0x571990);
DataPointer(ObjectMaster*, Egg1ExplosionTask, 0x3C6C940);
DataPointer(float, BossHealth, 0x3C58158);
FunctionPointer(void, SetCameraBossStuff, (EntityData1* data, float y, float x, int rot), 0x4647C0);
ObjectFunc(ObjectCrash, 0x5A95B0); // position is position, scale is direction, object is the model
FastcallFunctionPointer(void, DrawChunkModel_, (Sint32* a1, Sint16* a2), 0x7917F0);
FunctionPointer(void, SetPlayerPosition, (uint8_t pno, uint8_t frame, NJS_POINT3* pos, Angle3* ang), 0x441A40);

// Symbols
TaskFunc(EnemyMountainE104_execTgt, 0x605A90); // local name: execTgt
TaskFunc(EnemyMountainE104_dispTgt, 0x6056E0); // local name: dispTgt
TaskFunc(EnemyMountainE104_destTgt, 0x605650); // local name: destTgt
DataArray(NJS_POINT3, pCamTgtLinePos, 0x24C8D00, 5);
FunctionPointer(void, StaticShadow, (taskwk* twp, float ypos), 0x49ED70);
FunctionPointer(float, Shadow, (taskwk* twp, float scl), 0x49EE30);
FunctionPointer(float, _DrawShadow, (Angle3* ang, NJS_POINT3* pos, float scl), 0x49EF30);
FunctionPointer(float, ShadowWithAng, (taskwk* twp, float scl, Angle3* ang), 0x49EFD0);
FunctionPointer(int, MSetPositionWIgnoreAttribute, (NJS_POINT3* p, NJS_POINT3* v, Angle3* a, int attrmask, float r), 0x439620); // Move position "p" with direction "v" and angle "a"; outputs in "v" and "a"; handles intersection with geo collisions in radius "r" except those whose attributes match attrmask; returns bitmask (1: touched a collision, 2: touched a collision upside down).
FunctionPointer(BOOL, MSetPosition, (NJS_POINT3* p, NJS_POINT3* v, Angle3* a, float r), 0x43A030); // Move position "p" with direction "v" and angle "a"; outputs in "v" and "a"; handles intersection with any geometry collision in radius "r"; returns whether it touched a collision or not.
FunctionPointer(BOOL, MSetPositionIgnoreAttribute, (NJS_POINT3* p, NJS_POINT3* v, Angle3* a, int attrmask, float r), 0x43A060); // Move position "p" with direction "v" and angle "a"; outputs in "v" and "a"; handles intersection with geo collisions in radius "r" except those whose attributes match attrmask; returns whether it touched a collision or not.
FunctionPointer(void, ___dsSetPalette, (int no), 0x412420);
FunctionPointer(int, AdjustAngle, (__int16 ang0, unsigned __int16 ang1, int dang), 0x438350); // Slowly adjust ang0 to ang1 at dang speed
FunctionPointer(int, SubAngle, (int ang0, int ang1), 0x4383B0); // Difference
FunctionPointer(int, DiffAngle, (int ang0, int ang1), 0x4383D0); // Positive difference
FunctionPointer(int, VectorAngle, (NJS_POINT3* v1, NJS_POINT3* v2, NJS_POINT3* vn), 0x438410); // Calculate Y rotation between two points; "vn" is ground normal to reverse the angle if upside down (optional)
VoidFunc(EV_CanselOn, 0x42F630);
FunctionPointer(void, EventSe_Init, (int size), 0x64FC80);
VoidFunc(EventSe_Close, 0x64FCB0);
FunctionPointer(void, EV_Wait, (int time), 0x4314D0);
DataPointer(int, BlockMask, 0x3B36D48); // Mask to show/hide level geometry with blockbit

enum class EVTSTATES : int {
    Init,
    Main,
    Free
};

static inline void SetObjectStatusNotHeld(task* tp)
{
    tp->ocp->ssCondition &= ~0x4000u;
}

enum AnimDataProperties {
    AnimProp_NoAnimation,
    AnimProp_FirstFrame,
    AnimProp_Skip,
    AnimProp_Loop,
    AnimProp_OnceT,
    AnimProp_Once,
    AnimProp_ReverseT,
    AnimProp_Reverse,
    AnimProp_OnceCut,
    AnimProp_Input1,
    AnimProp_Input2,
    AnimProp_Input3,
    AnimProp_Custom
};