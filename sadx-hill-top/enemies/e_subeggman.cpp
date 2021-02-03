#include "pch.h"
#include "e_subeggman.h"
#include "lava.h"
#include "../objects/o_hillplatform.h"
#include "e_fireball.h"

static int HillTopBossMusic;

ModelInfo* ht_bosslava = nullptr;

ObjectMaster* CurrentBossLava = nullptr;
ObjectMaster* CurrentBoss = nullptr;
ObjectMaster* PlatformsHandlerPtr = nullptr;

#pragma region Lava

/*

Lava plane with vertex animation

*/

enum class LavaModes : short {
	Grow,
	Normal,
	Fast,
	Point
};

struct lavawk {
	LavaModes Mode;
	short nbPoint;
	short Status;
	short Timer;
	Float Speed;
	NJS_OBJECT* Object;
	Float* Offsets;
	Rotation3 Rotation;
	NJS_VECTOR Position;
	NJS_POINT2 Point;
	LavaModes PrevMode;
};

void SetLavaPoint(Float x, Float z) {
	if (CurrentBossLava) {
		lavawk* wk = (lavawk*)CurrentBossLava->Data1;
		wk->Point = { x, z };
		wk->PrevMode = wk->Mode;
		wk->Mode = LavaModes::Point;
	}
}

void SetLavaSpeed(Float speed) {
	if (CurrentBossLava) {
		lavawk* wk = (lavawk*)CurrentBossLava->Data1;
		wk->Speed = speed;
	}
}

void CalcLavaAnim(lavawk* wk) {
	int j = 1000;

	for (int i = 0; i < wk->nbPoint; ++i) {
		wk->Object->basicdxmodel->points[i].y = (1.0f - njSin(wk->Timer * 100 * wk->Offsets[i] + j)) * wk->Offsets[i];

		if (wk->Offsets[i] > 3.0f) {
			wk->Offsets[i] -= 0.035f;
		}

		j += 1000;
	}
}

void FindLavaPoint(lavawk* wk) {
	NJS_POINT3* points = wk->Object->basicdxmodel->points;

	float mindist = 10000.0f;
	int result = -1;

	for (int i = 0; i < wk->nbPoint; ++i) {
		float dist = sqrtf(powf(wk->Point.x - points[i].x, 2) + powf(wk->Point.y - points[i].z, 2));

		if (dist < mindist) {
			mindist = dist;
			result = i;
		}
	}

	if (result >= 0 && mindist < 50.0f) {
		wk->Offsets[result] = 6.0f;
	}
}

void RangeLavaSpeed(lavawk* wk, int speed) {
	if (wk->Speed > speed) {
		wk->Speed -= 0.1f;
	}
	else if (wk->Speed < speed) {
		wk->Speed += 0.1f;
	}
}

void __cdecl BossLava_Delete(ObjectMaster* obj) {
	lavawk* wk = (lavawk*)obj->Data1;

	if (wk->Object) {
		DynamicCOL_Remove(obj, wk->Object);
		ObjectArray_Remove(wk->Object);
	}

	CurrentBossLava = nullptr;
}

void __cdecl BossLava_Display(ObjectMaster* obj) {
	lavawk* wk = (lavawk*)obj->Data1;

	if (!MissedFrames) {
		njSetTexture(&CurrentLavaTex);
		njPushMatrixEx();
		njTranslateY(wk->Object->pos[1]);
		njDrawModel_SADX(wk->Object->basicdxmodel);
		njPopMatrixEx();
	}
}

void __cdecl BossLava_Main(ObjectMaster* obj) {
	lavawk* wk = (lavawk*)obj->Data1;

	CalcLavaAnim(wk);

	switch (wk->Mode) {
	case LavaModes::Grow:
		wk->Speed = 1.0f;

		if (wk->Object->pos[1] < 0.0f) {
			wk->Object->pos[1] += 0.5f;
		}
		else {
			wk->Mode = LavaModes::Normal;
		}

		break;
	case LavaModes::Normal:
		RangeLavaSpeed(wk, 2);
		break;
	case LavaModes::Point:
		RangeLavaSpeed(wk, 3);
		FindLavaPoint(wk);
		wk->Mode = wk->PrevMode;

		break;
	}

	wk->Timer += static_cast<int>(wk->Speed);

	obj->DisplaySub(obj);
}

