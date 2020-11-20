#pragma once

static constexpr int MaxPlayers = 4;

extern HelperFunctions HelperFunctionsGlobal;

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