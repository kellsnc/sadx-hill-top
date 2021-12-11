#include "pch.h"
#include "o_hillbush.h"

/*

A simple bush object.

*/

ModelInfo* ht_bush = nullptr;

void __cdecl HillBushDisplay(task* tp)
{
	if (!MissedFrames)
	{
		auto twp = tp->twp;
		auto object = (NJS_OBJECT*)twp->value.ptr;
		
		SetSecondObjectTexture();
		njPushMatrixEx();
		njTranslateEx(&twp->pos);

		// Draw root
		njPushMatrixEx();
		{
			njRotateEx((Angle*)&twp->ang, false);
			DrawModel(object->basicdxmodel);
			njPopMatrixEx();
		}

		njRotateX_(twp->ang.x);
		njRotateZ_(twp->ang.z);
		njTranslateY(object->child->pos[1]);

		// Some animation
		njRotateX_(static_cast<int>(1.0f - njSin(twp->wtimer * 0x200) * 300.0f));
		njScaleY(1.0f + (1.0f - njSin(twp->wtimer * 0x5DC)) / 50.0f);

		DrawModel(object->child->basicdxmodel); // draw bush

		njPopMatrixEx();
	}
}

void __cdecl HillBushExec(task* tp)
{
	if (!CheckRangeOut(tp))
	{
		auto twp = tp->twp;

		// Animate
		if (!MissedFrames)
		{
			twp->wtimer += 1;
		}

		tp->disp(tp);
	}
}

void __cdecl HillBush(task* tp)
{
	auto twp = tp->twp;

	twp->wtimer = static_cast<unsigned short>(rand()); // Randomize animation start
	twp->value.ptr = ht_bush->getmodel(); // Store model in twp->value

	tp->exec = HillBushExec;
	tp->disp = HillBushDisplay;
}

void HillBush_LoadAssets()
{
	LoadModelFile(&ht_bush, "ht_bush", ModelFormat_Basic);
}

void HillBush_FreeAssets()
{
	FreeFileInfo(&ht_bush);
}