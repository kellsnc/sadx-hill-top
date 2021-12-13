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

DataPointer(VoidFuncPtr, LevelDestructor, 0x3B291B4);
FunctionPointer(Bool, IsEventPerforming, (), 0x42FB00);
FunctionPointer(int, DetectDyncolCollision, (NJS_VECTOR* pos, NJS_VECTOR* output, Rotation3* rotation, ColFlags flagstoignore, float detectionradius), 0x439620);
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
struct PL_KILLCOLLI
{
    int character;
    obj* object;
};

struct ___stcClip
{
    float Near;
    float Far;
};

struct __declspec(align(4)) ___stcFog
{
    float StartZ;
    float EndZ;
    unsigned int Col;
    unsigned __int8 Enable;
};

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
FunctionPointer(BOOL, Knuckles_KakeraGame_Set_CheckEme, (uint8_t emeid, NJS_POINT3* emepos), 0x476660); // Check emerald
FunctionPointer(void, StaticShadow, (taskwk* twp, float ypos), 0x49ED70);
FunctionPointer(float, Shadow, (taskwk* twp, float scl), 0x49EE30);
FunctionPointer(float, _DrawShadow, (Angle3* ang, NJS_POINT3* pos, float scl), 0x49EF30);
FunctionPointer(float, ShadowWithAng, (taskwk* twp, float scl, Angle3* ang), 0x49EFD0);
FunctionPointer(int, GetStageNumber, (), 0x414650);
FunctionPointer(void, CreateSmoke, (NJS_POINT3* pos, NJS_POINT3* velo, float scl), 0x4B9820);
DataArray(_OBJ_LANDTABLE*, objLandTable, 0x97DA08, 120);
DataArray(TaskFuncPtr, RoundMasterList, 0x90BF38, 44); // level tasks
DataArray(TaskFuncPtr, ScrollMasterList, 0x90C1F0, 44); // skybox tasks
DataArray(PL_KILLCOLLI**, KillingCollisionModelsListList, 0x915908, 43);
DataPointer(__int16, StageNumber, 0x3B22DCC);
DataPointer(int, ActNumber, 0x3B22DEC);
FunctionPointer(void, ___njSetBackColor, (uint32_t c0, uint32_t c1, uint32_t c2), 0x402F10);
VoidFunc(SetScrollTask, 0x414420);
VoidFunc(SetRoundMaster, 0x4143C0);
TaskFunc(mt_gdcontrol, 0x600890);
VoidFunc(ADX_Close, 0x425670); // Cut music
FunctionPointer(void, LandChangeStage, (char Gap), 0x43A460); // Release landtable and request act change
FunctionPointer(void, AddSetStage, (char Gap), 0x46BF70); // Release objects and request act change
FunctionPointer(void, AddCameraStage, (__int16 Gap), 0x434680); // Release cameras and request act change
FunctionPointer(void, AdvanceAct, (__int16 Gap), 0x415980); // Advance act number
FunctionPointer(void, ___njClipZ, (float nearZ, float farZ), 0x403180);
VoidFunc(___njFogEnable, 0x411AF0);
VoidFunc(___njFogDisable, 0x411B40);
DataPointer(___stcClip, gClipMap, 0x3ABDC70);
DataPointer(___stcClip, gClipSky, 0x3ABDCA0);
DataPointer(___stcFog, gFog, 0x3ABDC60);

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