void __cdecl BossLava(ObjectMaster* obj) {
	lavawk* wk = (lavawk*)obj->Data1;

	// Create the dynamic collision
	NJS_OBJECT* object = ObjectArray_GetFreeObject();

	object->pos[0] = 0;
	object->pos[1] = -100.0f;
	object->pos[2] = 0;

	object->ang[0] = 0;
	object->ang[1] = 0;
	object->ang[2] = 0;

	object->scl[0] = 1.0f;
	object->scl[1] = 1.0f;
	object->scl[2] = 1.0f;

	object->basicdxmodel = ht_bosslava->getmodel()->basicdxmodel;

	DynamicCOL_Add((ColFlags)(0x08004000 | ColFlags_Solid | ColFlags_Hurt), obj, object);

	wk->Object = object;
	wk->Mode = LavaModes::Grow;
	wk->nbPoint = wk->Object->basicdxmodel->nbPoint;
	wk->Offsets = new Float[wk->nbPoint];

	for (int i = 0; i < wk->nbPoint; ++i) {
		wk->Offsets[i] = 3.0f;
	}

	obj->DeleteSub = BossLava_Delete;
	obj->DisplaySub = BossLava_Display;
	obj->MainSub = BossLava_Main;

	CurrentBossLava = obj;
}
#pragma endregion

#pragma region Platforms
enum PlatformsActions {
	PlatformAct_Hidden,
	PlatformAct_Sink,
	PlatformAct_Emerge,
	PlatformAct_Float
};

void __cdecl PlatformChild_Delete(ObjectMaster* obj) {
	if (obj->Data1->Object) {
		DynamicCOL_Remove(obj, obj->Data1->Object);
		ObjectArray_Remove(obj->Data1->Object);
	}
}

void __cdecl PlatformChild_Display(ObjectMaster* obj) {
	if (!MissedFrames) {
		EntityData1* data = obj->Data1;
		EntityData1* parent = obj->Parent->Data1;

		if (parent->Action != PlatformAct_Hidden) {
			njSetTexture(&HillTop_TexList);
			njPushMatrixEx();
			njTranslate(_nj_current_matrix_ptr_, data->Position.x, data->Object->pos[1], data->Position.z);
			njRotateY_(data->Rotation.y);
			njScalef(data->Scale.x);
			njDrawModel_SADX(data->Object->basicdxmodel);
			njPopMatrixEx();
		}
	}
}

void __cdecl PlatformChild_Main(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	EntityData1* parent = obj->Parent->Data1;

	if (parent->Action != PlatformAct_Hidden) {
		data->Object->pos[1] = parent->Position.y + (1.0f - powf(njSin(FrameCounterUnpaused * data->Scale.z), 2.0f)) * data->Scale.y;
		obj->DisplaySub(obj);
	}
}

void __cdecl PlatformsHandler_Main(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;

	switch (data->Action) {
	case PlatformAct_Emerge:
		if (data->Position.y < 15) {
			data->Position.y += 0.5;
		}
		else {
			data->Action = PlatformAct_Float;
		}

		break;
	case PlatformAct_Sink:
		if (data->Position.y > -100) {
			data->Position.y -= 0.1f;
		}
		else {
			data->Action = PlatformAct_Hidden;
		}
	}

	RunObjectChildren(obj);
}

void __cdecl PlatformsHandler(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;

	data->Position.y = -100;

	for (int i = 0; i < 0x10000; ++i) {
		ObjectMaster* child = LoadChildObject(LoadObj_Data1, PlatformChild_Main, obj);

		child->DeleteSub = PlatformChild_Delete;
		child->DisplaySub = PlatformChild_Display;

		child->Data1->Position.x = njCos(i) * 225;
		child->Data1->Position.z = njSin(i) * 225;

		child->Data1->Scale.x = static_cast<float>(60 + rand() % 25) / 100;
		child->Data1->Scale.y = static_cast<float>(5 + rand() % 30);
		child->Data1->Scale.z = static_cast<float>(60 + rand() % 60);
		child->Data1->Rotation.y = static_cast<Angle>(atan2f(child->Data1->Position.x, child->Data1->Position.z) * 65536.0f * 0.1591549762031479f);

		NJS_OBJECT* object = ObjectArray_GetFreeObject();

		object->pos[0] = child->Data1->Position.x;
		object->pos[1] = child->Data1->Position.y;
		object->pos[2] = child->Data1->Position.z;

		object->ang[0] = 0;
		object->ang[1] = child->Data1->Rotation.y;
		object->ang[2] = 0;

		object->scl[0] = child->Data1->Scale.x;
		object->scl[1] = child->Data1->Scale.x;
		object->scl[2] = child->Data1->Scale.x;

		child->Data1->Object = object;
		child->Data1->Object->basicdxmodel = ht_platform->getmodel()->getbasicdxmodel();

		DynamicCOL_Add((ColFlags)(0x08000000 | ColFlags_Solid | ColFlags_UseRotation), obj, object);

		i += 0x1000;
	}

	obj->MainSub = PlatformsHandler_Main;
	PlatformsHandlerPtr = obj;
}

