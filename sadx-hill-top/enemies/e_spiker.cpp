#include "pch.h"
#include "e_spiker.h"

NJS_TEXNAME SPIKER_TexNames[5];
NJS_TEXLIST SPIKER_TexList = { arrayptrandlength(SPIKER_TexNames) };

ModelInfo* e_spiker = nullptr;

AnimationFile* e_spiker_stand = nullptr;
AnimationFile* e_spiker_walk = nullptr;

CollisionData Spiker_Col = { 0, CollisionShape_Sphere, 0x10, 0x21, 0x400, { 0.0, 8.0f, 0.0 }, 4.5f, 0.0f, 0.0f, 0, 0, 0, 0 };
CollisionData Spike_Col = { 0, CollisionShape_Sphere, 0x77, 0x2F, 0, { 0.0, 20.0f, 0.0 }, 4.5f, 6.0f, 0.0f, 0, 0, 0, 0 };

NJS_ACTION SpikerActions[2] = { 0 };

enum class SpikerAnims {
	Stand,
	Walk
};

enum SpikerActions {
	SpikerAct_Stand,
	SpikerAct_Walk,
	SpikerAct_WalkToPlayer,
	SpikerAct_Attack,
	SpikerAct_RunningAway,
	SpikerAct_Destroyed
};

#pragma region Spike

void __cdecl SpikerSpike_Display(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	EntityData1* pdata = obj->Parent->Data1;
	enemywk* enmwk = (enemywk*)obj->Parent->Data2;

	if (!MissedFrames && data->Action != 0) {
		njSetTexture(&SPIKER_TexList);
		njPushMatrixEx();
		njTranslateEx(&pdata->Position);
		njRotateY_(pdata->Rotation.y);
		njRotateX_(pdata->Rotation.x);
		njTranslateY(9.0f);
		njDrawModel_SADX(data->Object->basicdxmodel);
		njPopMatrixEx();
	}
}

void __cdecl SpikerSpike_Main(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	EntityData1* pdata = obj->Parent->Data1;
	enemywk* enmwk = (enemywk*)obj->Parent->Data2;

	AddToCollisionList(data);
	obj->DisplaySub(obj);
}

void LoadSpikerSpike(ObjectMaster* obj, EntityData1* data) {
	ObjectMaster* spike = LoadChildObject(LoadObj_Data1, SpikerSpike_Main, obj);

	spike->Data1->Object = (NJS_OBJECT*)e_spiker->getdata("Spike");
	spike->DisplaySub = SpikerSpike_Display;

	Collision_Init(spike, &Spike_Col, 1, 3);
}

#pragma endregion

#pragma region Spiker

void Spiker_SetAnim(enemywk* enmwk, SpikerAnims anim) {
	enmwk->actp = &SpikerActions[static_cast<int>(anim)];
}

void Spiker_ActionStand(EntityData1* data, enemywk* enmwk) {
	if (IsPlayerInRange(&data->Position, enmwk->hear_range / 2)) {
		enmwk->old_mode = SpikerAct_Stand; // go back to this action when attack is finished
		data->Action = SpikerAct_Attack;
	}
}

void Spiker_ActionDelete(ObjectMaster* obj, EntityData1* data) {
	LoadEnemyExplosion(data->Position.x, data->Position.y + 5.0f, data->Position.z, 1.4f);
	SpawnAnimal(3, data->Position.x, data->Position.y + 10.0f, data->Position.y);
	SetEmeraldObtained(data->Rotation.z, &data->Position);
	UpdateSetDataAndDelete(obj);
	DeleteGammaMissileIfNoTarget(obj);
}

