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
FunctionPointer(Bool, IsEventPerforming, (), 0x42FB00);
FunctionPointer(int, DetectDyncolCollision, (NJS_VECTOR* pos, NJS_VECTOR* output, Rotation3* rotation, ColFlags flagstoignore, float detectionradius), 0x439620);
FunctionPointer(void, RedMountain_SetViewData_Clip, (int clipLevel), 0x6007E0);
VoidFunc(RedMountain_SetViewData, 0x600730);
DataArray(Color3, GlobalColorsLevel, 0x90BFE8, 43);
FunctionPointer(void, KillPlayer, (uint8_t ID), 0x440CD0);
VoidFunc(EnableTimeThing, 0x426030);
FunctionPointer(void, PlayVoiceCheckSetting, (int voice), 0x571990);
DataPointer(ObjectMaster*, Egg1ExplosionTask, 0x3C6C940);
DataPointer(float, BossHealth, 0x3C58158);
FunctionPointer(void, SetCameraBossStuff, (EntityData1* data, float y, float x, int rot), 0x4647C0);
ObjectFunc(LoadMusic_EventDelayed, 0x600890);
ObjectFunc(ObjectCrash, 0x5A95B0); // position is position, scale is direction, object is the model
FastcallFunctionPointer(void, DrawChunkModel_, (Sint32* a1, Sint16* a2), 0x7917F0);
FunctionPointer(void, SetPlayerPosition, (uint8_t pno, uint8_t frame, NJS_POINT3* pos, Angle3* ang), 0x441A40);

// Symbols
TaskFunc(EnemyMountainE104_execTgt, 0x605A90); // local name: execTgt
TaskFunc(EnemyMountainE104_dispTgt, 0x6056E0); // local name: dispTgt
TaskFunc(EnemyMountainE104_destTgt, 0x605650); // local name: destTgt
TaskFunc(EnemyMountainE104, 0x605B40);
DataArray(NJS_POINT3, pCamTgtLinePos, 0x24C8D00, 5);
VoidFunc(CCL_ClearSearch, 0x41B9D0);
FunctionPointer(c_colli_hit_info*, CCL_IsHitKindWithNumEx, (taskwk* twp, uint8_t kind), 0x41BF30);
FunctionPointer(taskwk*, CCL_IsHitKindWithNum, (taskwk* twp, uint8_t kind), 0x41C910); // Check if twp collides with a collision holding a specific "kind" identifier (see CCL_INFO), returns the object taskwk or 0
FunctionPointer(taskwk*, CCL_IsHitKindWithNum2, (taskwk* twp, uint8_t kind), 0x41C930); // Same as CCL_IsHitKindWithNum2 but with unnecessary safe checks.
FunctionPointer(taskwk*, CCL_IsHitPlayer, (taskwk* twp), 0x41CBC0); // Check if a player collides with twp, returns the player's taskwk or 0
FunctionPointer(c_colli_hit_info*, CCL_IsHitPlayerEx, (taskwk* twp), 0x41C6E0);
FunctionPointer(taskwk*, CCL_IsHitBullet, (taskwk* twp), 0x41CBE0); // Check if a projectile collides with twp, returns the projectile's taskwk or 0
FunctionPointer(c_colli_hit_info*, CCL_IsHitBulletEx, (taskwk* twp), 0x41C750);
TaskFunc(SetBroken, 0x46C0F0);   // Set flag that objects check with CheckBroken to not respawn
TaskFunc(SetNoRevive, 0x46C100); // Tell the game to not respawn the object upon restart
FunctionPointer(BOOL, CheckBroken, (task* tp), 0x46C110); // Check no respawn flag
TaskFunc(UniDestructor, 0x4E21D0); // Destructor task for every enemy, does nothing
TaskFunc(E102KillCursor, 0x4CEFE0); // Remove gamma target on task
FunctionPointer(BOOL, Knuckles_KakeraGame_Set_PutEme, (uint8_t emeid, NJS_POINT3* emepos), 0x477D90); // Obtain emerald

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