#include "pch.h"
#include "e_spiker.h"

/*

SPIKER ENEMY

Rot Z = Emerald ID (for Knuckles) -> Index
Scale X = Walk/Attack radius
Scale Y = Type (0 is homing missile; 1 is vertical attack);

*/

NJS_TEXNAME SPIKER_TexNames[5];
NJS_TEXLIST SPIKER_TexList = { arrayptrandlength(SPIKER_TexNames) };

ModelInfo* e_spiker = nullptr;

AnimationFile* e_spiker_stand = nullptr;
AnimationFile* e_spiker_walk = nullptr;

CCL_INFO Spiker_Col = { 0, CI_FORM_SPHERE, 0x10, 0x21, 0x400, { 0.0f, 8.0f, 0.0f }, 4.5f, 0.0f, 0.0f, 0.0f, 0, 0, 0 };
CCL_INFO Spike_Col =  { 0, CI_FORM_SPHERE, 0x70, 0x2F, 0, { 0.0f, 15.0f, 0.0f }, 4.5f, 6.0f, 0.0f, 0.0f, 0, 0, 0 };

NJS_ACTION SpikerActions[2] = { 0 };

enum SpikerAnim {
	SpikerAnim_Stand,
	SpikerAnim_Walk
};

enum SpikerAct {
	SpikerAct_Walk,
	SpikerAct_Stand,
	SpikerAct_Flee,
	SpikerAct_Dead
};

enum SpikerAttack {
	SpikerAttack_None,
	SpikerAttack_Homing,
	SpikerAttack_Vertical
};

#pragma region Spike

void SpikeUpdatePos(task* tp, float offset)
{
	auto ptwp = tp->ptp->twp;

	njPushMatrix(_nj_unit_matrix_);
	njTranslateEx(&ptwp->pos);
	njRotateZXY(&ptwp->ang);
	njTranslateY(9.0f);
	njGetTranslation(0, &tp->twp->pos);
	njPopMatrixEx();
}

void __cdecl SpikeDisplay(task* tp)
{
	if (!MissedFrames)
	{
		auto twp = tp->twp;
		auto object = (NJS_OBJECT*)twp->value.ptr;

		njSetTexture(&SPIKER_TexList);
		njPushMatrixEx();
		njTranslateEx(&twp->pos);

		if (twp->mode == SpikerAttack_Homing)
		{
			njRotateY_(twp->ang.y);
			njRotateX_(twp->ang.x);
		}
		else
		{
			njRotateZXY(&twp->ang);
		}

		dsDrawModel(object->basicdxmodel);
		njPopMatrixEx();
	}
}

void SpikeHoming(taskwk* twp)
{
	Angle rotx, roty;

	if (twp->wtimer < 100)
	{
		njLookAt(&twp->pos, &playertwp[GetTheNearestPlayerNumber(&twp->pos)]->pos, &rotx, &roty);
		rotx += 0x4000;

		twp->ang.y = BAMS_SubWrap(twp->ang.y, roty, 0x300);
		twp->ang.x = BAMS_SubWrap(twp->ang.x, rotx, 0x300);
	}

	njPushMatrix(_nj_unit_matrix_);
	njTranslateEx(&twp->pos);
	njRotateY_(twp->ang.y);
	njRotateX_(twp->ang.x);
	njTranslateY(twp->scl.x);
	njGetTranslation(0, &twp->pos);
	njPopMatrixEx();
}

void SpikeVertical(taskwk* twp)
{
	njPushMatrix(_nj_unit_matrix_);
	njTranslateEx(&twp->pos);
	njRotateZXY(&twp->ang);
	njTranslateY(twp->scl.x);
	njGetTranslation(0, &twp->pos);
	njPopMatrixEx();
}

