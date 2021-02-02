#include "pch.h"
#include "e_subeggman.h"
#include "lava.h"

static int HillTopBossMusic;

ModelInfo* ht_bosslava = nullptr;

void __cdecl BossLava_Delete(ObjectMaster* obj) {
	// Removes the dyncol before deleting the object

	if (obj->Data1->LoopData) {
		DynamicCOL_Remove(obj, (NJS_OBJECT*)obj->Data1->LoopData);
		ObjectArray_Remove((NJS_OBJECT*)obj->Data1->LoopData);
	}
}

void __cdecl BossLava_Display(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;

	if (!MissedFrames) {
		njSetTexture(&CurrentLavaTex);
		njPushMatrixEx();
		njDrawModel_SADX(data->Object->basicdxmodel);
		njPopMatrixEx();
	}
}

void __cdecl BossLava_Main(ObjectMaster* obj) {
	obj->DisplaySub(obj);
}

void __cdecl BossLava(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	
	data->Object = ht_bosslava->getmodel();

	// Create the dynamic collision
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

	DynamicCOL_Add((ColFlags)(0x08004000 | ColFlags_Solid | ColFlags_Hurt), obj, object);

	data->LoopData = (Loop*)object;

	obj->DeleteSub = BossLava_Delete;
	obj->DisplaySub = BossLava_Display;
	obj->MainSub = BossLava_Main;
}

void __cdecl SubEggman_Delete(ObjectMaster* obj) {

}

void __cdecl SubEggman_Display(ObjectMaster* obj) {

}

void __cdecl SubEggman_Main(ObjectMaster* obj) {

}

void __cdecl SubEggman(ObjectMaster* obj) {
	DisplayBossName2("Eggman", -1, 240, 120);

	ObjectMaster* musicobj = LoadObject(LoadObj_Data1, 1, LoadMusic_EventDelayed);
	musicobj->Data1->Action = HillTopBossMusic;
	musicobj->Data1->InvulnerableTime = 100;

	obj->MainSub = SubEggman_Main;
	obj->DeleteSub = SubEggman_Delete;
	obj->DisplaySub = SubEggman_Display;
}

void __cdecl Boss_SubEggman_Main(ObjectMaster* obj) {
	SetCameraControlEnabled(0);
}

void __cdecl Boss_SubEggman_Init(ObjectMaster* obj) {
	LoadObject(LoadObj_Data1, 2, BossLava);
	LoadObject(LoadObj_Data1, 2, SubEggman);
	
	CharObj2* co2 = CharObj2Ptrs[0];

	if (co2) {
		co2->Speed = { 4, 4, 0 };
		co2->field_A = 100; // no control timer

		obj->MainSub = Boss_SubEggman_Main;
	}
}

void Boss_LoadAssets() {
	LoadModelFile(&ht_bosslava, "ht_bosslava", ModelFormat::ModelFormat_Basic);
}

void Boss_FreeAssets() {
	FreeModelFile(&ht_bosslava);
}

void Boss_Init(const HelperFunctions& helperFunctions) {
	HillTopBossMusic = helperFunctions.RegisterMusicFile({ "hilltopboss", true });
}