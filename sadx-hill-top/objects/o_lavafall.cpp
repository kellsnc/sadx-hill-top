#include "pch.h"
#include "o_lavafall.h"

/*

A chunk model representing a fall of lava, the model bends with animations.

ScaleX: Global scale
ScaleY: Additional Y scale
RotZ: animation ID

*/

ModelInfo* ht_lavafall = nullptr;

AnimationFile* ht_lavafall0_anm = nullptr;
AnimationFile* ht_lavafall1_anm = nullptr;
AnimationFile* ht_lavafall2_anm = nullptr;
AnimationFile* ht_lavafall3_anm = nullptr;

NJS_ACTION LavaFall0_Action = { 0 };
NJS_ACTION LavaFall1_Action = { 0 };
NJS_ACTION LavaFall2_Action = { 0 };
NJS_ACTION LavaFall3_Action = { 0 };

NJS_TEXNAME LAVAFALL_TexNames[16];
NJS_TEXLIST LAVAFALL_TexList = { arrayptrandlength(LAVAFALL_TexNames) };

// The actual lava texture we are going to draw
static NJS_TEXLIST CurrentLavaTex = { nullptr, 1 };

CollisionData LavaFall_Col[] = {
	{ 0, CollisionShape_Cylinder, 0x77, 0x2F /* Hurt flag */, 0, {0, -25.0f, 0}, 8.0f, 25.0f, 0, 0, 0, 0, 0 },
	{ 0, CollisionShape_Capsule2, 0x77, 0x2F, 0, {0, -14.0f, -13.0f}, 10.0f, 8.0f, 0, 0, 0x2000, 0, 0 },
	{ 0, CollisionShape_Capsule2, 0x77, 0x2F, 0, {0, -15.0f, -24.0f}, 10.0f, 8.0f, 0, 0, 0x3000, 0, 0 }
};

void __cdecl LavaFall_Display(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	
	if (!MissedFrames) {

		// Since we cannot do UV shifting for chunk models (that is used here to bend the lava),
		// I just made 16 lava textures and I change the texture address before drawing.
		CurrentLavaTex.textures = &LAVAFALL_TexNames[data->Index];

		njSetTexture(&CurrentLavaTex);
		njPushMatrixEx();
		njTranslateEx(&data->Position);
		njRotateY_(data->Rotation.y);
		njRotateX_(data->Rotation.x);

		njScale(0, data->Scale.x, data->Scale.x + data->Scale.y, data->Scale.x);

		SetupWorldMatrix();
		Direct3D_SetChunkModelRenderState();
		njCnkAction((NJS_ACTION*)data->LoopData, data->Scale.z);
		Direct3D_UnsetChunkModelRenderState();
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

		// Texture animation
		if (FrameCounterUnpaused % 2 == 0) {
			data->Index += 1;
		}

		if (data->Index >= LAVAFALL_TexList.nbTexture) {
			data->Index = 0;
		}

		AddToCollisionList(data);
		obj->DisplaySub(obj);
	}
}

void __cdecl LavaFall(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;

	data->Object = ht_lavafall->getmodel();

	switch (static_cast<int>(data->Rotation.z) % 4) {
	case 0:
	default:
		data->LoopData = (Loop*)&LavaFall0_Action;
		Collision_Init(obj, &LavaFall_Col[0], 1, 4);
		break;
	case 1:
		data->LoopData = (Loop*)&LavaFall1_Action;
		Collision_Init(obj, &LavaFall_Col[0], 1, 4);
		break;
	case 2:
		data->LoopData = (Loop*)&LavaFall2_Action;
		//Collision_Init(obj, &LavaFall_Col[1], 1, 4);
		data->Rotation.x -= 0x4000;
		break;
	case 3:
		data->LoopData = (Loop*)&LavaFall3_Action;
		//Collision_Init(obj, &LavaFall_Col[2], 1, 4);
		data->Rotation.x -= 0x4000;
		break;
	}

	if (data->Scale.x == 0) {
		data->Scale.x = 1;
	}

	// Adjust collision size with scale
	if (data->CollisionInfo) {
		data->CollisionInfo->CollisionArray[0].center.y *= data->Scale.x + (data->Scale.y / 8);
		data->CollisionInfo->CollisionArray[0].a *= data->Scale.x;
		data->CollisionInfo->CollisionArray[0].b *= data->Scale.x + (data->Scale.y / 8);
	}

	data->Scale.x *= 8;
	
	obj->MainSub = LavaFall_Main;
	obj->DisplaySub = LavaFall_Display;
}

void LavaFall_LoadAssets() {
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