#include "pch.h"
#include "e_rexon.h"
#include "e_fireball.h"
#include "..\objects\o_growlava.h"

/*

REXON ENEMY

RotX = lava ID
RotZ = Emerald ID
ScaleX = Size;
ScaleY = Move Radius (stand if 0);

*/

NJS_TEXNAME REXON_TexNames[12];
NJS_TEXLIST REXON_TexList = { arrayptrandlength(REXON_TexNames) };

ModelInfo* e_rexon = nullptr;
ModelInfo* e_rexoncol = nullptr;

CCL_INFO Rexon_Col[] {
	{ 0, CI_FORM_SPHERE, 0x77, 0x21, 0x400, { 7.5f, 2.5f, 0.0f }, 2.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0 },
	{ 0, CI_FORM_SPHERE, 0x77, 0x21, 0x400, { 0.0f, 0.0f, 0.0f }, 2.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0 },
	{ 0, CI_FORM_SPHERE, 0x77, 0x21, 0x400, { 0.0f, 0.0f, 0.0f }, 2.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0 },
	{ 0, CI_FORM_SPHERE, 0x77, 0x21, 0x400, { 0.0f, 0.0f, 0.0f }, 2.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0 }
};

CCL_INFO RexonHead_Col = { 0, CI_FORM_SPHERE, 0x10, 0x21, 0x400, { 1.0f, 3.0f, 0.0f }, 2.5f, 0.0f, 0.0f, 0.0f, 0, 0, 0 };

enum RexonHeadActs
{
	RexonHeadAct_Normal,
	RexonHeadAct_Attacking
};

enum RexonActs
{
	RexonAct_Stand,
	RexonAct_Move,
	RexonAct_Dead
};

#pragma region RexonHead

void RexonHead_LoadFireBall(task* tp, taskwk* twp)
{
	NJS_VECTOR pos = { 0.0f, 3.0f, 1.0f };
	Angle roty, rotx;

	// Get fireball direction and spawn
	njPushMatrix(_nj_unit_matrix_);
	njTranslateEx(&twp->pos);
	njRotateY_(tp->ptp->twp->ang.y);
	njCalcPoint(0, &pos, &pos);
	njPopMatrixEx();
	njLookAt(&pos, &playertwp[GetTheNearestPlayerNumber(&twp->pos)]->pos, &rotx, &roty);
	LoadFireBall((task*)tp, &pos, roty, -rotx, 1.5f, 1.5f, 0.01f, 0);

	// Spawn smoke effect (convert rotation to direction vector)
	NJS_VECTOR velo = { 0.0f, 0.0f, 1.0f };
	njPushMatrix(_nj_unit_matrix_);
	njRotateY_(roty);
	njRotateX_(rotx);
	njCalcVector(0, &velo, &velo);
	njPopMatrixEx();
	CreateSmoke(&twp->pos, &velo, 1.0f);

	dsPlay_oneshot_Dolby(464, 0, 0, 60, 120, (taskwk*)twp);
}

void RexonHead_DrawMouth(taskwk* twp, NJS_OBJECT* object, float mouthopen)
{
	njPushMatrixEx();
	njTranslate(_nj_current_matrix_ptr_, object->pos[0], object->pos[1], object->pos[2]);
	njRotateXYZ(_nj_current_matrix_ptr_, object->ang[0], object->ang[1], object->ang[2]);
	njRotateZ(0, -static_cast<Angle>(mouthopen));
	dsDrawModel(object->basicdxmodel);
	njPopMatrixEx();

	DrawObjectRoot(object->sibling);
}

void __cdecl RexonHeadDisp(task* tp)
{
	if (!MissedFrames)
	{
		auto twp = tp->twp;
		auto ptwp = tp->ptp->twp;
		auto object = (NJS_OBJECT*)twp->value.ptr;

		___dsSetPalette(6);
		njSetTexture(&REXON_TexList);
		njPushMatrixEx();
		njTranslateEx(&twp->pos);
		njRotateY_(twp->ang.y);
		njScalef(ptwp->scl.x);

		dsDrawModel(object->basicdxmodel);

		RexonHead_DrawMouth(twp, object->child, twp->scl.x);

		njPopMatrixEx();
		___dsSetPalette(0);
	}
}

