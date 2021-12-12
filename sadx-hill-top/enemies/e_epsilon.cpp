#include "pch.h"
#include "e_epsilon.h"

// Only start the fight if the player is above the platform
int __cdecl E104_CheckPlayer(task* tp)
{
	return CheckCollisionP_num(&tp->twp->pos, 150.0f, 0) == true && (playertwp[0]->flag & Status_Ground);
}

// Bad attempt at preventing Epsilon from falling down the arena
void E104OutOfBounds(taskwk* twp)
{
	if (twp->pos.y < 945.0f)
	{
		twp->btimer = 1;
	}

	if (twp->btimer == 1)
	{
		twp->pos.y += 1.0f;

		if (twp->pos.y > 970.0f)
		{
			twp->btimer = 0;
		}
	}
}

void __cdecl E104Enemy_Main_j(task* tp)
{
	E104OutOfBounds(tp->twp);
	EnemyMountainE104_execTgt(tp);
}

void EpsilonBoss_Init()
{
	// New fight boundaries for the camera
	pCamTgtLinePos[0] = { 867.7648f, 996.625f, -600.5748f };
	pCamTgtLinePos[1] = { 990.7285f, 996.625f, -368.2955f };
	pCamTgtLinePos[2] = { 965.4641f, 996.625f, -203.8346f };
	pCamTgtLinePos[3] = { 845.3178f, 996.625f, -312.3701f };
	pCamTgtLinePos[4] = { 698.6425f, 1016.625f, -588.8475f };

	WriteJump((void*)0x604650, E104_CheckPlayer);
	WriteData((TaskFuncPtr*)0x605C25, E104Enemy_Main_j);
}