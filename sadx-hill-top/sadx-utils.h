#pragma once

#define TASKWK_PLAYERID(a) a->counter.b[0]
#define TASKWK_CHARID(a) a->counter.b[1]

static constexpr int MaxPlayers = 4;

enum ObjectFlags {
    ObjFlag_StopDynCol = 0x1,
    ObjFlag_Held       = 0x1000
};

struct Color3 {
    unsigned int c1;
    unsigned int c2;
    unsigned int c3;
};

typedef void(__cdecl* VoidFuncPtr)(void);

DataPointer(VoidFuncPtr, LevelDestructor, 0x3B291B4);
FunctionPointer(bool, Object_CheckEmerald, (unsigned __int8 emerald, NJS_VECTOR* pos), 0x476660);
FunctionPointer(bool, SetEmeraldObtained, (unsigned __int8 emerald, NJS_VECTOR* pos), 0x477D90);
FunctionPointer(Bool, IsEventPerforming, (), 0x42FB00);
FunctionPointer(int, DetectDyncolCollision, (NJS_VECTOR* pos, NJS_VECTOR* output, Rotation3* rotation, ColFlags flagstoignore, float detectionradius), 0x439620);
FunctionPointer(void, RedMountain_SetViewData_Clip, (int clipLevel), 0x6007E0);
VoidFunc(RedMountain_SetViewData, 0x600730);
DataArray(Color3, GlobalColorsLevel, 0x90BFE8, 43);
FunctionPointer(void, KillPlayer, (uint8_t ID), 0x440CD0);
DataArray(NJS_VECTOR, GammaCamBounds, 0x24C8D00, 5);
VoidFunc(EnableTimeThing, 0x426030);
FunctionPointer(void, PlayVoiceCheckSetting, (int voice), 0x571990);
DataPointer(ObjectMaster*, Egg1ExplosionTask, 0x3C6C940);
DataPointer(float, BossHealth, 0x3C58158);
FunctionPointer(void, SetCameraBossStuff, (EntityData1* data, float y, float x, int rot), 0x4647C0);
ObjectFunc(LoadMusic_EventDelayed, 0x600890);
ObjectFunc(ObjectCrash, 0x5A95B0); // position is position, scale is direction, object is the model
ObjectFunc(DeleteGammaMissileIfNoTarget, 0x4CEFE0);
FastcallFunctionPointer(void, DrawChunkModel_, (Sint32* a1, Sint16* a2), 0x7917F0);
FunctionPointer(void, SetPlayerPosition, (uint8_t pno, uint8_t frame, NJS_POINT3* pos, Angle3* ang), 0x441A40);

// Event related
VoidFunc(EV_SetCurrentEvt, 0x42F630);
FunctionPointer(void, EV_AllocMem, (int size), 0x64FC80);
VoidFunc(EV_FreeMem, 0x64FCB0);
FunctionPointer(void, EventWait, (int time), 0x4314D0);

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