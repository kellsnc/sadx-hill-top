#include "pch.h"
#include "o_growlava.h"
#include "lava.h"

/*

Objects related to the growing lava event in act 2

*/

extern NJS_TEXLIST HillTop_TexList;

extern ModelInfo* ht_platform;
static ModelInfo* ht_growlava = nullptr;

static CCL_INFO GrowLavaTrigger_Col = { 0, CI_FORM_SPHERE, 0xF0, 0, 0, { 0.0f, 0.0f, 0.0f }, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0 };

Grow_WK grow_workers[3] = { };

static void UpdateDynCol(task* tp, NJS_OBJECT* dyncol, NJS_POINT3* pos, int id)
{
	NJS_VECTOR newpos = { pos->x, pos->y + grow_workers[id].Offset, pos->z };
	MoveGeoCollision(tp, dyncol, &newpos);
}

#pragma region GrowLava

/*

Lava that goes up when the trigger is trigged
ScaleX: model ID
ScaleY: lava (0) or ground (1)

*/

void __cdecl GrowLavaDisplay(task* tp)
{
	if (!MissedFrames)
	{
		auto twp = tp->twp;
		auto object = (NJS_OBJECT*)twp->counter.ptr;
		auto trigger_id = twp->ang.z;
		
		if (twp->scl.y == 1.0f)
		{
			SetLevelTexture();
		}
		else
		{
			njSetTexture(&CurrentLavaTex);
		}

		njPushMatrixEx();
		njTranslateEx(&twp->pos);

		// Shake if enabled
		if (grow_workers[trigger_id].Enabled == true)
		{
			njTranslateY(grow_workers[trigger_id].Offset);
		}

		dsDrawModel(object->basicdxmodel);
		njPopMatrixEx();
	}
}

void __cdecl GrowLavaExec(task* tp)
{
	auto twp = tp->twp;
	auto trigger_id = twp->ang.z;

	if (grow_workers[trigger_id].Enabled == true)
	{
		twp->pos.y = grow_workers[trigger_id].Height;

		if (twp->pos.y < twp->scl.z)
		{
			twp->pos.y = twp->scl.z;
		}

		UpdateDynCol(tp, (NJS_OBJECT*)twp->counter.ptr, &twp->pos, trigger_id);
	}

	tp->disp(tp);
}

void __cdecl GrowLava(task* tp)
{
	auto twp = tp->twp;

	twp->scl.z = twp->pos.y; // Backup original position

	NJS_OBJECT* object = GetMobileLandObject();

	object->pos[0] = twp->pos.x;
	object->pos[1] = twp->pos.y;
	object->pos[2] = twp->pos.z;

	object->ang[0] = 0;
	object->ang[1] = 0;
	object->ang[2] = 0;

	object->scl[0] = 1.0f;
	object->scl[1] = 1.0f;
	object->scl[2] = 1.0f;

	object->basicdxmodel = GetModelSibling(ht_growlava->getmodel()->child, static_cast<int>(twp->scl.x))->basicdxmodel;

	if (twp->scl.y == 1.0f)
	{
		RegisterCollisionEntry(ColFlags_Dynamic | ColFlags_Solid, tp, object); // ground
	}
	else
	{
		RegisterCollisionEntry(0x4000 | ColFlags_Dynamic | ColFlags_Solid | ColFlags_Hurt, tp, object); // lava
	}

	twp->counter.ptr = object;  // store geometry collision object

	tp->exec = GrowLavaExec;
	tp->disp = GrowLavaDisplay;
	tp->dest = B_Destructor; // destructor function that removes geo collision in twp->count
}
#pragma endregion

#pragma region GrowLavaPlatform

/*

Platform that moves with lava height

RotX: lava id
RotZ: speed
ScaleX: scale
ScaleY: movement radius

*/

void __cdecl GrowLavaPlatformDisplay(task* tp)
{
	if (!MissedFrames)
	{
		auto twp = tp->twp;
		auto object = (NJS_OBJECT*)twp->counter.ptr;
		auto trigger_id = twp->ang.x;

		SetLevelTexture();
		njPushMatrixEx();
		njTranslateEx(&twp->pos);

		// Shake if enabled
		if (grow_workers[trigger_id].Enabled == true)
		{
			njTranslateY(grow_workers[trigger_id].Offset);
		}

		njRotateY_(twp->ang.y);
		njScalef(twp->scl.x);
		dsDrawModel(object->basicdxmodel);
		njPopMatrixEx();
	}
}

void __cdecl GrowLavaPlatformExec(task* tp)
{
	if (!CheckRangeOut(tp))
	{
		auto twp = tp->twp;
		auto trigger_id = twp->ang.x;

		if (grow_workers[trigger_id].Enabled == true && grow_workers[trigger_id].Height > twp->scl.z - twp->scl.y - 10.0f)
		{
			twp->scl.z = grow_workers[trigger_id].Height + twp->scl.y + 10.0f;
		}

		twp->pos.y = twp->scl.z + (1.0f - powf(njSin(GameTimer * twp->ang.z), 2.0f) * twp->scl.y);

		UpdateDynCol(tp, (NJS_OBJECT*)twp->counter.ptr, &twp->pos, trigger_id);
		tp->disp(tp);
	}
}

