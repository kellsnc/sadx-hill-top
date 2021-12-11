#include "pch.h"
#include "o_hillgrass.h"

/*

Simple grass object with an animation.
ScaleY: Y scale.

*/

ModelInfo* ht_grass0 = nullptr;

AnimationFile* ht_grass0_anm = nullptr;

NJS_ACTION Grass0_Action = { 0 };

void __cdecl HillGrassDisplay(task* tp)
{
	if (!MissedFrames)
	{
		auto twp = tp->twp;

		njSetTexture(&HillTopOBJ_TexList);
		njPushMatrixEx();
		njTranslateEx(&twp->pos);
		njRotateEx((Angle*)&twp->ang, false);

		njScaleY(1.0f + twp->scl.y);

		// Only process animation if close enough
		if (CheckCollisionP(&twp->pos, 400.0f))
		{
			njAction(&Grass0_Action, twp->scl.z);
		}
		else
		{
			DrawObject(Grass0_Action.object);
		}

		njPopMatrixEx();
	}
}

void __cdecl HillGrassExec(task* tp)
{
	if (!CheckRangeOut(tp))
	{
		auto twp = tp->twp;

		// Animate
		if (!MissedFrames)
		{
			twp->scl.z += 0.5f;
		}
		
		tp->disp(tp);
	}
}

void __cdecl HillGrass(task* tp)
{
	auto twp = tp->twp;

	twp->scl.z = rand(); // randomize animation start

	tp->exec = HillGrassExec;
	tp->disp = HillGrassDisplay;
}

void HillGrass_LoadAssets()
{
	LoadModelFile(&ht_grass0, "ht_grass0", ModelFormat_Basic);
	LoadAnimationFile(&ht_grass0_anm, "ht_grass0");

	Grass0_Action.object = ht_grass0->getmodel();
	Grass0_Action.motion = ht_grass0_anm->getmotion();
}

void HillGrass_FreeAssets()
{
	FreeFileInfo(&ht_grass0);
	FreeFileInfo(&ht_grass0_anm);
}