#include "pch.h"
#include "o_growlava.h"
#include "lava.h"

/*

Objects related to the growing lava event in act 2

*/

extern NJS_TEXLIST HillTop_TexList;

extern ModelInfo* ht_platform;
static ModelInfo* ht_growlava = nullptr;

static CollisionData GrowLavaTrigger_Col = { 0, CI_FORM_SPHERE, 0xF0, 0, 0, { 0, 0, 0 }, 0, 0, 0, 0, 0 };

Grow_WK grow_workers[3] = { };

static void UpdateDynCol(NJS_OBJECT* dyncol, NJS_VECTOR* pos, int id)
{
	dyncol->pos[0] = pos->x;
	dyncol->pos[1] = pos->y;
	dyncol->pos[2] = pos->z;

	dyncol->pos[1] += grow_workers[id].Offset;
}

#pragma region GrowLava

/*

Lava that goes up when the trigger is trigged
ScaleX: model ID
ScaleY: lava (0) or ground (1)

*/

void __cdecl GrowLava_Delete(ObjectMaster* obj)
{
	// Removes the dyncol before deleting the object

	if (obj->Data1->LoopData)
	{
		DynamicCOL_Remove(obj, (NJS_OBJECT*)obj->Data1->LoopData);
		ObjectArray_Remove((NJS_OBJECT*)obj->Data1->LoopData);
	}
}

void __cdecl GrowLava_Display(ObjectMaster* obj)
{
	if (!MissedFrames)
	{
		EntityData1* data = obj->Data1;

		int trigger_id = data->Rotation.z;

		if (data->Scale.y == 1.0f)
		{
			njSetTexture(&HillTop_TexList);
		}
		else
		{
			njSetTexture(&CurrentLavaTex);
		}

		njPushMatrixEx();
		njTranslateEx(&data->Position);

		if (grow_workers[trigger_id].Enabled == true)
		{
			njTranslateY(grow_workers[trigger_id].Offset);
		}

		DrawModel(data->Object->basicdxmodel);
		njPopMatrixEx();
	}
}

void __cdecl GrowLava_Main(ObjectMaster* obj)
{
	EntityData1* data = obj->Data1;

	int trigger_id = data->Rotation.z;

	if (grow_workers[trigger_id].Enabled == true)
	{
		data->Position.y = grow_workers[trigger_id].Height;

		if (data->Position.y < data->Scale.z)
		{
			data->Position.y = data->Scale.z;
		}

		UpdateDynCol((NJS_OBJECT*)data->LoopData, &data->Position, trigger_id);
	}

	obj->DisplaySub(obj);
}

void __cdecl GrowLava(ObjectMaster* obj)
{
	EntityData1* data = obj->Data1;

	data->Object = GetModelSibling(ht_growlava->getmodel()->child, static_cast<int>(data->Scale.x));

	NJS_OBJECT* object = ObjectArray_GetFreeObject();

	object->pos[0] = data->Position.x;
	object->pos[1] = data->Position.y;
	object->pos[2] = data->Position.z;

	object->ang[0] = 0;
	object->ang[1] = 0;
	object->ang[2] = 0;

	object->scl[0] = 1.0f;
	object->scl[1] = 1.0f;
	object->scl[2] = 1.0f;

	object->basicdxmodel = data->Object->basicdxmodel;

	if (data->Scale.y == 1.0f)
	{
		DynamicCOL_Add((ColFlags)(0x08000000 | ColFlags_Solid), obj, object);
	}
	else
	{
		DynamicCOL_Add((ColFlags)(0x08004000 | ColFlags_Solid | ColFlags_Hurt), obj, object);
	}

	data->LoopData = (Loop*)object;
	data->Scale.z = data->Position.y;

	obj->MainSub = GrowLava_Main;
	obj->DeleteSub = GrowLava_Delete;
	obj->DisplaySub = GrowLava_Display;
}
#pragma endregion

#pragma region GrowLavaPlatform

/*

Platform that moves with lava height

RotX: lava id
RotZ: speed
ScaleX: scale
ScaleY: movement radius

*/

void __cdecl GrowLavaPlatform_Delete(ObjectMaster* obj)
{
	// Removes the dyncol before deleting the object

	if (obj->Data1->LoopData)
	{
		DynamicCOL_Remove(obj, (NJS_OBJECT*)obj->Data1->LoopData);
		ObjectArray_Remove((NJS_OBJECT*)obj->Data1->LoopData);
	}
}

void __cdecl GrowLavaPlatform_Display(ObjectMaster* obj)
{
	if (!MissedFrames)
	{
		EntityData1* data = obj->Data1;

		int trigger_id = data->Rotation.x;

		njSetTexture(&HillTop_TexList);
		njPushMatrixEx();
		njTranslateEx(&data->Position);

		if (grow_workers[trigger_id].Enabled == true)
		{
			njTranslateY(grow_workers[trigger_id].Offset);
		}

		njRotateY_(data->Rotation.y);
		njScalef(data->Scale.x);
		DrawModel(data->Object->basicdxmodel);
		njPopMatrixEx();
	}
}

void __cdecl GrowLavaPlatform_Main(ObjectMaster* obj)
{
	if (!ClipSetObject(obj))
	{
		EntityData1* data = obj->Data1;

		int trigger_id = data->Rotation.x;

		if (grow_workers[trigger_id].Enabled == true && grow_workers[trigger_id].Height > data->Scale.z - data->Scale.y - 10.0f)
		{
			data->Scale.z = grow_workers[trigger_id].Height + data->Scale.y + 10.0f;
		}

		data->Position.y = data->Scale.z + (1.0f - powf(njSin(FrameCounterUnpaused * data->Rotation.z), 2.0f) * data->Scale.y);

		UpdateDynCol((NJS_OBJECT*)data->LoopData, &data->Position, trigger_id);
		obj->DisplaySub(obj);
	}
}

