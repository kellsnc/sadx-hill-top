#include "pch.h"
#include "o_hillflowers.h"

/*

Unidus balance

ScaleX: Height power

*/

ModelInfo* ht_balance = nullptr;

CollisionData HillTBalance_Col[] = {
	{ 0, CollisionShape_Capsule2, 0x77, 0, 0, {0.0f, 1.3f, 0.0f}, 1.3f, 4.5f, 0, 0, 0x4000, 0, 0 },
	{ 0, CollisionShape_Cube2, 0x77, 0, 0, {0.0f, 3.0f, 0.0f}, 18.0f, 0.35f, 3.7f, 0, 0, 0, 0 }
};

int Balance_IsOnBalance(EntityData1* data, float radius) {
	EntityData1* entity = GetCollidingEntityA(data);

	if (entity && entity->Action <= 2) {
		NJS_VECTOR newpos = { 0 };

		njPushMatrix(_nj_unit_matrix_);
		njTranslateEx(&data->Position);
		njRotateY_(data->Rotation.y);
		njRotateX_(data->Rotation.x);
		njTranslateX(-16.0f);
		njTranslateY(10.0f);
		njGetTranslation(0, &newpos);
		njPopMatrixEx();

		if (IsPointInsideSphere(&newpos, &entity->Position, radius)) {
			return entity->CharIndex + 1;
		}
	}

	return 0;
}

void __cdecl UnidusBalance_Display(ObjectMaster* obj) {
	if (!MissedFrames) {
		EntityData1* data = obj->Data1;

		njSetTexture(&UNI_A_UNIBODY_TEXLIST);
		njPushMatrixEx();
		njTranslateEx(&data->Position);
		njRotateY_(data->Rotation.y);

		DrawObject((NJS_OBJECT*)0x96DBF0);

		njPopMatrixEx();
	}
}

void __cdecl UnidusBalance_Main(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	Float orig = obj->Parent->SETData.SETData->SETEntry->Position.y + 4.0f;

	if (data->Action == 1) {
		data->field_A += 500;

		unsigned int sin = static_cast<unsigned int>(data->field_A);

		if (sin > 0x8000) {
			data->Action = 0;
			data->field_A = 0;
			data->Position.y = orig;
		}
		else {
			data->Position.y = orig + 50.0f * njSin(sin);
		}
	}

	AddToCollisionList(data);
	obj->DisplaySub(obj);
}

void LoadUnidusBalance(ObjectMaster* obj, EntityData1* data) {
	ObjectMaster* child = LoadChildObject(LoadObj_Data1, UnidusBalance_Main, obj);

	njPushMatrix(_nj_unit_matrix_);
	njTranslateEx(&data->Position);
	njRotateY(0, data->Rotation.y);
	njTranslateX(16.0f);
	njTranslateY(4.0f);
	njGetTranslation(0, &child->Data1->Position);
	njPopMatrixEx();

	child->DisplaySub = UnidusBalance_Display;

	Collision_Init(child, Unidus_Collision, Unidus_Collision_Length, 4);

	child->Data1->field_A = 0.0f;
}

void __cdecl HillBalance_Display(ObjectMaster* obj) {
	if (!MissedFrames) {
		EntityData1* data = obj->Data1;

		njSetTexture(&HillTopOBJ_TexList);
		njPushMatrixEx();
		njTranslateEx(&data->Position);
		njRotateY_(data->Rotation.y);

		njDrawModel_SADX(data->Object->basicdxmodel);

		njTranslateY(3.0f);
		njRotateZ_(data->Rotation.x);
		njDrawModel_SADX(data->Object->child->basicdxmodel);

		njPopMatrixEx();
	}
}

void __cdecl HillBalance_Main(ObjectMaster* obj) {
	if (!ClipSetObject(obj)) {
		EntityData1* data = obj->Data1;

		if (data->Action == 0) {
			if (Balance_IsOnBalance(data, 10.0f)) {
				data->Action = 1;
				obj->Child->Data1->Action = 1;
			}
		}
		else if (data->Action == 1) {
			if (data->Rotation.x < 0x880) {
				data->Rotation.x += 0x200;
			}
			else {
				data->Rotation.x = 0x880;
			}

			if (obj->Child->Data1->Action == 0) {
				int playerid = Balance_IsOnBalance(data, 15.0f);

				if (playerid) {
					CharObj2Ptrs[playerid - 1]->Speed.y = data->Scale.x;
					EntityData2Ptrs[playerid - 1]->VelocityDirection.y = data->Scale.x;
				}

				data->Action = 0;
				data->Rotation.x = -0x880;
			}
		}

		// Move Collision
		data->CollisionInfo->CollisionArray[1].angz = data->Rotation.x;

		RunObjectChildren(obj);
		AddToCollisionList(data);
		obj->DisplaySub(obj);
	}
}

void __cdecl HillBalance(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	
	data->Rotation.x = -0x880;
	data->Object = ht_balance->getmodel();

	Collision_Init(obj, arrayptrandlength(HillTBalance_Col), 4);

	obj->MainSub = HillBalance_Main;
	obj->DisplaySub = HillBalance_Display;

	LoadUnidusBalance(obj, data);
}

void HillBalance_LoadAssets() {
	LoadModelFile(&ht_balance, "ht_balance", ModelFormat_Basic);
}

void HillBalance_FreeAssets() {
	FreeModelFile(&ht_balance);
}