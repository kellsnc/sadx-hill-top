#include "pch.h"
#include "o_hilltree.h"

ModelInfo* ht_tree = nullptr;

CollisionData HillTree_Col[] = {
	{ 0, CollisionShape_Capsule2, 0x77, 0, 0, {0, 35.0f, 0}, 5.0f, 35.0f, 0, 0, 0, 0, 0 },
	{ 0, CollisionShape_Capsule2, 0x77, 0, 0, {0, 25.0f, 0}, 5.0f, 25.0f, 0, 0, 0, 0, 0 }
};

void __cdecl HillTree_Display(ObjectMaster* obj) {
	if (!MissedFrames) {
		EntityData1* data = obj->Data1;

		njSetTexture(LevelObjTexlists[1]);
		njPushMatrixEx();
		njTranslateEx(&data->Position);
		njRotateEx((Angle*)&data->Rotation, false);
		njScale(nullptr, data->Scale.x, data->Scale.x + data->Scale.y, data->Scale.x);
		DrawObject(data->Object);
		njPopMatrixEx();
	}
}

void __cdecl HillTree_Main(ObjectMaster* obj) {
	if (!ClipSetObject(obj)) {
		EntityData1* data = obj->Data1;
		
		AddToCollisionList(data);
		obj->DisplaySub(obj);
	}
}

void __cdecl HillTree(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;

	// Choose the model and collison based on set information
	if (data->Scale.z == 1) {
		data->Object = ht_tree->getmodel()->child->sibling;
		Collision_Init(obj, &HillTree_Col[1], 1, 4);
	}
	else {
		data->Object = ht_tree->getmodel()->child;
		Collision_Init(obj, &HillTree_Col[0], 1, 4);
	}

	// If the scale is null, set it to normal
	if (data->Scale.x == 0) {
		data->Scale.x = 1;
	}

	// Adjust position with scale
	data->CollisionInfo->CollisionArray[0].center.y *= data->Scale.x + data->Scale.y;

	// Adjust collision size with scale
	data->CollisionInfo->CollisionArray[0].a *= data->Scale.x;
	data->CollisionInfo->CollisionArray[0].b *= data->Scale.x + data->Scale.y;
	
	obj->MainSub = HillTree_Main;
	obj->DisplaySub = HillTree_Display;
}