void __cdecl GrowLavaPlatform(ObjectMaster* obj)
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

	DynamicCOL_Add((ColFlags)(0x08000000 | ColFlags_Solid | ColFlags_UseRotation), obj, object);

	data->LoopData = (Loop*)object;
	data->Scale.z = data->Position.y;

	if (data->Rotation.z == 0)
	{
		data->Rotation.z = 150; // speed
	}

	obj->MainSub = GrowLavaPlatform_Main;
	obj->DisplaySub = GrowLavaPlatform_Display;
	obj->DeleteSub = GrowLavaPlatform_Delete;
}
#pragma endregion

#pragma region GrowLavaTrigger
/*

Trigger that launches the lava & plaforms

RotX: grow speed (divided by 100)
RotY: if 1, reset lava id on restart
RotZ: index
ScaleX: radius
ScaleY: start height
ScaleZ: end height

*/

void __cdecl GrowLavaTrigger_Delete(ObjectMaster* obj)
{
	grow_workers[obj->Data1->Rotation.z].Enabled = false;
	grow_workers[obj->Data1->Rotation.z].Height = 0.0f;
	grow_workers[obj->Data1->Rotation.z].Offset = 0.0f;
}

void __cdecl GrowLavaTrigger_Main(ObjectMaster* obj)
{
	EntityData1* data = obj->Data1;

	int trigger_id = data->Rotation.z;

	// only reset lava if restarting
	if (data->Rotation.y == 1 && GameState != 4)
	{
		data->Action = 2;
	}

	if (data->Action == 0)
	{
		if (IsSpecificPlayerInSphere(&data->Position, data->Scale.x, 0))
		{
			EntityData1* entity = EntityData1Ptrs[0];

			if (data->Rotation.y == 1)
			{
				UpdateSetDataAndDelete(obj);
			}
			else
			{
				grow_workers[trigger_id].Enabled = true;
				grow_workers[trigger_id].Height = data->Scale.y;
				PlaySound(462, nullptr, 0, 0);
				data->field_A = 0;
				data->Action = 1;
			}
		}

		AddToCollisionList(data);
	}
	else if (data->Action == 1)
	{
		if (grow_workers[trigger_id].Height >= data->Scale.z)
		{
			grow_workers[trigger_id].Enabled = false;
			data->Action = 2;
		}

		if (!(CharObj2Ptrs[0]->Powerups & Powerups_Dead))
		{
			grow_workers[trigger_id].Height += static_cast<float>(data->Rotation.x) / 100.0f;
		}

		// Shake everything

		++data->field_A;

		if (data->field_A % 100 == 0)
		{
			PlaySound(462, nullptr, 0, 0);
		}

		if (data->field_A < 90)
		{
			float sin = (static_cast<float>(data->field_A) / 90.0f) * 1.5f;

			if (sin <= 1.0f)
			{
				sin = njSin(((sin * 90.0f + 90.0f) * 182.0444488525391f));
			}
			else
			{
				sin = 0.0f;
			}

			grow_workers[trigger_id].Offset = njSin((static_cast<float>(data->field_A) * 40.0f * 182.0444488525391f)) * (sin * 4.0f);
		}
		else
		{
			grow_workers[trigger_id].Offset = njSin(((static_cast<float>(data->field_A) - 90.0f) * 48.0f * 182.0444488525391f)) * 0.1800000071525574f;
		}
	}
}

void __cdecl GrowLavaTrigger(ObjectMaster* obj)
{
	EntityData1* data = obj->Data1;

	Collision_Init(obj, &GrowLavaTrigger_Col, 1, 4);

	data->CollisionInfo->CollisionArray->a = data->Scale.x;

	obj->MainSub = GrowLavaTrigger_Main;
	obj->DeleteSub = GrowLavaTrigger_Delete;
}
#pragma endregion

#pragma region KillCeiling

/*

Object that kills the player if standing while colliding with this.
Scale: box scale
RotX: lava id

*/

void __cdecl KillCeiling(ObjectMaster* obj)
{
	EntityData1* data = obj->Data1;

	ClipSetObject(obj);

	if (data->Action)
	{
		EntityData1* entity = GetCollidingEntityA(data);

		// if the player colldies while being on ground
		if (grow_workers[data->Rotation.z].Enabled == true && entity && (entity->Status & Status_Ground) && (CharObj2Ptrs[entity->CharIndex]->SurfaceFlags & 0x08000000))
		{
			KillPlayer(entity->CharIndex);
		}

		AddToCollisionList(data);
	}
	else
	{
		Collision_Init(obj, &C_Cube_Collision, 1, 4u);

		CollisionData* colinfo = data->CollisionInfo->CollisionArray;

		colinfo->push = 0xF0;
		colinfo->a = data->Scale.x + 10.0f;
		colinfo->b = data->Scale.y + 10.0f;
		colinfo->c = data->Scale.z + 10.0f;

		Collision_CalculateRadius(data);

		data->Action = 1;
	}
}
#pragma endregion

void GrowLava_LoadAssets()
{
	LoadModelFile(&ht_growlava, "ht_grow", ModelFormat_Basic);
}

void GrowLava_FreeAssets()
{
	FreeFileInfo(&ht_growlava);
}