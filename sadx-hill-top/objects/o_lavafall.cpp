#include "pch.h"
#include "o_lavafall.h"

ModelInfo* ht_lavafall = nullptr;

AnimationFile* ht_lavafall0_anm = nullptr;
AnimationFile* ht_lavafall1_anm = nullptr;
AnimationFile* ht_lavafall2_anm = nullptr;
AnimationFile* ht_lavafall3_anm = nullptr;

NJS_ACTION LavaFall0_Action = { 0 };
NJS_ACTION LavaFall1_Action = { 0 };
NJS_ACTION LavaFall2_Action = { 0 };
NJS_ACTION LavaFall3_Action = { 0 };

void __cdecl LavaFall_Display(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	
	if (!MissedFrames) {
		njSetTexture(LevelObjTexlists[1]);
		njPushMatrixEx();
		njTranslateEx(&data->Position);
		njRotateEx((Angle*)&data->Rotation, false);

		njScalef(data->Scale.x);

		SetupWorldMatrix();
		njCnkAction((NJS_ACTION*)data->LoopData, data->Scale.z);
		
		njPopMatrixEx();
	}
}

void __cdecl LavaFall_Main(ObjectMaster* obj) {
	if (!ClipSetObject(obj)) {
		EntityData1* data = obj->Data1;
		
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

		obj->DisplaySub(obj);
	}
}

void __cdecl LavaFall(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;

	data->Scale.x = 8;
	data->Object = ht_lavafall->getmodel();

	switch (static_cast<int>(data->Scale.y) % 4) {
	case 0:
	default:
		data->LoopData = (Loop*)&LavaFall0_Action;
		break;
	}

	if (data->Scale.x == 0) {
		data->Scale.x = 1;
	}
	
	obj->MainSub = LavaFall_Main;
	obj->DisplaySub = LavaFall_Display;
}

void LavaFall_LoadModel() {
	LoadModel(&ht_lavafall, "ht_lavafall", ModelFormat_Chunk);

	LoadAnimation(&ht_lavafall0_anm, "ht_lavafall0");
	LoadAnimation(&ht_lavafall1_anm, "ht_lavafall1");
	LoadAnimation(&ht_lavafall2_anm, "ht_lavafall2");
	LoadAnimation(&ht_lavafall3_anm, "ht_lavafall3");

	NJS_OBJECT* object = ht_lavafall->getmodel();

	LavaFall0_Action.object = object;
	LavaFall0_Action.motion = ht_lavafall0_anm->getmotion();

	LavaFall1_Action.object = object;
	LavaFall1_Action.motion = ht_lavafall1_anm->getmotion();

	LavaFall2_Action.object = object;
	LavaFall2_Action.motion = ht_lavafall2_anm->getmotion();

	LavaFall3_Action.object = object;
	LavaFall3_Action.motion = ht_lavafall3_anm->getmotion();
}