void EmergePlatforms() {
	if (PlatformsHandlerPtr && PlatformsHandlerPtr->Data1->Action != PlatformAct_Float) {
		PlatformsHandlerPtr->Data1->Action = PlatformAct_Emerge;
		SetLavaSpeed(20.0f);
	}
}

void SinkPlatforms() {
	if (PlatformsHandlerPtr && PlatformsHandlerPtr->Data1->Action != PlatformAct_Hidden) {
		PlatformsHandlerPtr->Data1->Action = PlatformAct_Sink;
		SetLavaSpeed(15.0f);
	}
}

#pragma endregion

#pragma region Cameras
void __cdecl EggmanBossCamera_Init(_OBJ_CAMERAPARAM*) {

}
#pragma endregion

#pragma region Attacks
NJS_VECTOR EggSub_GetAttackPoint(EntityData1* data) {
	NJS_VECTOR vec;
	
	njPushMatrix(_nj_unit_matrix_);
	njTranslateEx(&data->Position);
	njRotateY_(data->Rotation.y);
	njTranslate(_nj_current_matrix_ptr_, -20.0f, 50.0f, 0.0f);
	njGetTranslation(_nj_current_matrix_ptr_, &vec);
	njPopMatrixEx();

	return vec;
}

void EggSub_FireBall(EntityData1* data, Float scale) {
	NJS_VECTOR pos = EggSub_GetAttackPoint(data);
	Angle y;
	Angle x;

	njLookAt(&pos, &EntityData1Ptrs[GetClosestPlayerID(&pos)]->Position, &x, &y);
	LoadFireBall(CurrentBoss, &pos, y, -x, 5.0f, 4.0f, 0.0f, 0);
	// sound
}

#pragma endregion

#pragma region SubEggman
enum eggsubmainacts : Uint8 {
	eggsubmainact_init,
	eggsubmainact_run,
	eggsubmainact_death
};

enum class eggsubmtnacts : int {
	emerge,
	sink,
	stay,
	hidden,
	deflarg,
	bomb
};

enum class eggsubacts : int {
	act1,
	act2,
	act3,
	act4,
};

struct eggsubwk {
	bosswk bwk;
	int HitPoint;
	int HitTimer;
	int InternalTimer;
	eggsubacts Acts;
	eggsubmtnacts Subs;
	bool platformsLoaded;
	int lastattackscount;
	int MaxHealth;
	int Level;
};

CollisionData SubEgg_Col = { 0, CollisionShape_Sphere, 0x77, 0x20, 0x400, {0.0f, 0.0f, 0.0f}, 22.0f, 0.0f, 0.0f };

static constexpr float surfaceHeight = 0.0f;
static constexpr float sinkHeight = -50.0f;

void SubEgg_ChangeAction(eggsubwk* wk, eggsubacts action) {
	wk->Acts = action;
	wk->InternalTimer = 0;
}

void SubEgg_ChangeSub(eggsubwk* wk, eggsubmtnacts action) {
	wk->Subs = action;
}

bool SubEgg_CheckDamage(EntityData1* data, eggsubwk* wk) {
	if (wk->HitPoint > 0 && wk->HitTimer == 0) {
		EntityData1* entity = GetCollidingEntityA(data);

		bool playerHurting = entity && entity->Status & (Status_Attack | Status_Ball);

		if (playerHurting) {
			NJS_VECTOR spd = { -3.5f, 3.5f, 0.0f };
			PlayerDirectionToVector(entity, &spd);
			EnemyBounceThing(entity->CharIndex, spd.x, spd.y, spd.z);
			CharObj2Ptrs[entity->CharIndex]->field_A = 10;
		}

		if (playerHurting || GetCollidingEntityB(data)) {
			wk->HitTimer = 50;
			wk->HitPoint -= 1;
			PlaySound(463, 0, 0, 0);
			SetLavaSpeed(15.0f);

			if (wk->HitPoint == 0) {
				NonStaticFunctionPointer(void, sub_574460, (NJS_VECTOR * pos), 0x574460);
				sub_574460(&data->Position); // explosion effect
				wk->HitTimer = 200;
				data->Action = eggsubmainact_death;
				EmergePlatforms();

			}
			else if (wk->HitPoint == 1) {
				PlayVoiceCheckSetting(176);
			}
			else {
				PlayVoiceCheckSetting(172);
			}

			return true;
		}
	}

	return false;
}

