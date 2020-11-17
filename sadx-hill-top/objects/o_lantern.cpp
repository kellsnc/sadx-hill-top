#include "pch.h"
#include "o_lantern.h"

extern NJS_TEXLIST HillTopOBJ_TexList;

ModelInfo* ht_lantern = nullptr;

NJS_TEXANIM	FIREFLY_TEXANIM = { 0x1, 0x1, 0, 0, 0, 0, 0x100, 0x100, 7, 0x20 };
NJS_SPRITE FIREFLY_SPRITE = { { 0.0f, 0.0f, 0.0f }, 1.0f, 1.0f, 0, &HillTopOBJ_TexList, &FIREFLY_TEXANIM };

void __cdecl FireFly_Display(ObjectMaster* obj) {
	if (!MissedFrames) {
		EntityData1* data = obj->Data1;

		njSetTexture(LevelObjTexlists[1]);
		njPushMatrixEx();
		njTranslateEx(&data->Position);

		njRotateZ_(data->Rotation.z);
		njRotateX_(data->Rotation.x);
		njRotateY_(data->Rotation.y);

		njTranslateX(data->Scale.y);

		// Adapt to camera view
		njRotateZ_(-data->Rotation.z);
		njRotateX_(-data->Rotation.x);
		njRotateY_(-data->Rotation.y);

		njRotateY_(Camera_Data1->Rotation.y);
		njRotateX_(Camera_Data1->Rotation.x);

		njDrawSprite3D(&FIREFLY_SPRITE, 0, NJD_SPRITE_ALPHA);

		njPopMatrixEx();
	}
}

void __cdecl FireFly_Main(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;

	data->Rotation.y += data->field_A;

	obj->DisplaySub(obj);
}

void __cdecl FireFly_Load(ObjectMaster* parent, int count) {
	for (int i = 0; i < count; ++i) {
		ObjectMaster* child = LoadChildObject(LoadObj_Data1, FireFly_Main, parent);
		EntityData1* data = child->Data1;
		
		data->Scale.y = 4.0f + static_cast<Float>(rand() % 5);
		data->field_A = 500 + rand() % 250;

		data->Rotation.x = rand();
		data->Rotation.z = rand();

		child->MainSub = FireFly_Main;
		child->DisplaySub = FireFly_Display;
	}
}

void __cdecl Lantern_Display(ObjectMaster* obj) {
	if (!MissedFrames) {
		EntityData1* data = obj->Data1;

		njSetTexture(LevelObjTexlists[1]);
		njPushMatrixEx();
		njTranslateEx(&data->Position);
		njRotateEx((Angle*)&data->Rotation, false);
		
		Float rot = 1.0f - njSin(data->field_A) * 500.0f;

		njRotateX_(static_cast<int>(rot));

		njDrawModel_SADX(data->Object->basicdxmodel);

		njPopMatrixEx();
	}
}

void __cdecl Lantern_Main(ObjectMaster* obj) {
	if (!ClipSetObject(obj)) {
		EntityData1* data = obj->Data1;
		
		// Animate
		data->field_A += 0x300;

		RunObjectChildren(obj);
		obj->DisplaySub(obj);
	}
}

void __cdecl HillLantern(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	
	data->field_A = rand(); // randomize animation start
	data->Object = ht_lantern->getmodel();

	if (data->Scale.x) {
		FireFly_Load(obj, static_cast<int>(data->Scale.x) % 4);
	}

	obj->MainSub = Lantern_Main;
	obj->DisplaySub = Lantern_Display;
}

void Lantern_LoadAssets() {
	LoadModel(&ht_lantern, "ht_lantern", ModelFormat_Basic);
}