void __cdecl SpikeExec(task* tp)
{
	auto twp = tp->twp;
	auto ptwp = tp->ptp->twp;

	switch (twp->mode)
	{
	case SpikerAttack_None:
		if (ptwp->mode != SpikerAct_Stand)
		{
			SpikeUpdatePos(tp, 9.0f);
		}

		if (ptwp->flag & Status_Attack)
		{
			twp->mode = ptwp->smode;
			twp->cwp->info->center.y = 0.0f;
			twp->cwp->info->a = 10.0f;
		}

		break;
	case SpikerAttack_Homing:
		SpikeHoming(twp);
		break;
	case SpikerAttack_Vertical:
		SpikeVertical(twp);
		break;
	}

	if (twp->mode != SpikerAttack_None)
	{
		++twp->wtimer;

		if (twp->scl.x < 1.0f)
		{
			twp->scl.x += 0.05f;
		}

		if (twp->wtimer > 150 || CCL_IsHitPlayer(twp))
		{
			CreateFlash2(twp->pos.x, twp->pos.y, twp->pos.z, 1.0f);
			FreeTask(tp);
		}
	}
	
	EntryColliList(twp);
	tp->disp(tp);
}

void LoadSpikerSpike(task* tp)
{
	auto ctp = CreateChildTask(LoadObj_Data1, SpikeExec, tp);
	auto ctwp = ctp->twp;

	ctwp->value.ptr = e_spiker->getmodel()->child;
	ctwp->scl.x = 0.0f;

	CCL_Init(ctp, &Spike_Col, 1, 4);
	SpikeUpdatePos(ctp, 9.0f);

	ctp->disp = SpikeDisplay;
}

#pragma endregion

#pragma region Spiker

void Spiker_SetAnim(enemywk* enmwk, SpikerAnim anim)
{
	enmwk->actp = &SpikerActions[anim];
	enmwk->nframe = static_cast<float>((rand() % enmwk->actp->motion->nbFrame * 2)) / 2;
}

void __cdecl SpikerDisplay(task* tp)
{
	if (!MissedFrames)
	{
		auto twp = tp->twp;
		auto ewp = (enemywk*)tp->mwp;

		njSetTexture(&SPIKER_TexList);
		njPushMatrixEx();
		njTranslateEx(&twp->pos);
		njRotateZXY(&twp->ang);
		njAction(ewp->actp, ewp->nframe);
		njPopMatrixEx();

		Shadow(twp, ewp->shadow_scl);
	}
}

void SpikerDecideAim(task* tp, taskwk* twp, enemywk* ewp)
{
	if (EnemyDist2FromHome(twp, ewp) > ewp->view_range)
	{
		ewp->aim = ewp->home;
		return;
	}

	auto pnum = EnemySearchPlayer(twp, ewp);

	if (pnum)
	{
		ewp->aim = playertwp[pnum - 1]->pos;

		// Check attack
		if (!(twp->flag & Status_Attack) && EnemyDist2FromPlayer(twp, pnum) < ewp->view_range / 2)
		{
			SetBroken(tp);
			twp->flag |= Status_Attack;
			twp->mode = SpikerAct_Flee;
		}
	}
	else
	{
		ewp->aim = ewp->home;
	}
}

void SpikerMove(taskwk* twp, enemywk* ewp)
{
	EnemyPreservePreviousPosition(twp, ewp);
	ewp->velo.x = njCos(twp->ang.y) * 0.4f;
	ewp->velo.y = ewp->velo.y - ewp->weight;
	ewp->velo.z = njSin(-twp->ang.y) * 0.4f;
	twp->pos.x = twp->pos.x + ewp->velo.x;
	twp->pos.y = ewp->velo.y + twp->pos.y;
	twp->pos.z = ewp->velo.z + twp->pos.z;
	ewp->nframe += ewp->force.z;
}

void SpikerWalk(task* tp, taskwk* twp, enemywk* ewp)
{
	ewp->force.z = 0.5f;
	SpikerDecideAim(tp, twp, ewp);
	EnemyTurnToAim(twp, ewp);
	SpikerMove(twp, ewp);
	EnemyCheckGroundCollision(twp, ewp);
}

void SpikerStand(task* tp, taskwk* twp, enemywk* ewp)
{
	EnemyCheckGroundCollision(twp, ewp);

	if (!(twp->flag & Status_Attack) && CheckCollisionCylinderP(&twp->pos, twp->scl.x, 200.0f))
	{
		SetBroken(tp);
		twp->flag |= Status_Attack;
	}
}