void SubEgg_UpdateStatus(EntityData1* data, eggsubwk* wk) {
	data->Status &= ~(Status_Hurt | Status_Attack);

	if (wk->HitTimer) {
		--wk->HitTimer;
	}

	if (wk->HitPoint) {
		float p = static_cast<float>(wk->HitPoint) / static_cast<float>(wk->MaxHealth);

		if (p >= 1.0f) {
			wk->Level = 0;
		}
		else if (p >= 0.8f) {
			wk->Level = 1;
		}
		else if (p >= 0.6f) {
			wk->Level = 2;
		}
		else {
			wk->Level = 3;
		}
	}
}

bool SubEgg_Sink(EntityData1* data, float speed) {
	if (data->Position.y > sinkHeight) {
		data->Position.y -= speed;
	}
	else {
		return true;
	}

	return false;
}

bool SubEgg_Emerge(EntityData1* data, float speed) {
	if (data->Position.y < surfaceHeight) {
		data->Position.y += speed;
	}
	else {
		return true;
	}

	return false;
}

void SubEgg_LookAtPlayer(EntityData1* data, eggsubwk* wk) {
	EntityData1* entity = EntityData1Ptrs[GetClosestPlayerID(&data->Position)];

	njLookAt(&data->Position, &entity->Position, nullptr, &data->Rotation.y);
	data->Rotation.y += 0x4000;
}

// Get a random position around the lava outer circle
void SubEgg_GetRandomPosition(EntityData1* data, eggsubwk* wk) {
	int ang = rand() % 0x10000;
	float dist = 190;

	// Every 200 bams degrees
	if (ang % 0x200) {
		ang = ang + (0x200 - ang % 0x200);
	}

	if (rand() % 2 == 0) {
		dist = 260;
	}

	data->Position.x = njCos(ang) * dist;
	data->Position.y = sinkHeight;
	data->Position.z = njSin(ang) * dist;

	data->Rotation.y = static_cast<Angle>(atan2f(data->Position.x, data->Position.z) * 65536.0f * 0.1591549762031479f);
	data->Rotation.y += dist == 190 ? 0xC000 : 0x4000;
}

// Goes out of lava, shoots and turn toward player until touched, then act 2
void SubEgg_Act1(EntityData1* data, eggsubwk* wk) {
	switch (wk->Subs) {
	case eggsubmtnacts::hidden:
		SinkPlatforms();

		if (wk->HitPoint == 5) {
			data->Position = { 0, sinkHeight, 0 };
		}
		else {
			SubEgg_GetRandomPosition(data, wk);
		}

		SetLavaPoint(data->Position.x, data->Position.z);

		SubEgg_ChangeSub(wk, eggsubmtnacts::emerge);
		break;
	case eggsubmtnacts::emerge:
	default:
		if (SubEgg_Emerge(data, 0.2f)) {
			SubEgg_ChangeSub(wk, eggsubmtnacts::stay);
		}

		SubEgg_LookAtPlayer(data, wk);

		break;
	case eggsubmtnacts::stay:
		if (SubEgg_CheckDamage(data, wk)) {
			SubEgg_ChangeSub(wk, eggsubmtnacts::sink);
			EmergePlatforms();
			break;
		}

		SubEgg_LookAtPlayer(data, wk);

		if (++wk->InternalTimer % (wk->Level == 0 ? 80 : 45) == 0 || wk->InternalTimer == 1) {
			EggSub_FireBall(data, wk->Level == 0 ? 3.5f : 4.5f);
		}

		break;
	case eggsubmtnacts::sink:
		if (SubEgg_Sink(data, 0.1f)) {
			SubEgg_ChangeSub(wk, eggsubmtnacts::hidden);
			SubEgg_ChangeAction(wk, eggsubacts::act2);
		}
	}
}

