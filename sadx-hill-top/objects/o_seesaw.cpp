#include "pch.h"
#include "o_seesaw.h"

/*

Unidus seesaw

ScaleX: Height power

*/

ModelInfo* ht_seesaw = nullptr;

CollisionData HillSeesaw_Col[] = {
	{ 0, CollisionShape_Capsule2, 0x77, 0, 0, {0.0f, 1.3f, 0.0f}, 1.3f, 4.5f, 0, 0, 0x4000, 0, 0 },
	{ 0, CollisionShape_Cube2, 0x77, 0, 0, {0.0f, 3.0f, 0.0f}, 18.0f, 0.35f, 3.7f, 0, 0, 0, 0 }
};

bool IsOnSeesaw(EntityData1* data, NJS_VECTOR* pos) {
	NJS_VECTOR newpos = { 0 };

	njPushMatrix(_nj_unit_matrix_);
	njTranslateEx(&data->Position);
	njRotateY_(data->Rotation.y);
	njRotateZ_(data->Rotation.x);
	njTranslateX(-16.0f);
	njTranslateY(10.0f);
	njGetTranslation(0, &newpos);
	njPopMatrixEx();

	return IsPointInsideSphere(&newpos, pos, 12.0f);
}

void Seesaw_Check(ObjectMaster* obj, EntityData1* player) {
	EntityData1* data = obj->Data1;

	if (player->Action == 2 && IsOnSeesaw(data, &player->Position)) {
		data->Action = 1;
		obj->Child->Data1->Action = 1;
	}
}

void Seesaw_Launch(ObjectMaster* obj, EntityData1* player) {
	EntityData1* data = obj->Data1;

	if (IsOnSeesaw(data, &player->Position)) {
		CharObj2Ptrs[player->CharIndex]->Speed.y = data->Scale.x;
		EntityData2Ptrs[player->CharIndex]->VelocityDirection.y = data->Scale.x;
		PlaySound3D(458, nullptr, 0, 100, 120, data);
	}
}

void __cdecl UnidusSeesaw_Display(ObjectMaster* obj) {
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

void __cdecl UnidusSeesaw_Main(ObjectMaster* obj) {
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

void LoadUnidusSeesaw(ObjectMaster* obj, EntityData1* data) {
	ObjectMaster* child = LoadChildObject(LoadObj_Data1, UnidusSeesaw_Main, obj);

	njPushMatrix(_nj_unit_matrix_);
	njTranslateEx(&data->Position);
	njRotateY(0, data->Rotation.y);
	njTranslateX(16.0f);
	njTranslateY(4.0f);
	njGetTranslation(0, &child->Data1->Position);
	njPopMatrixEx();

	child->DisplaySub = UnidusSeesaw_Display;

	Collision_Init(child, Unidus_Collision, Unidus_Collision_Length, 4);

	child->Data1->field_A = 0.0f;
}

void __cdecl HillSeesaw_Display(ObjectMaster* obj) {
	if (!MissedFrames) {
		EntityData1* data = obj->Data1;

		njSetTexture(&HillTopOBJ_TexList);
		njPushMatrixEx();
		njTranslateEx(&data->Position);
		njRotateY_(data->Rotation.y);

		DrawModel(data->Object->basicdxmodel);

		njTranslateY(3.0f);
		njRotateZ_(data->Rotation.x);
		DrawModel(data->Object->child->basicdxmodel);

		njPopMatrixEx();
	}
}

void __cdecl HillSeesaw_Main(ObjectMaster* obj) {
	if (!ClipSetObject(obj)) {
		EntityData1* data = obj->Data1;


		IsOnSeesaw(data, &data->Position);

		if (data->Action == 0) {
			ForEveryCollidingPlayer(obj, Seesaw_Check);
		}
		else if (data->Action == 1) {
			if (data->Rotation.x < 0x880) {
				data->Rotation.x += 0x200;
			}
			else {
				data->Rotation.x = 0x880;
			}

			if (obj->Child->Data1->Action == 0) {
				ForEveryCollidingPlayer(obj, Seesaw_Launch);
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

void __cdecl HillSeesaw(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	
	data->Rotation.x = -0x880;
	data->Object = ht_seesaw->getmodel();

	Collision_Init(obj, arrayptrandlength(HillSeesaw_Col), 4);

	obj->MainSub = HillSeesaw_Main;
	obj->DisplaySub = HillSeesaw_Display;

	LoadUnidusSeesaw(obj, data);
}

void HillSeesaw_LoadAssets() {
	LoadModelFile(&ht_seesaw, "ht_seesaw", ModelFormat_Basic);
}

void HillSeesaw_FreeAssets() {
	FreeModelFile(&ht_seesaw);
}