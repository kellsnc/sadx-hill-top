#include "pch.h"
#include "o_hillbush.h"

/*

A simple bush object.

*/

ModelInfo* ht_bush = nullptr;

void __cdecl HillBush_Display(ObjectMaster* obj) {
	if (!MissedFrames) {
		EntityData1* data = obj->Data1;

		njSetTexture(&HillTopOBJ_TexList);
		njPushMatrixEx();
		njTranslateEx(&data->Position);

		// Draw root
		njPushMatrixEx(); {
			njRotateEx((Angle*)&data->Rotation, false);
			njDrawModel_SADX(data->Object->basicdxmodel);
			njPopMatrixEx();
		}
		
		njRotateX_(data->Rotation.x);
		njRotateZ_(data->Rotation.z);
		njTranslateY(data->Object->child->pos[1]);

		// Some animation
		Float rot = 1.0f - njSin(data->field_A) * 300.0f;
		njRotateX_(static_cast<int>(rot));
		njScaleY(1.0f + (1.0f - njSin(data->Status * 1500)) / 50.0f);

		njDrawModel_SADX(data->Object->child->basicdxmodel); // draw bush

		njPopMatrixEx();
	}
}

void __cdecl HillBush_Main(ObjectMaster* obj) {
	if (!ClipSetObject(obj)) {
		EntityData1* data = obj->Data1;
		
		// Animate
		data->field_A += 0x200;
		data->Status += 1;

		RunObjectChildren(obj);
		obj->DisplaySub(obj);
	}
}

void __cdecl HillBush(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	
	// Randomize animation start
	data->field_A = rand(); 
	data->Status = rand();

	data->Object = ht_bush->getmodel();

	obj->MainSub = HillBush_Main;
	obj->DisplaySub = HillBush_Display;
}

void HillBush_LoadAssets() {
	LoadModelFile(&ht_bush, "ht_bush", ModelFormat_Basic);
}

void HillBush_FreeAssets() {
	FreeModelFile(&ht_bush);
}