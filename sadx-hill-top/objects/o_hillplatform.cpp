#include "pch.h"
#include "o_hillplatform.h"

/*

Platform that goes up and down in lava.

ScaleX = scale
ScaleY = movement length
RotZ = speed (default to 150)

*/

ModelInfo* ht_platform = nullptr;

static void __cdecl HillPlatform_Delete(ObjectMaster* obj)
{
	// Removes the dyncol before deleting the object

	if (obj->Data1->LoopData)
	{
		DynamicCOL_Remove(obj, reinterpret_cast<NJS_OBJECT*>(obj->Data1->LoopData));
		ObjectArray_Remove(reinterpret_cast<NJS_OBJECT*>(obj->Data1->LoopData));
	}
}

static void __cdecl HillPlatform_Display(ObjectMaster* obj)
{
	if (!MissedFrames)
	{
		auto data = obj->Data1;

		njSetTexture(&HillTop_TexList);
		njPushMatrixEx();
		njTranslateEx(&data->Position);
		njRotateY_(data->Rotation.y);
		njScalef(data->Scale.x);
		DrawModel(data->Object->basicdxmodel);
		njPopMatrixEx();
	}
}

static void __cdecl HillPlatform_Main(ObjectMaster* obj)
{
	if (!ClipSetObject(obj))
	{
		auto data = obj->Data1;
		
		data->Position.y = data->Scale.z + static_cast<float>(1.0 - pow(njSin(FrameCounterUnpaused * data->Rotation.z), 2.0)) * data->Scale.y;

		reinterpret_cast<NJS_OBJECT*>(data->LoopData)->pos[1] = data->Position.y;

		obj->DisplaySub(obj);
	}
}

void __cdecl HillPlatform(ObjectMaster* obj)
{
	EntityData1* data = obj->Data1;
	
	if (data->Scale.x == 0.0f)
	{
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

	DynamicCOL_Add((ColFlags)(0x08004000 | ColFlags_Solid | ColFlags_UseRotation), obj, object);

	data->LoopData = (Loop*)object;
	data->Scale.z = data->Position.y;

	if (data->Rotation.z == 0)
	{
		data->Rotation.z = 150; // speed
	}

	obj->MainSub = HillPlatform_Main;
	obj->DisplaySub = HillPlatform_Display;
	obj->DeleteSub = HillPlatform_Delete;
}

void HillPlatform_LoadAssets()
{
	LoadModelFile(&ht_platform, "ht_platform", ModelFormat_Basic);
}

void HillPlatform_FreeAssets()
{
	FreeModelFile(&ht_platform);
}