void __cdecl RexonHeadExec(task* tp)
{
	auto twp = tp->twp;
	auto ewp = (enemywk*)tp->mwp;
	auto ptwp = tp->ptp->twp;

	twp->ang.y = ptwp->ang.y;

	switch (twp->mode)
	{
	case RexonHeadAct_Normal:
		if (twp->wtimer == 0 && EnemySearchPlayer(twp, ewp) && (rand() % 30) == 0)
		{
			twp->wtimer = 80 + (rand() % 50);
			twp->wtimer = 1;
			twp->mode = RexonHeadAct_Attacking;
		}

		if (twp->wtimer > 0)
		{
			--twp->wtimer;
		}
		else
		{
			twp->wtimer = 0;
		}

		break;
	case RexonHeadAct_Attacking:
		++twp->wtimer;
		twp->scl.x = -(0x2000 * fmin(0.0f, njSin(static_cast<int>(twp->wtimer * 1000)))); // mouth opening animation

		if (twp->wtimer == 0x25)
		{
			RexonHead_LoadFireBall(tp, twp);
		}

		if (twp->wtimer > 0x40 && twp->scl.x == 0)
		{
			twp->mode = RexonHeadAct_Normal;
			twp->wtimer = 1;
		}

		break;
	}

	if (EnemyCheckDamage(twp, ewp))
	{
		CreateFlash2(twp->pos.x, twp->pos.y + 3.0f, twp->pos.z, 1.0f);
		CreateExpSpring(twp, 2);
		CreateAnimal(3, twp->pos.x, twp->pos.y + 6.0f, twp->pos.z);
		Knuckles_KakeraGame_Set_PutEme(twp->ang.z, &twp->pos);
		FreeTask(tp);
		E102KillCursor(tp);

		// No respawn flag
		SetBroken(tp->ptp);

		return;
	}

	Knuckles_KakeraGame_Set_CheckEme(twp->ang.z, &twp->pos);
	LoopTaskC(tp);
	EntryColliList(twp);
	tp->disp(tp);
}

void __cdecl RexonHead(task* tp)
{
	auto twp = tp->twp;
	auto ptwp = tp->ptp->twp;
	auto enmwk = EnemyInitialize(tp, twp);

	CCL_Init(tp, &RexonHead_Col, 1, 3);

	// Scale collision:
	auto cwp = twp->cwp;
	if (cwp)
	{
		cwp->info->a *= ptwp->scl.x;
		cwp->info->center.x *= ptwp->scl.x;
		cwp->info->center.y *= ptwp->scl.x;
	}

	twp->value.ptr = ((NJS_OBJECT*)ptwp->value.ptr)->child->sibling->sibling->sibling->sibling->child->child->child->child; // head node
	twp->scl.x = 0; // mouth opening amount
	twp->wtimer = 0;

	enmwk->view_range = 10000.0f; // FOV effective radius
	enmwk->view_angle = 0x2500; // FOV
	enmwk->hear_range = 0.0f; // Enemy should only react to vision

	twp->ang.z = ptwp->ang.z;
	Knuckles_KakeraGame_Set_CheckEme(twp->ang.z, &twp->pos);

	tp->dest = UniDestructor;
	tp->exec = RexonHeadExec;
	tp->disp = RexonHeadDisp;
}

#pragma endregion

#pragma region Rexon

inline float Rexon_NeckMovement(int i, int action, float timer)
{
	float move = 0;

	if (i > 0)
	{
		if (action == RexonAct_Stand)
		{
			move = 0.25f * njSin(static_cast<int>(timer * 100));
		}
		else
		{
			move = 0.35f * njSin(static_cast<int>(timer * 500));
		}
	}

	if (i == 2)
	{
		move = -move;
	}

	return move;
}

