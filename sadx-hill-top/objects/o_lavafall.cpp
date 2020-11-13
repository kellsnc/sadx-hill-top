#include "pch.h"
#include "o_lavafall.h"

ModelInfo* ht_lavafall = nullptr;

static constexpr int lava_modelcount = 7;

struct LavaFall_Worker {
	NJS_OBJECT nodes[lava_modelcount];
};

void SetLavaMorph(LavaFall_Worker* wk, NJS_OBJECT* orig_object, Float scale, Float length, Float param3) {
	for (int i = 0; i < lava_modelcount; ++i) {
		
		orig_object = orig_object->sibling;
	}
}

void SetupLavaNodes(LavaFall_Worker* wk, NJS_OBJECT* object) {
	for (int i = 0; i < lava_modelcount; ++i) {
		memcpy(&wk->nodes[i], object, sizeof(NJS_OBJECT));
		wk->nodes[i].evalflags &= 0xFFFFFFFD;
		object = object->sibling;
	}
}

void __cdecl LavaFall_Display(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	LavaFall_Worker* wk = (LavaFall_Worker*)data->LoopData;

	if (!MissedFrames) {
		njSetTexture(LevelObjTexlists[1]);
		njPushMatrixEx();
		njTranslateEx(&data->Position);
		njRotateEx((Angle*)&data->Rotation, false);

		njScalef(data->Scale.x);

		SetupWorldMatrix();
		DrawChunkObject(wk->nodes);
		
		njPopMatrixEx();
	}
}

void __cdecl LavaFall_Main(ObjectMaster* obj) {
	if (!ClipSetObject(obj)) {
		EntityData1* data = obj->Data1;
		LavaFall_Worker* wk = (LavaFall_Worker*)data->LoopData;

		SetLavaMorph(wk, data->Object->child, data->Scale.x, data->Scale.y, data->Scale.z);

		obj->DisplaySub(obj);
	}
}

void __cdecl LavaFall(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	LavaFall_Worker* wk = new LavaFall_Worker;

	data->Object = ht_lavafall->getmodel();

	SetupLavaNodes(wk, data->Object->child);	
	SetLavaMorph(wk, data->Object->child, data->Scale.x, data->Scale.y, data->Scale.z);

	data->LoopData = (Loop*)wk;

	obj->MainSub = LavaFall_Main;
	obj->DisplaySub = LavaFall_Display;
}

void LavaFall_LoadModel() {
	LoadModel(&ht_lavafall, "ht_lavafall", ModelFormat_Chunk);
}