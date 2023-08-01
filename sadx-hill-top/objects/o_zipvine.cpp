#include "pch.h"
#include "multi.h"
#include "o_transporter.h"
#include "o_hillpole.h"
#include "o_zipvine.h"
#include "o_lantern.h"

/*

Long vines that acts like a zipline.

Path object, twp->value contains the path information.
See paths.cpp for path data.

*/

#define PNUM(twp) twp->btimer

enum VineMode
{
	VineMode_Input,
	VineMode_Run,
	VineMode_Reset
};

CCL_INFO ZipVine_Col = { 0, CI_FORM_SPHERE, 0xF0, 0, 0, { 0.0f, 0.0f, 0.0f }, 10.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0 };

void DrawZipVinePole(NJS_OBJECT* poles, NJS_VECTOR* pos, Angle rot)
{
	njPushMatrixEx();
	njTranslateEx(pos);
	njRotateY_(rot);
	dsDrawModel(poles->basicdxmodel);
	dsDrawModel(poles->child->basicdxmodel);
	njPopMatrixEx();
}

void DrawZipVine(pathtag* path, NJS_OBJECT* vine)
{
	for (int i = 0; i < path->points - 1; ++i) // Loop through all vines
	{
		auto spoint = &path->tblhead[i];
		auto epoint = &path->tblhead[i + 1];

		float Y = epoint->ypos - spoint->ypos; // Vine height
		float Z = sqrtf(powf(epoint->xpos - spoint->xpos, 2) + powf(epoint->zpos - spoint->zpos, 2)); // Vine length

		Angle y;
		njLookAt((NJS_VECTOR*)&spoint->xpos, (NJS_VECTOR*)&epoint->xpos, nullptr, &y);

		njPushMatrixEx();
		njTranslateEx((NJS_VECTOR*)&spoint->xpos);
		njRotateY_(y);
		DrawVine(vine, (NJS_VECTOR*)&spoint->xpos, Y, Z, 0.0f);
		njPopMatrixEx();
	}
}

void __cdecl ZipVineDisplay(task* tp)
{
	if (!MissedFrames)
	{
		auto twp = tp->twp;
		auto path = (pathtag*)twp->value.ptr;
		auto hangvineobj = ht_transporter->getmodel()->child->child;
		auto vineobj = ht_vine->getmodel();

		SetSecondObjectTexture();

		DrawZipVine(path, vineobj);

		// Draw hanging vine
		njPushMatrixEx();
		njTranslateEx(&twp->pos);
		njRotateY_(twp->ang.y);
		njRotateZ_(twp->ang.z);
		dsDrawModel(hangvineobj->basicdxmodel);
		njPopMatrixEx();
	}
}

bool ZipVineUpdatePos(pathtag* tag, taskwk* twp)
{
	pathinfo info;
	info.onpathpos = twp->counter.f;

	// Get position along path
	if (GetStatusOnPath(tag, &info) == TRUE)
	{
		twp->pos = { info.xpos, info.ypos - 23.0f, info.zpos };
		//twp->ang.y = 0x4000 - NJM_RAD_ANG(atan2f(info.front.x, info.front.z));
		twp->ang.y = info.slangz;
		return true;
	}
	else
	{
		return false;
	}
}

void __cdecl ZipVineExec(task* tp)
{
	auto twp = tp->twp;
	auto path = (pathtag*)twp->value.ptr;
	auto& progress = twp->counter.f;

	// If act changed, remove the object (not automatic for path tasks)
	if (twp->timer.w[0] != GetStageNumber())
	{
		FreeTask(tp);
		return;
	}

	if (twp->mode == VineMode_Input)
	{
		if (IsMultiplayerActive())
		{
			int pnum = CheckCollisionP(&twp->pos, 14.0f) - 1;
			if (pnum >= 0 && pnum < MaxPlayers)
			{
				twp->mode = VineMode_Run;
				SetInputP(pnum, PL_OP_PLACEWITHHUNG);
				PNUM(twp) = pnum;
			}
		}
		else
		{
			// In single player, we don't want AI to steal the vine
			if (CheckCollisionP_num(&twp->pos, 14.0f, 0))
			{
				twp->mode = VineMode_Run;
				SetInputP(0, PL_OP_PLACEWITHHUNG);
				PNUM(twp) = 0;
			}
		}
	}
	else if (twp->mode == VineMode_Run)
	{
		if (ZipVineUpdatePos(path, twp))
		{
			if (!twp->smode)
			{
				ForcePlayerPos(PNUM(twp), twp->pos.x, twp->pos.y - 7.5f, twp->pos.z);
				RotatePlayer(PNUM(twp), twp->ang.y);
			}
			progress += 5.0f;
			dsPlay_timer_v(465, 0, 0, 0, 1, twp->pos.x, twp->pos.y, twp->pos.z);
		}
		else
		{
			twp->mode = VineMode_Reset;
			SetInputP(PNUM(twp), PL_OP_LETITGO);
		}

		if (twp->wtimer)
		{
			--twp->wtimer;
		}
		else if (CheckJump(PNUM(twp)))
		{
			twp->smode = 1;
			twp->wtimer = 50;
		}

		if (GetDistance(&playertwp[PNUM(twp)]->pos, &twp->pos) > 300.0f)
		{
			twp->mode = VineMode_Reset;
		}
	}
	else
	{
		twp->smode = 0;
		progress = 10.0f;
		ZipVineUpdatePos(path, twp);
		twp->mode = VineMode_Input;
	}
	
	LoopTaskC(tp);
	EntryColliList(twp);
	tp->disp(tp);
}

void SpawnPole(task* tp, pathtbl* pt)
{
	auto ctp = CreateChildTask(IM_TWK, HillPole, tp);
	ctp->twp->pos = { pt->xpos, pt->ypos - 37.0f, pt->zpos };
}

void __cdecl ZipVine(task* tp)
{
	auto twp = tp->twp;
	auto path = (pathtag*)twp->value.ptr;
	
	twp->counter.f = 10.0f;
	ZipVineUpdatePos(path, twp);

	SpawnPole(tp, &path->tblhead[0]);
	SpawnPole(tp, &path->tblhead[path->points - 1]);
	FireFly_Load(tp, 3);

	CCL_Init(tp, &ZipVine_Col, 1, 3);

	// Bake Y pos until not lazy to set normal manually
	for (int i = 0; i < path->points - 1; ++i)
	{
		njLookAt((NJS_VECTOR*)&path->tblhead[i].xpos, (NJS_VECTOR*)&path->tblhead[i + 1].xpos, nullptr, (Angle*)&path->tblhead[i].slangz);
	}

	tp->exec = ZipVineExec;
	tp->disp = ZipVineDisplay;
}