void Rexon_GetHeadPos(taskwk* twp, NJS_VECTOR* headpos)
{
	NJS_OBJECT* object = ((NJS_OBJECT*)twp->value.ptr)->child->sibling->sibling->sibling->sibling; // neck node

	njPushMatrix(_nj_unit_matrix_);
	njRotateY_(twp->ang.y);
	njScalef(twp->scl.x);

	for (int i = 0; i < 4; ++i)
	{
		njTranslate(_nj_current_matrix_ptr_, object->pos[0], object->pos[1], object->pos[2]);
		njRotateXYZ(_nj_current_matrix_ptr_, object->ang[0], object->ang[1], object->ang[2]);

		if (i > 0)
		{
			njPushMatrixEx();
			njTranslateX(Rexon_NeckMovement(i, twp->mode, twp->wtimer));

			// Get the head position
			if (i == 3)
			{
				njGetTranslation(nullptr, headpos);
				njAddVector(headpos, &twp->pos);
			}

			njPopMatrixEx();
		}

		object = object->child;
	}

	njPopMatrixEx();
}

void Rexon_MoveColliNeck(taskwk* twp)
{
	NJS_OBJECT* object = ((NJS_OBJECT*)twp->value.ptr)->child->sibling->sibling->sibling->sibling; // neck node

	njPushMatrix(_nj_unit_matrix_);
	njScalef(twp->scl.x);

	for (int i = 0; i < 4; ++i)
	{
		njTranslate(_nj_current_matrix_ptr_, object->pos[0], object->pos[1], object->pos[2]);
		njRotateXYZ(_nj_current_matrix_ptr_, object->ang[0], object->ang[1], object->ang[2]);

		if (i > 0)
		{
			njPushMatrixEx();
			njTranslateX(Rexon_NeckMovement(i, twp->mode, twp->wtimer));
			njGetTranslation(nullptr, &twp->cwp->info[i].center); // move collision
			njPopMatrixEx();
		}

		object = object->child;
	}

	njPopMatrixEx();
}
void RexonMove(task* tp, taskwk* twp)
{
	// Move up and down to simulate floating
	twp->timer.l += 200;
	twp->pos.y = twp->scl.z + 1.0f + (1.0f * njSin(twp->timer.l));

	auto geocol = (NJS_OBJECT*)twp->counter.ptr;

	MoveGeoCollision(tp, geocol, &twp->pos);
	RotYGeoCollision(tp, geocol, twp->ang.y);
}

void Rexon_DrawFins(taskwk* twp, NJS_OBJECT* object, float timer)
{
	for (int i = 0; i < 4; ++i)
	{
		int rot = 0;

		if (twp->mode == RexonAct_Stand)
		{
			rot = 0x2000 * njSin(static_cast<int>(timer * 100));
		}
		else
		{
			rot = 0x3000 * njSin(static_cast<int>(timer * 500));
		}

		njPushMatrixEx();
		njTranslateEx((NJS_POINT3*)object->pos);
		njRotateEx((Angle*)object->ang, FALSE);
		njRotateY_(rot);
		njRotateX_(rot / 4);
		njScaleEx((NJS_POINT3*)object->scl);
		dsDrawModel(object->basicdxmodel);
		njPopMatrixEx();

		object = object->sibling;
	}
}

void Rexon_DrawNeck(taskwk* twp, NJS_OBJECT* object, float timer)
{
	njPushMatrixEx();

	for (int i = 0; i < 4; ++i)
	{
		njTranslateEx((NJS_POINT3*)object->pos);
		njRotateEx((Angle*)object->ang, FALSE);

		njPushMatrixEx();
		njTranslateX(Rexon_NeckMovement(i, twp->mode, timer));
		dsDrawModel(object->basicdxmodel);
		njPopMatrixEx();

		object = object->child;
	}

	njPopMatrixEx();
}

void __cdecl RexonDisp(task* tp)
{
	if (!MissedFrames)
	{
		auto twp = tp->twp;
		auto object = (NJS_OBJECT*)twp->value.ptr; // stored model

		___dsSetPalette(6);
		njSetTexture(&REXON_TexList);
		njPushMatrixEx();
		njTranslateEx(&twp->pos);
		njRotateY_(twp->ang.y);
		njScalef(twp->scl.x);

		dsDrawModel(object->basicdxmodel);

		if (twp->mode != RexonAct_Dead)
		{
			Rexon_DrawFins(twp, object->child, twp->wtimer);
			Rexon_DrawNeck(twp, object->child->sibling->sibling->sibling->sibling, twp->wtimer);
		}

		njPopMatrixEx();
		___dsSetPalette(0);
	}
}