void __cdecl Spiker_Display(ObjectMaster* obj) {
	if (!MissedFrames) {
		EntityData1* data = obj->Data1;
		enemywk* enmwk = (enemywk*)obj->Data2;

		njSetTexture(&SPIKER_TexList);
		njPushMatrixEx();
		njTranslateEx(&data->Position);
		njRotateEx((Angle*)&data->Rotation, false);

		if (data->Action != SpikerAct_RunningAway) {
			data->Object->child->evalflags &= ~NJD_EVAL_HIDE;
		}
		else {
			data->Object->child->evalflags |= NJD_EVAL_HIDE;
		}

		if (IsPlayerInRange(&data->Position, enmwk->view_range)) {
			njAction(enmwk->actp, enmwk->pframe);
		}
		else {
			DrawObject(data->Object);
		}
		
		njPopMatrixEx();
	}
}

void __cdecl Spiker_Main(ObjectMaster* obj) {
	if (!ClipSetObject(obj)) {
		EntityData1* data = obj->Data1;
		enemywk* enmwk = (enemywk*)obj->Data2;

		// Animate
		enmwk->pframe += 1.0f;

		// Run most things when in a certain range
		if (IsPlayerInRange(&data->Position, enmwk->view_range)) {
			switch (data->Action) {
			case SpikerAct_Stand:
				Spiker_ActionStand(data, enmwk);
				break;
			case SpikerAct_Destroyed:
				Spiker_ActionDelete(obj, data);
				return;
			}

			if (OhNoImDead(data, (ObjectData2*)enmwk)) {
				data->Action = SpikerAct_Destroyed;
				Collision_Free(obj);
			}

			Object_CheckEmerald(data->Rotation.z, &data->Position);
			ObjectData2_SetPreviousPosition(data, (ObjectData2*)enmwk);
			AddToCollisionList(data);
			RunObjectChildren(obj);
		}
		
		// Display
		obj->DisplaySub(obj);
	}
}

void __cdecl Spiker(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	enemywk* enmwk = (enemywk*)AllocateObjectData2(obj, data); // new structure from the debug symbols

	Collision_Init(obj, &Spiker_Col, 1, 3);
	
	// some enemy information

	enmwk->height = 0.0;
	enmwk->view_range = 160000.0f;
	enmwk->hear_range = 78400.0f;
	enmwk->colli_top = 5.0f;
	enmwk->colli_radius = 5.0f;
	enmwk->aim_angle = data->Rotation.y;
	enmwk->colli_center.x = 0.0f;
	enmwk->colli_center.y = 0.0f;
	enmwk->colli_center.z = 0.0f;
	enmwk->colli_bottom = 0.0f;
	enmwk->bound_side = 1.0f;
	enmwk->bound_floor = 1.0f;
	enmwk->bound_ceiling = 1.0f;
	enmwk->shadow_scl = 1.6;
	enmwk->shadow_scl_ratio = 1.0;
	enmwk->sub_angle[0] = (rand() * 0.000030517578 * 360.0 * 65536.0 * 0.002777777777777778);
	enmwk->weight = 0.1f;

	float posy = DrawShadow(data, 0.0); // also returns the floor position

	if (posy > -100000) {
		data->Position.y = posy;
	}

	ObjectData2_SetStartPosition(data, (ObjectData2*)enmwk);

	// Randomize animation start
	data->Scale.z = rand();
	data->Object = e_spiker->getmodel();

	Spiker_SetAnim(enmwk, SpikerAnims::Stand);

	obj->DeleteSub = Enemy_Delete;
	obj->MainSub = Spiker_Main;
	obj->DisplaySub = Spiker_Display;

	Object_CheckEmerald(data->Rotation.z, &data->Position); // signals position to the radar, rotz is emerald id

	LoadSpikerSpike(obj, data);
}

#pragma endregion

void Spiker_LoadAssets() {
	LoadModel(&e_spiker, "e_spiker", ModelFormat_Basic);
	LoadAnimation(&e_spiker_stand, "e_spiker_stand");
	LoadAnimation(&e_spiker_walk, "e_spiker_walk");

	NJS_OBJECT* object = e_spiker->getmodel();

	SpikerActions[0].object = object;
	SpikerActions[0].motion = e_spiker_stand->getmotion();

	SpikerActions[1].object = object;
	SpikerActions[1].motion = e_spiker_walk->getmotion();
}