// Spawns away from the player, launches persistent fire on ground
void SubEgg_Act2(EntityData1* data, eggsubwk* wk) {
	int speed;

	switch (wk->Subs) {
	case eggsubmtnacts::hidden:

		EmergePlatforms();

		SubEgg_GetRandomPosition(data, wk);

		while (IsSpecificPlayerInSphere(&data->Position, 150.0f, 0)) {
			SubEgg_GetRandomPosition(data, wk);
		}

		SetLavaPoint(data->Position.x, data->Position.z);

		SubEgg_ChangeSub(wk, eggsubmtnacts::emerge);
		break;
	case eggsubmtnacts::emerge:
	default:
		if (SubEgg_Emerge(data, 0.5f)) {
			SubEgg_ChangeSub(wk, eggsubmtnacts::stay);
		}

		break;
	case eggsubmtnacts::stay:
		if (SubEgg_CheckDamage(data, wk)) {
			SubEgg_ChangeSub(wk, eggsubmtnacts::sink);
			break;
		}

		speed = wk->Level == 1 ? 100 : 70;

		if (++wk->InternalTimer == speed) {
			// fire
		}
		else if (wk->InternalTimer > speed * 2) {
			SubEgg_ChangeSub(wk, eggsubmtnacts::sink);
		}

		break;
	case eggsubmtnacts::sink:
		if (SubEgg_Sink(data, 0.8f)) {
			SubEgg_ChangeSub(wk, eggsubmtnacts::hidden);

			// if untouched during this act, redo or change
			if (wk->InternalTimer > 200 && rand() % 10 < 2) {
				SubEgg_ChangeAction(wk, eggsubacts::act2);
			}
			else {
				if (wk->Level <= 2) {
					SubEgg_ChangeAction(wk, eggsubacts::act1);
				}
				else {
					SubEgg_ChangeAction(wk, rand() % 2 == 0 ? eggsubacts::act3 : eggsubacts::act4); // act 3 or 4 when 2 lives left
				}
			}
		}
	}
}

inline void SubEggAct3Attack(EntityData1* data, eggsubwk* wk) {
	++wk->InternalTimer;

	int speed = wk->InternalTimer * 10;

	data->Rotation.y += speed > 0x500 ? 0x500 : speed;

	if (wk->InternalTimer % 10 == 0) {
		// deflarg
	}

	if (wk->InternalTimer > 500) {
		SubEgg_ChangeSub(wk, eggsubmtnacts::sink);
	}
}

// Swirls around with lava coming out of gun
void SubEgg_Act3(EntityData1* data, eggsubwk* wk) {
	switch (wk->Subs) {
	case eggsubmtnacts::hidden:
		EmergePlatforms();

		SubEgg_GetRandomPosition(data, wk);
		
		SetLavaPoint(data->Position.x, data->Position.z);
		SubEgg_LookAtPlayer(data, wk);

		SubEgg_ChangeSub(wk, eggsubmtnacts::emerge);
		break;
	case eggsubmtnacts::emerge:
	default:
		if (SubEgg_Emerge(data, 0.5f)) {
			SubEgg_ChangeSub(wk, eggsubmtnacts::stay);
			
		}

		SubEggAct3Attack(data, wk);

		break;
	case eggsubmtnacts::stay:
		if (SubEgg_CheckDamage(data, wk)) {
			SubEgg_ChangeSub(wk, eggsubmtnacts::sink);
			break;
		}

		SubEggAct3Attack(data, wk);

		break;
	case eggsubmtnacts::sink:
		if (SubEgg_Sink(data, 0.5f)) {
			++wk->lastattackscount;

			SubEgg_ChangeSub(wk, eggsubmtnacts::hidden);
			SubEgg_ChangeAction(wk, eggsubacts::act4);

			if (wk->lastattackscount > 6) {
				SubEgg_ChangeAction(wk, eggsubacts::act1);
				EmergePlatforms();
			}
			
			return;
		}

		SubEggAct3Attack(data, wk);
	}
}

