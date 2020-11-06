#pragma once

static constexpr int MaxPlayers = 4;

extern HelperFunctions HelperFunctionsGlobal;

FunctionPointer(void, DrawObject, (NJS_OBJECT*), 0x408530);
ObjectFunc(LoadMusic_EventDelayed, 0x600890);
FunctionPointer(Bool, IsEventPerforming, (), 0x42FB00);
FastcallFunctionPointer(void, DrawChunkModel_, (Sint32* a1, Sint16* a2), 0x7917F0);

enum CollisionShapes {
    CollisionShape_Sphere,
    CollisionShape_Cylinder,
    CollisionShape_PushCylinder,
    CollisionShape_Cube,
    CollisionShape_Cube2,
    CollisionShape_Capsule,
    CollisionShape_Capsule2
};