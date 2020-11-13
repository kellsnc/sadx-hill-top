#include "pch.h"
#include "o_hillflowers.h"

ModelInfo* ht_flower0 = nullptr;
ModelInfo* ht_flower1 = nullptr;
ModelInfo* ht_flowerpatch = nullptr;

AnimationFile* ht_flower0_anm = nullptr;
AnimationFile* ht_flower1_anm = nullptr;

NJS_ACTION Flower0_Action = { 0 };
NJS_ACTION Flower1_Action = { 0 };

void __cdecl HillFlowers_Display(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;

	if (!MissedFrames) {
		njSetTexture(LevelObjTexlists[1]);
		njPushMatrixEx();
		njTranslateEx(&data->Position);
		njRotateEx((Angle*)&data->Rotation, false);

		njScale(nullptr, data->Scale.x, data->Scale.x + data->Scale.y, data->Scale.x);

		if (data->LoopData && IsPlayerInsideSphere_(&data->Position, 500.0f)) {
			njAction((NJS_ACTION*)data->LoopData, static_cast<float>(FrameCounterUnpaused) / 2.0f);
		}
		else {
			DrawObject(data->Object);
		}
		
		njPopMatrixEx();
	}
}

void __cdecl HillFlowers_Main(ObjectMaster* obj) {
	if (!ClipSetObject(obj)) {
		obj->DisplaySub(obj);
	}
}

void __cdecl HillFlowers(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;

	// Choose the model and collison based on set information
	switch (static_cast<int>(data->Scale.z)) {
	default:
		data->Object = ht_flower0->getmodel();
		data->LoopData = (Loop*)&Flower0_Action;
		break;
	case 1:
		data->Object = ht_flower1->getmodel();
		data->LoopData = (Loop*)&Flower1_Action;
		break;
	case 2:
		data->Object = ht_flowerpatch->getmodel();
		break;
	}
	
	// If the scale is null, set it to normal
	if (data->Scale.x == 0) {
		data->Scale.x = 1;
	}

	obj->MainSub = HillFlowers_Main;
	obj->DisplaySub = HillFlowers_Display;
}

void HillFlowers_LoadModels() {
	LoadModel(&ht_flower0, "ht_flower0", ModelFormat_Basic);
	LoadModel(&ht_flower1, "ht_flower1", ModelFormat_Basic);
	LoadModel(&ht_flowerpatch, "ht_flowerpatch", ModelFormat_Basic);

	// Set up animations:

	LoadAnimation(&ht_flower0_anm, "ht_flower0");
	LoadAnimation(&ht_flower1_anm, "ht_flower1");

	Flower0_Action.object = ht_flower0->getmodel();
	Flower0_Action.motion = ht_flower0_anm->getmotion();

	Flower1_Action.object = ht_flower1->getmodel();
	Flower1_Action.motion = ht_flower1_anm->getmotion();
}