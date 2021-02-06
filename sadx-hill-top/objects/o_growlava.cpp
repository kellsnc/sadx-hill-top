#include "pch.h"
#include "o_growlava.h"
#include "lava.h"

/*

Objects related to the growing lava event in act 2

*/

extern NJS_TEXLIST HillTop_TexList;

extern ModelInfo* ht_platform;
static ModelInfo* ht_growlava = nullptr;

static CollisionData GrowLavaTrigger_Col = { 0, CollisionShape_Sphere, 0xF0, 0, 0, { 0, 0, 0 }, 0, 0, 0, 0, 0 };
static int TriggerIndex = -1;
static float LavaHeight = 0.0f;
static float LavaSpeed = 0.0f;

static float ShakeOffset = 0.0f;

static void UpdateDynCol(NJS_OBJECT* dyncol, NJS_VECTOR* pos) {
	dyncol->pos[0] = pos->x;
	dyncol->pos[1] = pos->y;
	dyncol->pos[2] = pos->z;
	
	dyncol->pos[1] += ShakeOffset;
}

#pragma region GrowLava

/*

Lava that goes up when the trigger is trigged
ScaleX: model ID

*/

void __cdecl GrowLava_Delete(ObjectMaster* obj) {
	// Removes the dyncol before deleting the object

	if (obj->Data1->LoopData) {
		DynamicCOL_Remove(obj, (NJS_OBJECT*)obj->Data1->LoopData);
		ObjectArray_Remove((NJS_OBJECT*)obj->Data1->LoopData);
	}
}

void __cdecl GrowLava_Display(ObjectMaster* obj) {
	if (!MissedFrames) {
		EntityData1* data = obj->Data1;

		njSetTexture(&CurrentLavaTex);
		njPushMatrixEx();
		njTranslateEx(&data->Position);
		njTranslateY(ShakeOffset);
		njDrawModel_SADX(data->Object->basicdxmodel);
		njPopMatrixEx();
	}
}

void __cdecl GrowLava_Main(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;

	if (LavaHeight != 0.0f) {
		data->Position.y = LavaHeight;

		if (data->Position.y < data->Scale.z) {
			data->Position.y = data->Scale.z;
		}

		UpdateDynCol((NJS_OBJECT*)data->LoopData, &data->Position);
	}
	else {
		data->Position.y = data->Scale.z;
	}

	obj->DisplaySub(obj);
}

void __cdecl GrowLava(ObjectMaster* obj) {
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

	DynamicCOL_Add((ColFlags)(0x08000000 | ColFlags_Solid | ColFlags_Hurt), obj, object);

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

RotZ: speed
ScaleX: scale
ScaleY: movement radius

*/

void __cdecl GrowLavaPlatform_Delete(ObjectMaster* obj) {
	// Removes the dyncol before deleting the object

	if (obj->Data1->LoopData) {
		DynamicCOL_Remove(obj, (NJS_OBJECT*)obj->Data1->LoopData);
		ObjectArray_Remove((NJS_OBJECT*)obj->Data1->LoopData);
	}
}

void __cdecl GrowLavaPlatform_Display(ObjectMaster* obj) {
	if (!MissedFrames) {
		EntityData1* data = obj->Data1;

		njSetTexture(&HillTop_TexList);
		njPushMatrixEx();
		njTranslateEx(&data->Position);
		njTranslateY(ShakeOffset);
		njRotateY_(data->Rotation.y);
		njScalef(data->Scale.x);
		njDrawModel_SADX(data->Object->basicdxmodel);
		njPopMatrixEx();
	}
}

void __cdecl GrowLavaPlatform_Main(ObjectMaster* obj) {
	if (!ClipSetObject(obj)) {
		EntityData1* data = obj->Data1;

		if (TriggerIndex == -data->Rotation.x) {
			data->Scale.z += LavaSpeed;
		}
		
		data->Position.y = data->Scale.z + (1.0f - powf(njSin(FrameCounterUnpaused * data->Rotation.z), 2.0f) * data->Scale.y);
		
		UpdateDynCol((NJS_OBJECT*)data->LoopData, &data->Position);
		obj->DisplaySub(obj);
	}
}

void __cdecl GrowLavaPlatform(ObjectMaster* obj) {
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

	obj->MainSub = GrowLavaPlatform_Main;
	obj->DisplaySub = GrowLavaPlatform_Display;
	obj->DeleteSub = GrowLavaPlatform_Delete;
}
#pragma endregion

#pragma region GrowLavaTrigger
/*

Trigger that launches the lava & plaforms

RotX: grow speed (divided by 100)
RotY: if 1, force lava to position in scale y on restart
RotZ: index
ScaleX: radius
ScaleY: start height
ScaleZ: end height

*/

void __cdecl GrowLavaTrigger_Delete(ObjectMaster* obj) {
	if (TriggerIndex == obj->Data1->Rotation.z) {
		TriggerIndex = -1;
	}
}

void __cdecl GrowLavaTrigger_Main(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;

	// only reset lava if restarting
	if (data->Rotation.y == 1 && GameState != 4) {
		UpdateSetDataAndDelete(obj);
	}

	if (data->Action == 0) {
		if (!ClipSetObject(obj)) {
			EntityData1* entity = EntityData1Ptrs[IsPlayerInsideSphere_(&data->Position, data->Scale.x) - 1];

			if (entity && entity->CharIndex == 0) {
				LavaHeight = data->Scale.y;
				LavaSpeed = static_cast<float>(data->Rotation.x) / 100;
				
				if (data->Rotation.y == 1) {
					TriggerIndex = -1;
					UpdateSetDataAndDelete(obj);
				}
				else {
					TriggerIndex = data->Rotation.z;
					PlaySound(462, nullptr, 0, 0);
					data->field_A = 0;
					data->Action = 1;
				}
			}

			AddToCollisionList(data);
		}
	}
	else {
		if (TriggerIndex != data->Rotation.z || LavaHeight >= data->Scale.z) {
			DeleteObject_(obj);
		}

		LavaHeight += LavaSpeed;

		// Shake everything

		++data->field_A;

		if (data->field_A % 100 == 0) {
			PlaySound(462, nullptr, 0, 0);
		}

		if (data->field_A < 90) {
			float sin = (static_cast<float>(data->field_A) / 90.0f) * 1.5f;

			if (sin <= 1.0f) {
				sin = njSin(((sin * 90.0f + 90.0f) * 182.0444488525391f));
			}
			else {
				sin = 0.0f;
			}

			ShakeOffset = njSin((static_cast<float>(data->field_A) * 40.0f * 182.0444488525391f)) * (sin * 4.0f);
		}
		else 
		{
			ShakeOffset = njSin(((static_cast<float>(data->field_A) - 90.0f) * 48.0f * 182.0444488525391f)) * 0.1800000071525574f;
		}
	}
}

void __cdecl GrowLavaTrigger(ObjectMaster* obj) {
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

*/

void __cdecl KillCeiling(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;

	ClipSetObject(obj);

	if (data->Action) {
		EntityData1* entity = GetCollidingEntityA(data);

		// if the player colldies while being on ground
		if (entity && entity->Status & (Status_Ground | Status_Unknown1)) {
			KillPlayer(entity->CharIndex);
		}

		AddToCollisionList(data);
	}
	else {
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

void GrowLava_LoadAssets() {
	LoadModelFile(&ht_growlava, "ht_growlava", ModelFormat_Basic);
}

void GrowLava_FreeAssets() {
	FreeModelFile(&ht_growlava);
}