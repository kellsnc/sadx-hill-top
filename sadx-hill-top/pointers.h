#pragma once

static constexpr int MaxPlayers = 4;

extern HelperFunctions HelperFunctionsGlobal;

struct Color3 {
    unsigned int c1;
    unsigned int c2;
    unsigned int c3;
};

typedef void(__cdecl* VoidFuncPtr)(void);

DataPointer(VoidFuncPtr, LevelDestructor, 0x3B291B4);
FunctionPointer(void, DrawObject, (NJS_OBJECT*), 0x408530);
FunctionPointer(void, LoadEnemyExplosion, (Float x, Float y, Float z, Float scale), 0x4CAF80);
FunctionPointer(void, SpawnAnimal, (Uint32 unk, Float x, Float y, Float z), 0x4BE610);
FunctionPointer(bool, Object_CheckEmerald, (unsigned __int8 emerald, NJS_VECTOR* pos), 0x476660);
FunctionPointer(void, ObjectData2_SetPreviousPosition, (EntityData1* a1, ObjectData2* a2), 0x4CD390);
FunctionPointer(bool, SetEmeraldObtained, (unsigned __int8 emerald, NJS_VECTOR* pos), 0x477D90);
FunctionPointer(Float, ObjectData2_GetDistFromStartPos, (EntityData1* a1, ObjectData2* a2), 0x4CD510);
FunctionPointer(Float, GetGroundYPosition, (Float x, Float y, Float z, Rotation3* rot), 0x49E920);
FunctionPointer(Angle, ObjectData2_LookAtHome, (EntityData1* a1, ObjectData2* a2), 0x4CD5A0);
FunctionPointer(Angle, ObjectData2_LookAtPlayer, (EntityData1* a1, ObjectData2* a2, Uint8 player), 0x4CD6F0);
FunctionPointer(Bool, IsEventPerforming, (), 0x42FB00);
FunctionPointer(int, GetClosestPlayerID, (NJS_VECTOR* pos), 0x441B70);
FunctionPointer(int, DetectDyncolCollision, (NJS_VECTOR* pos, NJS_VECTOR* output, Rotation3* rotation, ColFlags flagstoignore, float detectionradius), 0x439620);
FunctionPointer(void, RedMountain_SetViewData_Clip, (int clipLevel), 0x6007E0);
VoidFunc(RedMountain_SetViewData, 0x600730);
DataArray(Color3, GlobalColorsLevel, 0x90BFE8, 43);
FunctionPointer(void, KillPlayer, (uint8_t ID), 0x440CD0);
DataArray(NJS_VECTOR, GammaCamBounds, 0x24C8D00, 5);
FunctionPointer(void, DisplayBossName2, (const char* name, signed int width, signed int height, int a4), 0x4B36D0);

// Event related
VoidFunc(EV_InitCamera, 0x4303E0);
VoidFunc(EV_FreeCamera, 0x4304B0);
VoidFunc(EV_SetCurrentEvt, 0x42F630);
FunctionPointer(void, EV_AllocMem, (int size), 0x64FC80);
VoidFunc(EV_FreeMem, 0x64FCB0);
FunctionPointer(void, EV_LoadObject, (float x, float y, float z, Angle rotx, Angle roty, Angle rotz, float scalex, float scaley, float scalez, NJS_OBJECT* object, NJS_TEXLIST* texlist, int ObjectID), 0x6ECF20);
FunctionPointer(void, EV_FreeObject, (int ObjectID), 0x6ECB50);
FunctionPointer(void, EV_LoadMotion, (int ObjectID, NJS_ACTION* Action, int ActionID), 0x6ECAC0);
FunctionPointer(void, EV_SetObjectThing, (int ObjectID, float a2, float a3, int a4), 0x6ECAE0);
FunctionPointer(void, EV_MoveObjectFromTo, (ObjectMaster* obj, float from_x, float from_y, float from_z, float to_x, float to_y, float to_z, signed int time), 0x6EC2B0);
FunctionPointer(ObjectMaster*, EV_GetObject, (int ObjectID), 0x6ECB40);
FunctionPointer(void, EV_PlaySoundQueue, (int idk, int ID, int PlayTime), 0x64FD00);
FunctionPointer(void, sub_64FD80, (int a1, int a2, int a3), 0x64FD80);
FunctionPointer(void, EventWait, (int time), 0x4314D0);
FunctionPointer(void, EV_SetPlayerPos, (ObjectMaster* obj, float x, float y, float z), 0x42FC70);
FunctionPointer(void, EV_RotatePlayer, (ObjectMaster* obj, Angle x, Angle y, Angle z), 0x42FCD0);
FunctionPointer(void, EV_MoveCamera, (char a1, int a2, float x, float y, float z), 0x430D00);
FunctionPointer(void, EV_RotateCamera, (char a1, int a2, Angle x, Angle y, Angle z), 0x430C90);
FunctionPointer(void, EV_ResetPlayer, (int playerID), 0x431780);
VoidFunc(EV_FreeControlObjects, 0x6EBB00);

enum class EVTSTATES : int {
    Init,
    Main,
    Free
};

ObjectFunc(LoadMusic_EventDelayed, 0x600890);
ObjectFunc(SecondaryObjectPhysics, 0x49D730);
ObjectFunc(SetObjectStatusHeld, 0x46C120);
ObjectFunc(ObjectCrash, 0x5A95B0); // position is position, scale is direction, object is the model
ObjectFunc(UpdateSetDataAndDelete, 0x46C150);
ObjectFunc(DeleteGammaMissileIfNoTarget, 0x4CEFE0);

FastcallFunctionPointer(void, DrawChunkModel_, (Sint32* a1, Sint16* a2), 0x7917F0);

static inline void SetObjectStatusNotHeld(ObjectMaster* obj) {
    obj->SETData.SETData->Flags &= ~0x70;
}

enum CollisionShapes {
    CollisionShape_Sphere,
    CollisionShape_Cylinder,
    CollisionShape_PushCylinder,
    CollisionShape_Cube,
    CollisionShape_Cube2,
    CollisionShape_Capsule,
    CollisionShape_Capsule2
};