void __cdecl RexonExec(task* tp)
{
	if (!CheckRangeOut(tp))
	{
		auto twp = tp->twp;

		// Handle growing lava in act 2:
		int lava_id = twp->ang.x;
		if (ssActNumber == 1 && grow_workers[lava_id].Enabled == true)
		{
			if (twp->scl.z < grow_workers[lava_id].Height)
			{
				twp->scl.z = grow_workers[lava_id].Height; // adjust height to new lava height
			}
		}
		
		if (twp->mode != RexonAct_Dead)
		{
			twp->wtimer += 1;

			// If the rexon is a moving one
			if (twp->mode == RexonAct_Move)
			{
				// Turn around if outside of range
				if (CheckCollisionPointSphere((NJS_POINT3*)&tp->ocp->pObjEditEntry->xpos, &twp->pos, twp->scl.y) == false)
				{
					twp->ang.y += 0x8000;
				}

				// Move forward
				njPushMatrix(_nj_unit_matrix_);
				njTranslateEx(&twp->pos);
				njRotateY(0, twp->ang.y);
				njTranslateX(twp->scl.y / 1000);
				njGetTranslation(0, &twp->pos);
				njPopMatrixEx();
			}

			// If the head is still there
			if (tp->ctp)
			{
				Rexon_MoveColliNeck(twp); // calc neck parts pos
				Rexon_GetHeadPos(twp, &tp->ctp->twp->pos); // calc head pos
				EntryColliList(twp);
			}
			else
			{
				// Otherwise we're dead
				twp->mode = RexonAct_Dead;
				FreeColliWork(twp);
			}

			LoopTaskC(tp);
		}

		RexonMove(tp, twp);
		EntryColliList(twp);
		tp->disp(tp);
	}
}

void __cdecl RexonDestroy(task* tp)
{
	B_Destructor(tp); // destructor function that removes geo collision in twp->count
	UniDestructor(tp); // common enemy destructor that does nothing
}

void __cdecl Rexon(task* tp)
{
	auto twp = tp->twp;

	// If already dead, load only the floating shell
	if (CheckBroken(tp))
	{
		twp->mode = RexonAct_Dead;
	}
	else
	{
		CreateChildTask(IM_TWK, RexonHead, tp);
		CCL_Init(tp, arrayptrandlength(Rexon_Col), 4);

		// Scale collision:
		auto cwp = twp->cwp;
		if (cwp)
		{
			for (int i = 0; i < cwp->nbInfo; ++i)
			{
				cwp->info[i].a *= twp->scl.x;
				cwp->info[i].center.x *= twp->scl.x;
				cwp->info[i].center.y *= twp->scl.x;
			}
		}
	}

	if (twp->scl.x == 0.0f)
	{
		twp->scl.x = 1.0f; // default scale
	}

	if (twp->scl.y != 0.0f)
	{
		twp->mode = RexonAct_Move;
	}

	twp->ang.y -= 0x4000; // correct rotation
	twp->scl.z = twp->pos.y; // store original height
	twp->value.ptr = e_rexon->getmodel(); // store model

	tp->dest = RexonDestroy;
	tp->exec = RexonExec;
	tp->disp = RexonDisp;

	// Create geometry collision for shell:
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

	object->basicdxmodel = e_rexoncol->getmodel()->basicdxmodel;

	RegisterCollisionEntry(ColFlags_Dynamic | ColFlags_Solid, tp, object);
	twp->counter.ptr = object; // store geo collision
}

#pragma endregion

void Rexon_LoadAssets()
{
	LoadModelFile(&e_rexon, "e_rexon", ModelFormat_Basic);
	LoadModelFile(&e_rexoncol, "e_rexoncol", ModelFormat_Basic);
}

void Rexon_FreeAssets()
{
	FreeFileInfo(&e_rexon);
	FreeFileInfo(&e_rexoncol);
}