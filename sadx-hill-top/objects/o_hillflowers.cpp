#include "pch.h"
#include "o_hillflowers.h"

ModelInfo* ht_flower0 = nullptr;
ModelInfo* ht_flower1 = nullptr;
ModelInfo* ht_flowerpatch0 = nullptr;
ModelInfo* ht_flowerpatch1 = nullptr;

AnimationFile* ht_flower0_anm = nullptr;
AnimationFile* ht_flower1_anm = nullptr;
AnimationFile* ht_flowerpatch1_anm = nullptr;

NJS_ACTION Flower0_Action = { 0 };
NJS_ACTION Flower1_Action = { 0 };
NJS_ACTION Flowerpatch1_Action = { 0 };

void __cdecl HillFlowers_Display(ObjectMaster* obj) {
	if (!MissedFrames) {
		EntityData1* data = obj->Data1;

		njSetTexture(LevelObjTexlists[1]);
		njPushMatrixEx();
		njTranslateEx(&data->Position);
		njRotateEx((Angle*)&data->Rotation, false);

		njScale(nullptr, data->Scale.x, data->Scale.x + data->Scale.y, data->Scale.x);
		
		njScaleY(1.0f + (1.0f - njSin(data->field_A * 2000)) / 50.0f); // bouncy animation

		if (data->LoopData) { // If there is an animation,
			njAction((NJS_ACTION*)data->LoopData, data->Scale.z);
		}
		else {
			njDrawModel_SADX(data->Object->basicdxmodel);
		}
		
		njPopMatrixEx();
	}
}

void __cdecl HillFlowers_Main(ObjectMaster* obj) {
	if (!ClipSetObject(obj)) {
		EntityData1* data = obj->Data1;

		// If there is an animation
		if (data->LoopData) {

			// Smooth transition between frames
			if (data->NextAction == 0) {
				data->Scale.z += 0.5f;

				if (data->Scale.z > 30.0f) {
					data->Scale.z = 30.0f;
					data->NextAction = 1;
				}
			}
			else {
				data->Scale.z -= 0.5f;

				if (data->Scale.z < 0.0f) {
					data->Scale.z = 0.0f;
					data->NextAction = 0;
				}
			}
		}

		data->field_A += 1; // bouncy animation

		obj->DisplaySub(obj);
	}
}

void __cdecl HillFlowers(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	
	// Choose the model and collison based on set information
	switch (static_cast<int>(data->Scale.z) % 6) {
	case 0:
	default:
		data->LoopData = (Loop*)&Flower0_Action;
		break;
	case 1:
		data->LoopData = (Loop*)&Flower1_Action;
		break;
	case 2:
		data->Object = ht_flowerpatch0->getmodel();
		break;
	case 3:
		data->LoopData = (Loop*)&Flowerpatch1_Action;
		break;
	case 4:
		data->Object = (NJS_OBJECT*)ht_flowerpatch1->getdata("Flower0");
		break;
	case 5:
		data->Object = (NJS_OBJECT*)ht_flowerpatch1->getdata("Flower1");
		break;
	}

	// If the scale is null, set it to normal
	if (data->Scale.x == 0) {
		data->Scale.x = 1;
	}

	data->Scale.z = rand() % 30; // we'll use that for the animation
	data->NextAction = rand() % 2; // randomize animation direction
	data->field_A = rand(); // randomize bump speed

	obj->MainSub = HillFlowers_Main;
	obj->DisplaySub = HillFlowers_Display;
}

void HillFlowers_LoadAssets() {
	LoadModel(&ht_flower0, "ht_flower0", ModelFormat_Basic);
	LoadModel(&ht_flower1, "ht_flower1", ModelFormat_Basic);
	LoadModel(&ht_flowerpatch0, "ht_flowerpatch0", ModelFormat_Basic);
	LoadModel(&ht_flowerpatch1, "ht_flowerpatch1", ModelFormat_Basic);

	// Set up animations:

	LoadAnimation(&ht_flower0_anm, "ht_flower0");
	LoadAnimation(&ht_flower1_anm, "ht_flower1");
	LoadAnimation(&ht_flowerpatch1_anm, "ht_flowerpatch1");

	Flower0_Action.object = ht_flower0->getmodel();
	Flower0_Action.motion = ht_flower0_anm->getmotion();

	Flower1_Action.object = ht_flower1->getmodel();
	Flower1_Action.motion = ht_flower1_anm->getmotion();

	Flowerpatch1_Action.object = ht_flowerpatch1->getmodel();
	Flowerpatch1_Action.motion = ht_flowerpatch1_anm->getmotion();
}