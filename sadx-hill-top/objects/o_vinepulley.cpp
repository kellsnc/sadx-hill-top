#include "pch.h"
#include "o_vinepulley.h"
#include "o_lantern.h"
#include "o_transporter.h"

/*

Vine that acts like a pulley.

Position: where the vine starts
Scale X: distance to where the player grabs the vine
Scale Y: distance to where the vine stops

*/

CCL_INFO VinePulley_Col = { 0, CI_FORM_SPHERE, 0xF0, 0, 0, {0.0f, 0.0f, 0.0f}, 8.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0 };

void __cdecl VinePulleyTarget(task* tp)
{
	LoopTaskC(tp);
	EntryColliList(tp->twp);
}

void __cdecl VinePulleyDisplay(task* tp)
{
	if (!MissedFrames)
	{
		auto twp = tp->twp;
		auto ctwp = tp->ctp->twp;
		auto object = (NJS_OBJECT*)twp->value.ptr;

		SetSecondObjectTexture();
		njPushMatrixEx();
		njTranslateEx(&ctwp->pos);
		njScaleY(twp->scl.z / 23);
		dsDrawModel(object->basicdxmodel);
		njPopMatrixEx();
	}
}

void __cdecl VinePulleyExec(task* tp)
{
	if (!CheckRangeOut(tp))
	{
		auto twp = tp->twp;
		auto ctwp = tp->ctp->twp;

		if (twp->mode == 0)
		{
			if (twp->scl.z < twp->scl.x)
			{
				twp->scl.z += 5.0f;
				ctwp->pos.y = twp->pos.y - twp->scl.z;
			}

			if (twp->wtimer > 0)
			{
				--twp->wtimer;
			}
			else
			{
				taskwk* player = (taskwk*)GetCollidingEntityA((EntityData1*)ctwp);

				if (player)
				{
					twp->btimer = TASKWK_PLAYERID(player);
					SetInputP(0, 16);
					dsPlay_oneshot(463, 0, 0, 0);
					twp->mode = 1;
				}
			}
		}
		else
		{
			int player_id = static_cast<int>(twp->btimer);

			if (twp->scl.z > twp->scl.y)
			{
				twp->scl.z -= 5.0f;
				ctwp->pos.y = twp->pos.y - twp->scl.z;
			}

			if (CheckJump(player_id))
			{
				twp->mode = 0;
				twp->wtimer = 100;
			}

			ForcePlayerPos(player_id, &ctwp->pos);
			playertwp[player_id]->ang.y = twp->ang.y;
		}

		tp->disp(tp);
	}

	LoopTaskC(tp);
}

void __cdecl VinePulley(task* tp)
{
	auto twp = tp->twp;

	twp->value.ptr = ht_transporter->getmodel()->child->child;

	auto ctp = CreateChildTask(IM_TWK, VinePulleyTarget, tp);
	ctp->twp->pos.y = twp->pos.y - twp->scl.x;
	CCL_Init(ctp, &VinePulley_Col, 1, 3);
	FireFly_Load(ctp, 3);

	twp->scl.z = twp->scl.x;

	tp->exec = VinePulleyExec;
	tp->disp = VinePulleyDisplay;
}