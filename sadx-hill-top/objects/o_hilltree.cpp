#include "pch.h"
#include "o_hilltree.h"

ModelInfo* ht_tree = nullptr;

CollisionData HillTree_Col = { 0, CollisionShape_Cylinder, 0x77, 0, 0, {0, 25.0f, 0}, 12.0f, 25.0f, 0, 0, 0, 0, 0 };

void __cdecl HillTree_Display(ObjectMaster* obj) {
	if (!MissedFrames) {
		EntityData1* data = obj->Data1;

		njSetTexture(LevelObjTexlists[1]);
		njPushMatrixEx();
		njTranslateEx(&Camera_Data1->Position);
		njDrawModel_SADX(data->Object->basicdxmodel);
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

	Collision_Init(obj, &HillTree_Col, 1, 4);

	data->Object = ht_tree->getmodel();
	
	obj->MainSub = HillTree_Main;
	obj->DisplaySub = HillTree_Display;
}