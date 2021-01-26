#include "pch.h"
#include "o_hillpole.h"

// Unidus balance, Scale X is height speed power.

extern ModelInfo* ht_transporter;

CollisionData HillPole_Col = {
	0, CollisionShape_Capsule2, 0x77, 0, 0, {0.0f, 20.0f, 0.0f}, 2.0f, 20.0f, 0, 0, 0, 0, 0
};

void __cdecl HillPole_Display(ObjectMaster* obj) {
	if (!MissedFrames) {
		EntityData1* data = obj->Data1;

		njSetTexture(LevelObjTexlists[1]);
		njPushMatrixEx();
		njTranslateEx(&data->Position);
		njDrawModel_SADX(data->Object->basicdxmodel);
		njPopMatrixEx();
	}
}

void __cdecl HillPole_Main(ObjectMaster* obj) {
	if (!ClipSetObject(obj)) {
		AddToCollisionList(obj->Data1);
		obj->DisplaySub(obj);
	}
}

void __cdecl HillPole(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;

	data->Object = ht_transporter->getmodel()->child->sibling;

	Collision_Init(obj, &HillPole_Col, 1, 4);

	obj->MainSub = HillPole_Main;
	obj->DisplaySub = HillPole_Display;
}