void SpikerFlee(taskwk* twp, enemywk* ewp)
{
	NJS_VECTOR aim = playertwp[GetTheNearestPlayerNumber(&twp->pos)]->pos;
	njSubVector(&aim, &twp->pos);
	aim.x = -aim.x;
	aim.y = -aim.y;
	aim.z = -aim.z;
	njAddVector(&aim, &twp->pos);
	ewp->aim = aim;
	ewp->force.z = 0.25f;
	EnemyTurnToAim(twp, ewp);
	SpikerMove(twp, ewp);
	EnemyCheckGroundCollision(twp, ewp);
}

void SpikerCollision(taskwk* twp, enemywk* ewp)
{
	if (EnemyCheckDamage(twp, ewp) == TRUE)
	{
		twp->mode = SpikerAct_Dead;
		twp->flag |= Status_Hurt;
	}
}

void __cdecl SpikerExec(task* tp)
{
	if (!CheckRangeOut(tp))
	{
		auto twp = tp->twp;
		auto ewp = (enemywk*)tp->mwp;

		SpikerCollision(twp, ewp);

		switch (twp->mode)
		{
		case SpikerAct_Walk:
			SpikerWalk(tp, twp, ewp);
			break;
		case SpikerAct_Stand:
			SpikerStand(tp, twp, ewp);
			break;
		case SpikerAct_Flee:
			SpikerFlee(twp, ewp);
			break;
		case SpikerAct_Dead:
			Knuckles_KakeraGame_Set_PutEme(twp->btimer, &twp->pos);
			CreateFlash2(twp->pos.x, twp->pos.y, twp->pos.z, 1.0f);
			CreateExpSpring(twp, 6);
			CreateAnimal(20, twp->pos.x, twp->pos.y, twp->pos.z);
			DeadOut(tp);
			break;
		}

		LoopTaskC(tp);
		EntryColliList(twp);
		tp->disp(tp);
	}
}

void __cdecl Spiker(task* tp)
{
	auto twp = tp->twp;
	auto ewp = EnemyInitialize(tp, twp);

	CCL_Init(tp, &Spiker_Col, 1, 3);

	// some enemy information:
	ewp->height = 20.0f;
	ewp->view_angle = 0x2300;
	ewp->view_range = twp->scl.x * twp->scl.x;
	ewp->hear_range = ewp->view_range / 4;
	ewp->aim_angle = twp->ang.y;
	ewp->colli_radius = 5.0f;
	ewp->colli_top = 5.0f;
	ewp->colli_bottom = -5.0f;
	ewp->weight = 0.1f;
	ewp->shadow_scl = 1.3f;
	ewp->shadow_scl_ratio = 1.0f;
	ewp->ang_spd.y = 0x500;

	EnemyPreserveHomePosition(twp, ewp);
	
	tp->dest = UniDestructor;
	tp->exec = SpikerExec;
	tp->disp = SpikerDisplay;

	twp->btimer = twp->ang.z; // emerald id
	twp->mode = static_cast<char>(twp->scl.y) % 2; // walk or stand

	// If enemy hasn't already lost its spike
	if (CheckBroken(tp) == FALSE)
	{
		LoadSpikerSpike(tp);
	}

	// Set up animation:
	if (twp->mode == SpikerAct_Walk)
	{
		Spiker_SetAnim(ewp, SpikerAnim_Walk);
		twp->smode = SpikerAttack_Homing;
	}
	else
	{
		Spiker_SetAnim(ewp, SpikerAnim_Stand);
		twp->smode = SpikerAttack_Vertical;
	}
}

#pragma endregion

void Spiker_LoadAssets()
{
	LoadModelFile(&e_spiker, "e_spiker", ModelFormat_Basic);
	LoadAnimationFile(&e_spiker_stand, "e_spiker_stand");
	LoadAnimationFile(&e_spiker_walk, "e_spiker_walk");

	NJS_OBJECT* object = e_spiker->getmodel();

	object->child->evalflags |= NJD_EVAL_HIDE;

	SpikerActions[0].object = object;
	SpikerActions[0].motion = e_spiker_stand->getmotion();

	SpikerActions[1].object = object;
	SpikerActions[1].motion = e_spiker_walk->getmotion();
}

void Spiker_FreeAssets()
{
	FreeFileInfo(&e_spiker);
	FreeFileInfo(&e_spiker_stand);
	FreeFileInfo(&e_spiker_walk);
}