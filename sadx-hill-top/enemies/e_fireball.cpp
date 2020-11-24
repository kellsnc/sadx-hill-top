#include "pch.h"
#include "e_fireball.h"
#include "..\objects\o_lavafall.h"

ModelInfo* e_fireball = nullptr;

CollisionData FireBall_Col = { 0, CollisionShape_Sphere, 0x77, 0x2F, 0, { 0.0f, 1.0f, 0.0f }, 1.0f, 0.0f, 0.0f, 0, 0, 0, 0 };

void __cdecl FireBall_Display(ObjectMaster* obj) {
	if (!MissedFrames) {
		EntityData1* data = obj->Data1;

		njSetTexture(&LAVAFALL_TexList);
		njPushMatrixEx();
		njTranslateEx(&data->Position);
		njRotateY_(data->Rotation.y);
		njRotateX_(data->Rotation.x);
		njScalef(data->Scale.x);

		data->Object->basicdxmodel->mats->attr_texId = data->Index;
		njDrawModel_SADX(data->Object->basicdxmodel);

		njPopMatrixEx();
	}
}

void __cdecl FireBall_Main(ObjectMaster* obj) {
	if (!ClipSetObject(obj)) {
		EntityData1* data = obj->Data1;
		
		if (data->Index >= LAVAFALL_TexList.nbTexture - 1) {
			data->Index = 0;
		}
		else {
			++data->Index;
		}

		// Move
		njPushMatrix(_nj_unit_matrix_);
		njTranslateEx(&data->Position);
		njTranslateY(-(data->Scale.z / 4)); // grav
		njRotateY_(data->Rotation.y - 0x4000);
		njRotateX_(data->Rotation.x);

		// check direction
		if (data->Unknown == 0) {
			njTranslateX(data->Scale.y); // speed
		}
		else {
			njTranslateY(data->Scale.y);
		}
		
		njGetTranslation(nullptr, &data->Position);
		njPopMatrixEx();

		// update movement speed
		data->Scale.z *= 1.02;
		data->Scale.y *= 0.99;

		// Break
		if (GetCollidingEntityA(data) || GetCollidingEntityB(data)) {
			DeleteObject_(obj);
			return;
		}

		AddToCollisionList(data);
		obj->DisplaySub(obj);
	}
}

void __cdecl FireBall(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;

	if (data->Scale.x == 0.0f) {
		data->Scale.x = 1.0f;
	}

	Collision_Init(obj, &FireBall_Col, 1, 4);

	data->CollisionInfo->CollisionArray->a *= data->Scale.x;
	data->Object = e_fireball->getmodel();

	obj->MainSub = FireBall_Main;
	obj->DisplaySub = FireBall_Display;
}

void LoadFireBall(ObjectMaster* obj, NJS_VECTOR* position, Angle roty, Angle rotx, Float Speed, Float size, Float Grav, int dir) {
	ObjectMaster* child = LoadChildObject(LoadObj_Data1, FireBall, obj);
	EntityData1* data = child->Data1;

	data->Position = *position;
	data->Rotation.y = roty;
	data->Rotation.x = rotx;
	data->Scale.x = size;
	data->Scale.y = Speed;
	data->Scale.z = Grav;
	data->Unknown = dir;
}

/*

FIREBALL LAUNCHER ENEMY

FloatX = scale (if 0 > 1)
FloatY = launch speed (if 0 > 1)
FloatZ = spawn radius (if 0 spawn at position)
RotZ = Launch rate in frames (if 0 > 180)

*/

void FireBallLauncher_Main(ObjectMaster* obj) {
	if (!ClipSetObject(obj)) {
		EntityData1* data = obj->Data1;

		if (data->field_A <= 0) {
			NJS_VECTOR pos = data->Position;

			if (data->Scale.z != 0) {
				pos.x += (data->Scale.z / 2.0f) - static_cast<float>(rand() % static_cast<int>(data->Scale.z));
				pos.z += (data->Scale.z / 2.0f) - static_cast<float>(rand() % static_cast<int>(data->Scale.z));
			}
			
			LoadFireBall(obj, &pos, data->Rotation.y, data->Rotation.x, data->Scale.y, data->Scale.x, data->Scale.y / 2, 1);

			data->field_A = data->Rotation.z;
		}
		else {
			--data->field_A;
		}

		RunObjectChildren(obj);
	}
}

void FireBallLauncher(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;

	obj->MainSub = FireBallLauncher_Main;

	// Default values

	if (data->Scale.x == 0.0f) {
		data->Scale.x = 1.0f;
	}

	if (data->Scale.y == 0.0f) {
		data->Scale.y = 1.0f;
	}

	if (data->Rotation.z == 0) {
		data->Rotation.z = 180;
	}
}

void FireBall_LoadAssets() {
	LoadModel(&e_fireball, "e_fireball", ModelFormat_Basic);
}