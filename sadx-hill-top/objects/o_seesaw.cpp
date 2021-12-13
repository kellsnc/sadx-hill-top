#include "pch.h"
#include "o_seesaw.h"

/*

Unidus seesaw

ScaleX: Height power

*/

static ModelInfo* ht_seesaw = nullptr;

static CCL_INFO HillSeesaw_Col[] = {
	{ 0, CI_FORM_CAPSULE, 0x77, 0, 0, {0.0f, 1.3f, 0.0f}, 1.3f, 4.5f, 0.0f, 0.0f, 0x4000, 0, 0 },
	{ 0, CI_FORM_RECTANGLE2, 0x77, 0, 0, {0.0f, 2.5f, 0.0f}, 19.0f, 0.75f, 5.0f, 0.0f, 0, 0, 0 }
};

static bool IsOnSeesaw(taskwk* twp, NJS_VECTOR* pos)
{
	NJS_VECTOR newpos = { 0 };

	njPushMatrix(_nj_unit_matrix_);
	njTranslateEx(&twp->pos);
	njRotateY_(twp->ang.y);
	njRotateZ_(twp->ang.x);
	njTranslateX(-16.0f);
	njTranslateY(10.0f);
	njGetTranslation(0, &newpos);
	njPopMatrixEx();

	return CheckCollisionPointSphere(&newpos, pos, 12.0f);
}

static void Seesaw_Check(task* tp, taskwk* ptwp)
{
	auto twp = tp->twp;

	if (ptwp->mode == 2 && IsOnSeesaw(twp, &ptwp->pos))
	{
		twp->mode = 1;
		tp->ctp->twp->mode = 1;
	}
}

static void Seesaw_Launch(task* tp, taskwk* ptwp)
{
	auto twp = tp->twp;

	if (IsOnSeesaw(twp, &ptwp->pos))
	{
		playerpwp[TASKWK_PLAYERID(ptwp)]->spd.y = twp->scl.x;
		playermwp[TASKWK_PLAYERID(ptwp)]->spd.y = twp->scl.x;
		dsPlay_oneshot_Dolby(458, 0, 0, 100, 120, twp);
	}
}

static void __cdecl UnidusSeesawDisplay(task* tp)
{
	if (!MissedFrames)
	{
		auto twp = tp->twp;

		njSetTexture(&UNI_A_UNIBODY_TEXLIST);
		njPushMatrixEx();
		njTranslateEx(&twp->pos);
		njRotateY_(twp->ang.y);
		DrawObject((NJS_OBJECT*)0x96DBF0); // Unudius model
		njPopMatrixEx();
	}
}

static void __cdecl UnidusSeesawExec(task* tp)
{
	auto twp = tp->twp;
	float orig_height = tp->ptp->ocp->pObjEditEntry->ypos+ 4.0f;

	if (twp->mode == 1)
	{
		twp->wtimer += 500; // jump animation frame

		unsigned int angle = static_cast<unsigned int>(twp->wtimer);

		if (angle > 0x8000) // if animation is finished, reset state
		{
			twp->mode = 0;
			twp->wtimer = 0;
			twp->pos.y = orig_height;
		}
		else
		{
			twp->pos.y = orig_height + 50.0f * njSin(angle); // jump animation
		}
	}

	EntryColliList(twp);
	tp->disp(tp);
}

static void LoadUnidusSeesaw(task* tp, taskwk* twp)
{
	auto ctp = CreateChildTask(LoadObj_Data1, UnidusSeesawExec, tp);
	auto ctwp = ctp->twp;

	// Calculate initial position of Unidus
	njPushMatrix(_nj_unit_matrix_);
	njTranslateEx(&twp->pos);
	njRotateY(0, twp->ang.y);
	njTranslateX(16.0f);
	njTranslateY(4.0f);
	njGetTranslation(0, &ctwp->pos);
	njPopMatrixEx();

	CCL_Init(ctp, (CCL_INFO*)Unidus_Collision, Unidus_Collision_Length, 4);

	ctwp->wtimer = 0;
	ctp->disp = UnidusSeesawDisplay;
}

static void __cdecl HillSeesawDisplay(task* tp)
{
	if (!MissedFrames)
	{
		auto twp = tp->twp;
		auto object = (NJS_OBJECT*)twp->value.ptr;

		SetSecondObjectTexture();
		njPushMatrixEx();
		njTranslateEx(&twp->pos);
		njRotateY_(twp->ang.y);

		dsDrawModel(object->basicdxmodel);

		njTranslateY(3.0f);
		njRotateZ_(twp->ang.x);
		dsDrawModel(object->child->basicdxmodel);

		njPopMatrixEx();
	}
}

static void __cdecl HillSeesawExec(task* tp)
{
	if (!CheckRangeOut(tp))
	{
		auto twp = tp->twp;

		IsOnSeesaw(twp, &twp->pos);

		if (twp->mode == 0)
		{
			ForEveryCollidingPlayer(tp, Seesaw_Check);
		}
		else if (twp->mode == 1)
		{
			if (twp->ang.x < 0x880)
			{
				twp->ang.x += 0x200;
			}
			else
			{
				twp->ang.x = 0x880;
			}

			// If Unidus is ready
			if (tp->ctp->twp->mode == 0)
			{
				ForEveryCollidingPlayer(tp, Seesaw_Launch);
				twp->mode = 0;
				twp->ang.x = -0x880;
			}
		}

		// Move Collision
		auto cwp = twp->cwp;

		if (cwp)
		{
			cwp->info[1].angz = twp->ang.x;
		}

		LoopTaskC(tp); // Run Unidus task
		EntryColliList(twp);
		tp->disp(tp);
	}
}

void __cdecl HillSeesaw(task* tp)
{
	auto twp = tp->twp;
	
	twp->ang.x = -0x880;
	twp->value.ptr = ht_seesaw->getmodel();

	CCL_Init(tp, arrayptrandlength(HillSeesaw_Col), 4);

	tp->exec = HillSeesawExec;
	tp->disp = HillSeesawDisplay;

	LoadUnidusSeesaw(tp, twp);
}

void HillSeesaw_LoadAssets()
{
	LoadModelFile(&ht_seesaw, "ht_seesaw", ModelFormat_Basic);
}

void HillSeesaw_FreeAssets()
{
	FreeFileInfo(&ht_seesaw);
}