// Launches bombs around while under lava
void SubEgg_Act4(EntityData1* data, eggsubwk* wk) {
	switch (wk->Subs) {
	case eggsubmtnacts::hidden:
		SinkPlatforms();

		++wk->InternalTimer;

		if (wk->InternalTimer % 50 == 0) {
			SubEgg_GetRandomPosition(data, wk);
			// bomb
		}

		if (wk->InternalTimer > 800) {
			++wk->lastattackscount;

			SubEgg_ChangeAction(wk, eggsubacts::act3);
		}
		break;
	default:
		SubEgg_ChangeAction(wk, eggsubacts::act1); // failsafe
		break;
	}
}

void __cdecl SubEggman_Delete(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	eggsubwk* wk = (eggsubwk*)obj->UnknownB_ptr;

	CurrentBoss = nullptr;
}

void __cdecl SubEggman_Display(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	eggsubwk* wk = (eggsubwk*)obj->UnknownB_ptr;

	if (!MissedFrames && (wk->HitTimer == 0 || wk->HitTimer % 5) && wk->Subs != eggsubmtnacts::hidden) {
		njSetTexture(&HillTopOBJ_TexList);
		njPushMatrixEx();
		njTranslateEx(&data->Position);
		njRotateY_(data->Rotation.y);

		njPushMatrixEx(); {
			njTranslateX(-15.0f);
			njDrawModel_SADX(Sphere_Model.basicdxmodel);
			njPopMatrixEx();
		}

		njScalef(2.0f);
		njDrawModel_SADX(Sphere_Model.basicdxmodel);
		njPopMatrixEx();
	}
}

void __cdecl SubEggman_Main(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	eggsubwk* wk = (eggsubwk*)obj->UnknownB_ptr;

	EntityData1* player = EntityData1Ptrs[0];
	CharObj2* pco2 = CharObj2Ptrs[0];

	SubEgg_UpdateStatus(data, wk);

	switch (data->Action) {
	case eggsubmainact_init:
		if (player->Status & Status_Ground && pco2->field_A == 0) {
			SetLavaPoint(0.0f, 0.0f);
			EnableTimeThing();
			LoadLifeGauge(0, 0, wk->HitPoint);
			data->Action = 1;
		}

		break;
	case eggsubmainact_run:
		switch (wk->Acts) {
		case eggsubacts::act1:
			SubEgg_Act1(data, wk);
			break;
		case eggsubacts::act2:
			SubEgg_Act2(data, wk);
			break;
		case eggsubacts::act3:
			SubEgg_Act3(data, wk);
			break;
		case eggsubacts::act4:
			SubEgg_Act4(data, wk);
			break;
		}

		break;
	case eggsubmainact_death:
		data->Position.y -= 0.1f;

		if (wk->HitTimer == 0 && Egg1ExplosionTask) {
			CheckThingButThenDeleteObject(Egg1ExplosionTask);
			DeleteObject_(obj);
		}

		break;
	}

	RunObjectChildren(obj);
	AddToCollisionList(data);
	obj->DisplaySub(obj);
}

void __cdecl SubEggman(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	eggsubwk* wk = (eggsubwk*)BossAlloc(data, sizeof(eggsubwk));

	ObjectMaster* musicobj = LoadObject(LoadObj_Data1, 1, LoadMusic_EventDelayed);
	musicobj->Data1->Action = HillTopBossMusic;
	musicobj->Data1->InvulnerableTime = 100;

	obj->UnknownB_ptr = (void*)wk;
	wk->HitPoint = 5;
	wk->MaxHealth = 5;

	wk->Subs = eggsubmtnacts::hidden;
	wk->Acts = eggsubacts::act1;

	data->Position.y = sinkHeight;

	obj->MainSub = SubEggman_Main;
	obj->DeleteSub = SubEggman_Delete;
	obj->DisplaySub = SubEggman_Display;

	DisplayBossName2("Egg Sub", -1, 240, 80);

	Collision_Init(obj, &SubEgg_Col, 1, 2);

	//SetCameraEvent(EggmanBossCamera_Init, CameraAdjustsIDs::None, CameraDirectIDs::Sonic);

	CurrentBoss = obj;
}
#pragma endregion

void __cdecl Boss_SubEggman_Main(ObjectMaster* obj) {
	//SetCameraControlEnabled(0);
}

void __cdecl Boss_SubEggman_Init(ObjectMaster* obj) {
	LoadObject(LoadObj_Data1, 2, BossLava);
	LoadObject(LoadObj_Data1, 2, PlatformsHandler);
	LoadObject(LoadObj_Data1, 2, SubEggman);
	
	CharObj2* co2 = CharObj2Ptrs[0];

	DisableTimeThing();

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