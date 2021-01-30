#include "pch.h"
#include "o_hillplatform.h"

/*

Platform that goes up and down in lava.

ScaleX = scale
ScaleY = movement length
RotZ = speed (default to 150)

*/

ModelInfo* ht_platform = nullptr;

extern NJS_TEXLIST HillTop_TexList;

static void UpdateDynCol(NJS_OBJECT* dyncol, float y) {
	dyncol->pos[1] = y;
}

void __cdecl HillPlatform_Delete(ObjectMaster* obj) {
	// Removes the dyncol before deleting the object

	if (obj->Data1->LoopData) {
		DynamicCOL_Remove(obj, (NJS_OBJECT*)obj->Data1->LoopData);
		ObjectArray_Remove((NJS_OBJECT*)obj->Data1->LoopData);
	}
}

void __cdecl HillPlatform_Display(ObjectMaster* obj) {
	if (!MissedFrames) {
		EntityData1* data = obj->Data1;

		njSetTexture(&HillTop_TexList);
		njPushMatrixEx();
		njTranslateEx(&data->Position);
		njRotateY_(data->Rotation.y);
		njScalef(data->Scale.x);
		njDrawModel_SADX(data->Object->basicdxmodel);
		njPopMatrixEx();
	}
}

void __cdecl HillPlatform_Main(ObjectMaster* obj) {
	if (!ClipSetObject(obj)) {
		EntityData1* data = obj->Data1;
		
		data->Position.y = data->Scale.z + (1.0f - powf(njSin(FrameCounterUnpaused * data->Rotation.z), 2.0f)) * data->Scale.y;

		UpdateDynCol((NJS_OBJECT*)data->LoopData, data->Position.y);
		obj->DisplaySub(obj);
	}
}

void __cdecl HillPlatform(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	
	if (data->Scale.x == 0.0f) {
		data->Scale.x = 1.0f;
	}

	data->Object = ht_platform->getmodel();

	// Create the dynamic collision
	NJS_OBJECT* object = ObjectArray_GetFreeObject();

	object->pos[0] = data->Position.x;
	object->pos[1] = data->Position.y;
	object->pos[2] = data->Position.z;

	object->ang[0] = 0;
	object->ang[1] = data->Rotation.y;
	object->ang[2] = 0;

	object->scl[0] = data->Scale.x;
	object->scl[1] = data->Scale.x;
	object->scl[2] = data->Scale.x;

	object->basicdxmodel = data->Object->basicdxmodel;

	DynamicCOL_Add((ColFlags)(0x08000000 | ColFlags_Solid | ColFlags_UseRotation), obj, object);

	data->LoopData = (Loop*)object;
	data->Scale.z = data->Position.y;

	if (data->Rotation.z == 0) {
		data->Rotation.z = 150; // speed
	}

	obj->MainSub = HillPlatform_Main;
	obj->DisplaySub = HillPlatform_Display;
	obj->DeleteSub = HillPlatform_Delete;
}

void HillPlatform_LoadAssets() {
	LoadModel(&ht_platform, "ht_platform", ModelFormat_Basic);
}