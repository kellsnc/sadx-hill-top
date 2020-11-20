#include "pch.h"
#include "e_spiker.h"

/*

SPIKER ENEMY

Rot X = Floor (0) or Ceiling (1)
Rot Z = Emerald ID (for Knuckles) -> Index
Scale X = Walking radius (if 0, doesn't move)
Scale Y = Attack type (0 is homing missile; 1 is vertical attack);
Scale Z = Attack radius (cylinder)

*/

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

enum class SpikerActs : Uint32 {
	Stand,
	Walk,
	WalkToPlayer,
	Attack,
	RunningAway,
	Destroyed
};

enum SpikerAttacks {
	SpikerAttack_Homing,
	SpikerAttack_Vertical
};

struct SpikerData1 {
	SpikerActs Action;
	Uint32 EmeraldID;
	Bool Ceiling;
	NJS_OBJECT* Object;
	Bool SpikeReleased;
	Rotation3 Rotation;
	NJS_VECTOR Position;
	Float WalkRadius;
	Float AttackType;
	Float AttackRadius;
	CollisionInfo* CollisionInfo;
};

#pragma region Spike

void __cdecl SpikerSpike_Display(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	EntityData1* pdata = obj->Parent->Data1;
	enemywk* enmwk = (enemywk*)obj->Parent->Data2;

	if (!MissedFrames && data->Action != 0) {
		njSetTexture(&SPIKER_TexList);
		njPushMatrixEx();
		njTranslateEx(&data->Position);
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

	data->Position = pdata->Position;

	AddToCollisionList(data);
	obj->DisplaySub(obj);
}

void LoadSpikerSpike(ObjectMaster* obj, SpikerData1* data) {
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

void Spiker_MoveForward(SpikerData1* data, float speed) {
	njPushMatrix(_nj_unit_matrix_);
	njTranslateEx(&data->Position);
	njRotateY(0, data->Rotation.y + 0x8000);
	njTranslateX(-speed);
	njGetTranslation(0, &data->Position);
	njPopMatrixEx();
}

bool Spiker_AttachFloor(SpikerData1* data) {
	Rotation3 rot = { 0, data->Rotation.y, 0 };
	float posy = GetGroundYPosition(data->Position.x, data->Position.y + 5.0f, data->Position.z, &rot);

	if (posy > -100000) {
		data->Position.y = posy;
		data->Rotation = rot;
	}
	else {
		return false; // no floor detected
	}

	return true;
}

bool Spiker_CanAttack(SpikerData1* data) {
	return data->SpikeReleased == false;
}

void Spiker_ActionStand(SpikerData1* data, enemywk* enmwk) {
	// Attack if finds player in range
	if (Spiker_CanAttack(data) == true && IsPlayerInsideSphere_(&data->Position, data->AttackRadius)) {
		enmwk->old_mode = static_cast<int>(SpikerActs::Stand); // go back to stand action when attack is finished
		data->Action = SpikerActs::Attack;
	}
}

void Spiker_ActionWalk(SpikerData1* data, enemywk* enmwk) {
	// Move forward
	Spiker_MoveForward(data, 0.1f);

	// Turn around if out of range, or no floor
	if (IsPointInsideSphere(&enmwk->home, &data->Position, data->WalkRadius) == false || !Spiker_AttachFloor(data)) {
		njLookAt(&data->Position, &enmwk->home, nullptr, &data->Rotation.y);
		data->Rotation.y += 0x4000;
	}

	// Attack if finds player in range
	if (Spiker_CanAttack(data) == true && IsPlayerInsideSphere_(&data->Position, data->AttackRadius)) {
		enmwk->old_mode = static_cast<int>(SpikerActs::Walk); // go back to walk action when attack is finished
		data->Action = SpikerActs::WalkToPlayer;
	}
}

void Spiker_ActionWalkToPlayer(SpikerData1* data, enemywk* enmwk) {
	// Move forward
	Spiker_MoveForward(data, 0.2f);

	ObjectData2_LookAtPlayer((EntityData1*)data, (ObjectData2*)enmwk, GetClosestPlayerID(&data->Position));

	if (IsPlayerInsideSphere_(&data->Position, data->AttackRadius) == false || Spiker_AttachFloor(data) == false || Spiker_CanAttack(data) == false) {
		data->Action = static_cast<SpikerActs>(enmwk->old_mode);
	}

	if (IsPlayerInsideSphere_(&data->Position, data->AttackRadius / 4)) {
		data->Action = SpikerActs::Attack;
	}
}

void Spiker_ActionDelete(ObjectMaster* obj, SpikerData1* data) {
	LoadEnemyExplosion(data->Position.x, data->Position.y + 5.0f, data->Position.z, 1.4f);
	SpawnAnimal(3, data->Position.x, data->Position.y + 10.0f, data->Position.z);
	SetEmeraldObtained(data->Rotation.z, &data->Position);
	UpdateSetDataAndDelete(obj);
	DeleteGammaMissileIfNoTarget(obj);
}

void __cdecl Spiker_Display(ObjectMaster* obj) {
	if (!MissedFrames) {
		SpikerData1* data = (SpikerData1*)obj->Data1;
		enemywk* enmwk = (enemywk*)obj->Data2;

		njSetTexture(&SPIKER_TexList);
		njPushMatrixEx();
		njTranslateEx(&data->Position);

		// Rotate with respect to the ground if on the floor, otherwise reverse
		if (data->Ceiling == true) {
			njRotateY_(data->Rotation.y);
			njRotateX(0, 0x8000);
		}
		else {
			njRotateEx((Angle*)&data->Rotation, false);
		}

		njRotateY(0, 0x8000);
		
		// Hide the animated spike if it's released
		if (data->SpikeReleased == false) {
			data->Object->child->evalflags &= ~NJD_EVAL_HIDE;
		}
		else {
			data->Object->child->evalflags |= NJD_EVAL_HIDE;
		}

		// Run animation only if in range or visible
		if (IsPlayerInRange(&data->Position, enmwk->view_range) && IsVisible(&data->Position, 40.0f)) {
			njAction(enmwk->actp, enmwk->pframe);
		}
		else {
			DrawObject(data->Object);
		}
		
		njPopMatrixEx();

		// Draw the shadow if on the ground
		if (data->Ceiling == false) {
			DrawShadow((EntityData1*)data, enmwk->shadow_scl);
		}
	}
}

void __cdecl Spiker_Main(ObjectMaster* obj) {
	if (!ClipSetObject(obj)) {
		SpikerData1* data = (SpikerData1*)obj->Data1;
		enemywk* enmwk = (enemywk*)obj->Data2;

		// Animate
		enmwk->pframe += 1.0f;

		// Run most things when in a certain range
		if (IsPlayerInRange(&data->Position, enmwk->view_range)) {
			switch (data->Action) {
			case SpikerActs::Stand:
				Spiker_ActionStand(data, enmwk);
				break;
			case SpikerActs::Walk:
				Spiker_ActionWalk(data, enmwk);
				break;
			case SpikerActs::WalkToPlayer:
				Spiker_ActionWalkToPlayer(data, enmwk);
				break;
			case SpikerActs::Destroyed:
				Spiker_ActionDelete(obj, data);
				return;
			}

			// If the object is killed by something
			if (OhNoImDead((EntityData1*)data, (ObjectData2*)enmwk)) {
				data->Action = SpikerActs::Destroyed;
				return;
			}

			Object_CheckEmerald(data->EmeraldID, &data->Position);
			ObjectData2_SetPreviousPosition((EntityData1*)data, (ObjectData2*)enmwk);
			AddToCollisionList((EntityData1*)data);
			RunObjectChildren(obj);
		}
		
		// Display
		obj->DisplaySub(obj);
	}
}

void __cdecl Spiker(ObjectMaster* obj) {
	SpikerData1* data = (SpikerData1*)obj->Data1;
	enemywk* enmwk = (enemywk*)AllocateObjectData2(obj, (EntityData1*)data); // new structure from the debug symbols

#ifdef _DEBUG
	data->Rotation.x = 0; // floor or ceiling
	data->WalkRadius = 50.0f;
	data->AttackType = 0.0f;
	data->AttackRadius = 200.0f;
#endif // _DEBUG

	Collision_Init(obj, &Spiker_Col, 1, 3);
	
	// some enemy information

	enmwk->height = 20.0f;
	enmwk->view_range = 160000.0f;
	enmwk->hear_range = 78400.0f;
	enmwk->colli_top = 5.0f;
	enmwk->colli_radius = 5.0f;
	enmwk->aim_angle = data->Rotation.y;
	enmwk->sub_angle[0] = data->Rotation.y;
	enmwk->colli_center.x = 0.0f;
	enmwk->colli_center.y = 0.0f;
	enmwk->colli_center.z = 0.0f;
	enmwk->colli_bottom = 0.0f;
	enmwk->bound_side = 1.0f;
	enmwk->bound_floor = 1.0f;
	enmwk->bound_ceiling = 1.0f;
	enmwk->shadow_scl = 1.3f;
	enmwk->weight = 0.1f;
	
	// Randomize animation start
	enmwk->pframe = rand();
	data->Object = e_spiker->getmodel();

	obj->DeleteSub = Enemy_Delete;
	obj->MainSub = Spiker_Main;
	obj->DisplaySub = Spiker_Display;

	data->EmeraldID = data->Rotation.z;
	Object_CheckEmerald(data->EmeraldID, &data->Position); // signals position to the radar, rotz is emerald id

	LoadSpikerSpike(obj, data);

	// floor or ceiling
	data->Ceiling = data->Rotation.x % 2;

	// If the enemy is on the floor, attach
	if (data->EmeraldID == false) {
		Spiker_AttachFloor(data);
	}

	enmwk->home = data->Position;

	// Set up start action based on set information
	if (data->WalkRadius) {
		data->Action = SpikerActs::Walk;
		Spiker_SetAnim(enmwk, SpikerAnims::Walk);
	}
	else {
		data->Action = SpikerActs::Stand;
		Spiker_SetAnim(enmwk, SpikerAnims::Stand);
	}

	data->Rotation.y += 0x8000; // reverse rotation y as calculations are done globally
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