#include "pch.h"
#include "e_subeggman.h"
#include "lava.h"

static int HillTopBossMusic;

ModelInfo* ht_bosslava = nullptr;

ObjectMaster* CurrentBossLava = nullptr;
ObjectMaster* CurrentBoss = nullptr;

#pragma region Lava

/*

Lava plane with vertex animation

*/

enum class LavaModes : int {
	Grow,
	Normal,
	Fast,
	Point
};

struct lavawk {
	LavaModes Mode;
	short Status;
	short Timer;
	int nbPoint;
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

void SetLavaMode(LavaModes mode) {
	if (CurrentBossLava) {
		lavawk* wk = (lavawk*)CurrentBossLava->Data1;
		wk->PrevMode = wk->Mode;
		wk->Mode = mode;
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
		wk->Offsets[result] = 7.0f;
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
		wk->Timer += 1;

		if (wk->Object->pos[1] < 0.0f) {
			wk->Object->pos[1] += 0.5f;
		}
		else {
			wk->Mode = LavaModes::Normal;
		}

		break;
	case LavaModes::Normal:
		wk->Timer += 2;
		break;
	case LavaModes::Fast:
		wk->Timer += 5;
		break;
	case LavaModes::Point:
		wk->Timer += 2;
		FindLavaPoint(wk);
		wk->Mode = wk->PrevMode;

		break;
	}

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

#pragma region Cameras
void __cdecl EggmanBossCamera_Init(_OBJ_CAMERAPARAM*) {

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
};

CollisionData SubEgg_Col = { 0, CollisionShape_Sphere, 0x77, 0x20, 0x400, {0.0f, 0.0f, 0.0f}, 22.0f, 0.0f, 0.0f };

static constexpr float surfaceHeight = 0.0f;
static constexpr float sinkHeight = -50.0f;

bool SubEgg_CheckDamage(EntityData1* data, eggsubwk* wk) {
	if (wk->HitPoint > 0 && wk->HitTimer == 0) {
		EntityData1* entity = GetCollidingEntityA(data);

		bool playerHurting = entity && entity->Status & (Status_Attack | Status_Ball);

		if (playerHurting) {
			NJS_VECTOR spd = { -2.0f, 2.0f, 0.0f };
			PlayerDirectionToVector(entity, &spd);
			EnemyBounceThing(entity->CharIndex, spd.x, spd.y, spd.z);
		}

		if (playerHurting || GetCollidingEntityB(data)) {
			wk->HitTimer = 50;
			wk->HitPoint -= 1;
			PlaySound(463, 0, 0, 0);

			if (wk->HitPoint == 0) {
				NonStaticFunctionPointer(void, sub_574460, (NJS_VECTOR * pos), 0x574460);
				sub_574460(&data->Position); // explosion effect
				wk->HitTimer = 200;
				data->Action = eggsubmainact_death;

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

	data->Rotation.y = -LOWORD(wk->bwk.yacc)
		- (atan2(wk->bwk.zspd - entity->Position.z, wk->bwk.xspd - entity->Position.x)
			* 65536.0
			* 0.1591549762031479);
}

void SubEgg_Act1(EntityData1* data, eggsubwk* wk) {
	switch (wk->Subs) {
	case eggsubmtnacts::hidden:
		wk->Subs = eggsubmtnacts::emerge;
		break;
	case eggsubmtnacts::emerge:
	default:
		if (SubEgg_Emerge(data, 0.2f)) {
			wk->Subs = eggsubmtnacts::stay;
			wk->InternalTimer = 0;
		}

		break;
	case eggsubmtnacts::stay:
		if (SubEgg_CheckDamage(data, wk)) {
			wk->Subs = eggsubmtnacts::sink;
			break;
		}

		SubEgg_LookAtPlayer(data, wk);

		if (++wk->InternalTimer % 200 == 0) {
			// shoot
		}

		break;
	case eggsubmtnacts::sink:
		if (SubEgg_Sink(data, 0.1f)) {
			wk->Subs = eggsubmtnacts::hidden;
			wk->Acts = eggsubacts::act2;
		}
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
		}

		break;
	case eggsubmainact_death:
		data->Position.y -= 0.05f;

		if (wk->HitTimer == 0 && Egg1ExplosionTask) {
			CheckThingButThenDeleteObject(Egg1ExplosionTask);
			DeleteObject_(obj);
		}

		break;
	}

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
	wk->Subs = eggsubmtnacts::hidden;

	data->Position.y = sinkHeight;

	obj->MainSub = SubEggman_Main;
	obj->DeleteSub = SubEggman_Delete;
	obj->DisplaySub = SubEggman_Display;

	DisplayBossName2("Submarine Eggman", -1, 240, 80);

	Collision_Init(obj, &SubEgg_Col, 1, 2);

	//SetCameraEvent(EggmanBossCamera_Init, CameraAdjustsIDs::None, CameraDirectIDs::Sonic);

	CurrentBoss = obj;
}
#pragma endregion

void __cdecl Boss_SubEggman_Main(ObjectMaster* obj) {
	SetCameraControlEnabled(0);
}

void __cdecl Boss_SubEggman_Init(ObjectMaster* obj) {
	LoadObject(LoadObj_Data1, 2, BossLava);
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