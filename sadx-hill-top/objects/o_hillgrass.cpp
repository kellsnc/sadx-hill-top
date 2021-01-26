#include "pch.h"
#include "o_hillgrass.h"

ModelInfo* ht_grass0 = nullptr;

AnimationFile* ht_grass0_anm = nullptr;

NJS_ACTION Grass0_Action = { 0 };

void __cdecl HillGrass_Display(ObjectMaster* obj) {
	if (!MissedFrames) {
		EntityData1* data = obj->Data1;

		njSetTexture(LevelObjTexlists[1]);
		njPushMatrixEx();
		njTranslateEx(&data->Position);
		njRotateEx((Angle*)&data->Rotation, false);
		
		njScaleY(1.0f + data->Scale.y);

		if (IsPlayerInsideSphere_(&data->Position, 400.0f)) {
			njAction(&Grass0_Action, data->Scale.z);
		}
		else {
			DrawObject(Grass0_Action.object);
		}
		
		njPopMatrixEx();
	}
}

void __cdecl HillGrass_Main(ObjectMaster* obj) {
	if (!ClipSetObject(obj)) {
		EntityData1* data = obj->Data1;
		
		// Animate
		data->Scale.z += 0.5f;

		obj->DisplaySub(obj);
	}
}

void __cdecl HillGrass(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	
	data->Scale.z = rand(); // randomize animation start

	obj->MainSub = HillGrass_Main;
	obj->DisplaySub = HillGrass_Display;
}

void HillGrass_LoadAssets() {
	LoadModel(&ht_grass0, "ht_grass0", ModelFormat_Basic);
	LoadAnimation(&ht_grass0_anm, "ht_grass0");

	Grass0_Action.object = ht_grass0->getmodel();
	Grass0_Action.motion = ht_grass0_anm->getmotion();
}