#include "pch.h"
#include "o_hillflowers.h"

/*

Object that spawns different kind of flowers.

ScaleX: Global scale
ScaleY: Additional Y scale
ScaleZ: Flower ID

*/

ModelInfo* ht_flower0 = nullptr;
ModelInfo* ht_flower1 = nullptr;
ModelInfo* ht_flowerpatch0 = nullptr;
ModelInfo* ht_flowerpatch1 = nullptr;

AnimationFile* ht_flower0_anm = nullptr;
AnimationFile* ht_flower1_anm = nullptr;
AnimationFile* ht_flowerpatch1_anm = nullptr;

NJS_ACTION Flower0_Action = { 0 };
NJS_ACTION Flower1_Action = { 0 };
NJS_ACTION Flowerpatch1_Action = { 0 };

void __cdecl HillFlowersDisplay(task* tp)
{
	if (!MissedFrames)
	{
		auto twp = tp->twp;

		SetSecondObjectTexture();
		njPushMatrixEx();
		njTranslateEx(&twp->pos);
		njRotateEx((Angle*)&twp->ang, false);

		njScale(nullptr, twp->scl.x, twp->scl.x + twp->scl.y, twp->scl.x);

		njScaleY(twp->counter.f); // bouncy animation

		if (twp->value.ptr != nullptr) // If there is an animation
		{ 
			njAction((NJS_ACTION*)twp->value.ptr, twp->scl.z);
		}
		else
		{
			dsDrawModel(((NJS_OBJECT*)twp->timer.ptr)->basicdxmodel);
		}

		njPopMatrixEx();
	}
}

void __cdecl HillFlowersExec(task* tp)
{
	if (!CheckRangeOut(tp))
	{
		auto twp = tp->twp;

		twp->counter.f = 1.0f + (1.0f - njSin(twp->wtimer * 0x7D0)) / 50.0f; // Bouncy animation

		if (twp->value.ptr != nullptr) // If there is an animation
		{
			twp->scl.z = 15.0f + (njSin(twp->wtimer * 0x200) * 15.0f); // Calculate current animation frame
		}
		
		twp->wtimer += 1; // bouncy animation
		
		tp->disp(tp);
	}
}

void __cdecl HillFlowers(task* tp)
{
	auto twp = tp->twp;

	// Choose the model and collison based on set information
	switch (static_cast<int>(twp->scl.z) % 6) {
	case 0:
	default:
		twp->value.ptr = &Flower0_Action;
		break;
	case 1:
		twp->value.ptr = (Loop*)&Flower1_Action;
		break;
	case 2:
		twp->timer.ptr = ht_flowerpatch0->getmodel();
		break;
	case 3:
		twp->value.ptr = (Loop*)&Flowerpatch1_Action;
		break;
	case 4:
		twp->timer.ptr = (NJS_OBJECT*)ht_flowerpatch1->getdata("Flower0");
		break;
	case 5:
		twp->timer.ptr = (NJS_OBJECT*)ht_flowerpatch1->getdata("Flower1");
		break;
	}

	// If the scale is null, set it to normal
	if (twp->scl.x == 0.0f)
	{
		twp->scl.x = 1.0f;
	}

	twp->wtimer = static_cast<unsigned short>(rand());

	tp->exec = HillFlowersExec;
	tp->disp = HillFlowersDisplay;
}

void HillFlowers_LoadAssets()
{
	LoadModelFile(&ht_flower0, "ht_flower0", ModelFormat_Basic);
	LoadModelFile(&ht_flower1, "ht_flower1", ModelFormat_Basic);
	LoadModelFile(&ht_flowerpatch0, "ht_flowerpatch0", ModelFormat_Basic);
	LoadModelFile(&ht_flowerpatch1, "ht_flowerpatch1", ModelFormat_Basic);

	LoadAnimationFile(&ht_flower0_anm, "ht_flower0");
	LoadAnimationFile(&ht_flower1_anm, "ht_flower1");
	LoadAnimationFile(&ht_flowerpatch1_anm, "ht_flowerpatch1");

	Flower0_Action.object = ht_flower0->getmodel();
	Flower0_Action.motion = ht_flower0_anm->getmotion();

	Flower1_Action.object = ht_flower1->getmodel();
	Flower1_Action.motion = ht_flower1_anm->getmotion();

	Flowerpatch1_Action.object = ht_flowerpatch1->getmodel();
	Flowerpatch1_Action.motion = ht_flowerpatch1_anm->getmotion();
}

void HillFlowers_FreeAssets()
{
	FreeFileInfo(&ht_flower0);
	FreeFileInfo(&ht_flower1);
	FreeFileInfo(&ht_flowerpatch0);
	FreeFileInfo(&ht_flowerpatch1);
	FreeFileInfo(&ht_flower0_anm);
	FreeFileInfo(&ht_flower1_anm);
	FreeFileInfo(&ht_flowerpatch1_anm);
}