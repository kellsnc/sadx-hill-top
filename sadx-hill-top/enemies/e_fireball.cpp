#include "pch.h"
#include "e_fireball.h"
#include "..\objects\o_lavafall.h" // For LAVAFALL_TexList

ModelInfo* e_fireball = nullptr;

CCL_INFO FireBallCol = { 0, CI_FORM_SPHERE, 0x77, 0x2F, 0, { 0.0f, 1.0f, 0.0f }, 1.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0 };

static NJS_TEXLIST CurrentLavaTex = { nullptr, 1 };

static void __cdecl FireBallDisplay(task* tp)
{
	if (!MissedFrames)
	{
		auto twp = tp->twp;
		auto object = (NJS_OBJECT*)twp->value.ptr;

		CurrentLavaTex.textures = &LAVAFALL_TexList.textures[twp->btimer];
		njSetTexture(&CurrentLavaTex);

		njPushMatrixEx();
		njTranslateEx(&twp->pos);
		njRotateY_(twp->ang.y);
		njRotateZ_(twp->ang.x);
		njScalef(twp->scl.x);
		dsDrawModel(object->basicdxmodel);
		njPopMatrixEx();

		Shadow(twp, twp->scl.x * 0.25f);
	}
}

static void __cdecl FireBallExec(task* tp)
{
	if (!CheckRangeOut(tp))
	{
		auto twp = tp->twp;

		if (twp->btimer >= LAVAFALL_TexList.nbTexture - 1)
		{
			twp->btimer = 0;
		}
		else
		{
			++twp->btimer;
		}

		// Move
		njPushMatrix(_nj_unit_matrix_);
		njTranslateEx(&twp->pos);
		njTranslateY(-(twp->scl.z / 4)); // grav
		njRotateY_(twp->ang.y - 0x4000);
		njRotateZ_(twp->ang.x);

		// Check direction
		if (twp->wtimer == 0)
		{
			njTranslateX(twp->scl.y); // speed
		}
		else
		{
			njTranslateY(twp->scl.y);
		}

		njGetTranslation(nullptr, &twp->pos);
		njPopMatrixEx();

		// Update movement speed
		twp->scl.z *= 1.02;
		twp->scl.y *= 0.99;

		// Destroy if touched
		if (CCL_IsHitPlayer(twp) || CCL_IsHitBullet(twp))
		{
			FreeTask(tp);
			return;
		}

		EntryColliList(twp);
		tp->disp(tp);
	}
}

void __cdecl FireBall(task* tp)
{
	auto twp = tp->twp;

	if (twp->scl.x == 0.0f)
	{
		twp->scl.x = 1.0f;
	}

	twp->value.ptr = e_fireball->getmodel(); // store model

	CCL_Init(tp, &FireBallCol, 1, 4);

	auto cwp = twp->cwp;

	if (cwp)
	{
		cwp->info->a *= twp->scl.x; // scale collision size
	}
	
	tp->exec = FireBallExec;
	tp->disp = FireBallDisplay;
}

void LoadFireBall(task* tp, NJS_POINT3* pos, Angle angy, Angle angx, Float spd, Float scl, Float grav, int dir)
{
	auto ctp = CreateChildTask(IM_TWK, FireBall, tp);
	auto ctwp = ctp->twp;

	ctwp->pos = *pos;
	ctwp->ang.y = angy;
	ctwp->ang.x = angx;
	ctwp->scl.x = scl;
	ctwp->scl.y = spd;
	ctwp->scl.z = grav;
	ctwp->wtimer = dir;
}

/*

FIREBALL LAUNCHER ENEMY

FloatX = scale (default 1)
FloatY = launch speed (default 1)
FloatZ = spawn radius (if 0 spawn at position)
RotZ = Launch rate in frames (default 180)

*/

static void __cdecl FireBallLauncherExec(task* tp)
{
	if (!CheckRangeOut(tp))
	{
		auto twp = tp->twp;

		if (twp->wtimer <= 0)
		{
			auto pos = twp->pos;

			if (twp->scl.z != 0)
			{
				// Get random position in spawn radius
				pos.x += (twp->scl.z / 2.0f) - static_cast<float>(rand() % static_cast<int>(twp->scl.z));
				pos.z += (twp->scl.z / 2.0f) - static_cast<float>(rand() % static_cast<int>(twp->scl.z));
			}

			NJS_VECTOR velo = { 0, 1.0f, 0 };
			CreateSmoke(&twp->pos, &velo, 1.0f);
			LoadFireBall(tp, &pos, twp->ang.y, twp->ang.x, twp->scl.y, twp->scl.x, twp->scl.y / 2, 1);
			dsPlay_oneshot_Dolby(464, 0, 0, 60, 120, twp);

			twp->wtimer = twp->ang.z; // wait for provided number of frames
		}
		else
		{
			--twp->wtimer;
		}

		LoopTaskC(tp);
	}
}

void __cdecl FireBallLauncher(task* tp)
{
	auto twp = tp->twp;

	tp->exec = FireBallLauncherExec;

	// Default values:

	if (twp->scl.x == 0.0f)
	{
		twp->scl.x = 1.0f;
	}

	if (twp->scl.y == 0.0f)
	{
		twp->scl.y = 1.0f;
	}

	if (twp->ang.z == 0)
	{
		twp->ang.z = 180;
	}
}

void FireBall_LoadAssets()
{
	LoadModelFile(&e_fireball, "e_fireball", ModelFormat_Basic);
}

void FireBall_FreeAssets()
{
	FreeFileInfo(&e_fireball);
}