void __cdecl GrowLavaPlatform(task* tp)
{
	auto twp = tp->twp;

	if (twp->scl.x == 0.0f)
	{
		twp->scl.x = 1.0f; // default scale
	}

	if (twp->ang.z == 0)
	{
		twp->ang.z = 150; // default speed
	}

	twp->scl.z = twp->pos.y; // backup original y position

	// Create a geometry collision:
	auto object = GetMobileLandObject();

	object->pos[0] = twp->pos.x;
	object->pos[1] = twp->pos.y;
	object->pos[2] = twp->pos.z;

	object->ang[0] = 0;
	object->ang[1] = twp->ang.y;
	object->ang[2] = 0;

	object->scl[0] = twp->scl.x;
	object->scl[1] = twp->scl.x;
	object->scl[2] = twp->scl.x;

	object->basicdxmodel = ht_platform->getmodel()->basicdxmodel;

	RegisterCollisionEntry(ColFlags_Dynamic | ColFlags_Solid | ColFlags_UseRotation, tp, object);

	twp->counter.ptr = object; // store geometry collision object
	
	tp->exec = GrowLavaPlatformExec;
	tp->disp = GrowLavaPlatformDisplay;
	tp->dest = B_Destructor; // destructor function that removes geo collision in twp->count
}
#pragma endregion

#pragma region GrowLavaTrigger
/*

Trigger that launches the lava & plaforms

AngX: grow speed (divided by 100)
AngY: if 1, reset lava id on restart
AngZ: index
SclX: radius
SclY: start height
SclZ: end height

*/

void __cdecl GrowLavaTriggerDestroy(task* tp)
{
	grow_workers[tp->twp->ang.z] = { false, 0.0f, 0.0f };
}

void __cdecl GrowLavaTriggerExec(task* tp)
{
	auto twp = tp->twp;

	int trigger_id = twp->ang.z;

	// If twp->ang.y is 1, reset lava if restarting
	if (twp->ang.y == 1 && GameState != 4)
	{
		twp->mode = 2;
		DeadOut(tp); // Remove object and tell it not to spawn until next restart
	}

	if (twp->mode == 0)
	{
		if (CheckCollisionP_num(&twp->pos, twp->scl.x, 0))
		{
			if (twp->ang.y == 1)
			{
				DeadOut(tp);
			}
			else
			{
				grow_workers[trigger_id].Enabled = true;
				grow_workers[trigger_id].Height = twp->scl.y;
				dsPlay_oneshot(462, 0, 0, 0);
				twp->wtimer = 0;
				twp->mode = 1;
			}
		}

		EntryColliList(twp);
	}
	else if (twp->mode == 1)
	{
		if (grow_workers[trigger_id].Height >= twp->scl.z)
		{
			grow_workers[trigger_id].Enabled = false;
			twp->mode = 2;
		}

		++twp->wtimer;

		if (!(playerpwp[0]->item & Powerups_Dead))
		{
			grow_workers[trigger_id].Height += static_cast<float>(twp->ang.x) / 100.0f;
		}
		
		// Shake everything by applying changing offset:
		if (twp->wtimer % 100 == 0)
		{
			dsPlay_oneshot(462, 0, 0, 0);
		}

		if (twp->wtimer < 90)
		{
			float sin = (static_cast<float>(twp->wtimer) / 90.0f) * 1.5f;

			if (sin <= 1.0f)
			{
				sin = njSin(((sin * 90.0f + 90.0f) * 182.0444488525391f));
			}
			else
			{
				sin = 0.0f;
			}

			grow_workers[trigger_id].Offset = njSin((static_cast<float>(twp->wtimer) * 40.0f * 182.0f)) * (sin * 4.0f);
		}
		else
		{
			grow_workers[trigger_id].Offset = njSin(((static_cast<float>(twp->wtimer) - 90.0f) * 48.0f * 182.0f)) * 0.18f;
		}
	}
}

void __cdecl GrowLavaTrigger(task* tp)
{
	auto twp = tp->twp;

	CCL_Init(tp, &GrowLavaTrigger_Col, 1, 4);

	twp->cwp->info->a = twp->scl.x;

	tp->exec = GrowLavaTriggerExec;
	tp->dest = GrowLavaTriggerDestroy;
}
#pragma endregion

#pragma region KillCeiling

/*

Object that kills the player if standing while colliding with this.
Scale: box scale
RotX: lava id

*/

void __cdecl KillCeiling(task* tp)
{
	if (!CheckRangeOut(tp))
	{
		auto twp = tp->twp;

		if (twp->mode != 0)
		{
			taskwk* entity = (taskwk*)GetCollidingEntityA((EntityData1*)twp);

			// if the player collides while being on the ground
			if (grow_workers[twp->ang.x].Enabled == true && entity && (entity->flag & Status_Ground) && (playerpwp[TASKWK_PLAYERID(entity)]->attr & ColFlags_Dynamic))
			{
				KillPlayer(TASKWK_PLAYERID(entity));
			}

			EntryColliList(twp);
		}
		else
		{
			CCL_Init(tp, (CCL_INFO*)&C_Cube_Collision, 1, 4u);

			auto colinfo = twp->cwp->info;

			colinfo->push = 0xF0;
			colinfo->a = twp->scl.x + 10.0f;
			colinfo->b = twp->scl.y + 10.0f;
			colinfo->c = twp->scl.z + 10.0f;

			CCL_CalcRange(twp);

			twp->mode = 1;
		}
	}
}
#pragma endregion

void GrowLava_LoadAssets()
{
	LoadModelFile(&ht_growlava, "ht_grow", ModelFormat_Basic);
}

void GrowLava_FreeAssets()
{
	FreeFileInfo(&ht_growlava);
}