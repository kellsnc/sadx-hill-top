#pragma once

extern HelperFunctions HelperFunctionsGlobal;

ObjectFunc(LoadMusic_EventDelayed, 0x600890);
FunctionPointer(Bool, IsEventPerforming, (), 0x42FB00);

enum CollisionShapes {
    CollisionShape_Sphere,
    CollisionShape_Cylinder,
    CollisionShape_PushCylinder,
    CollisionShape_Cube,
    CollisionShape_Cube2,
    CollisionShape_Capsule,
    CollisionShape_Capsule2
};