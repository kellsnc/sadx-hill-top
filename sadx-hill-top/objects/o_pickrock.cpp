#include "pch.h"
#include "o_pickrock.h"

/*

A little rock that can be held and pushed.
Example of a holdable & pushable object.

*/

extern NJS_TEXLIST HillTop_TexList;

ModelInfo* ht_pickrock = nullptr;

CCL_INFO PickRockCol[] = {
	{ 0, CI_FORM_CYLINDER, 0x7 /* Pushable flag */, 0, 0, { 0.0f, 2.0f, 0.0f }, 4.0f, 3.0f, 0.0f },
	{ 0, CI_FORM_CYLINDER, 0x70, 0, 0x2400, { 0, 2, 0 }, 6.0f, 4.0f, 0.0f } // For detection, not solid
};

enum PicActions
{
	PicAction_Floor,
	PicAction_Grabbed
};

void __cdecl PickRockDestroy(task* tp)
{
	SetObjectStatusNotHeld(tp);
}

void __cdecl PickRockDisplay(task* tp)
{
	if (!MissedFrames)
	{
		auto twp = tp->twp;

		SetLevelTexture();
		njPushMatrixEx();
		njTranslateEx(&twp->pos);
		njRotateEx((Angle*)&twp->ang, false);
		DrawObject((NJS_OBJECT*)twp->value.ptr);
		njPopMatrixEx();
	}
}

void __cdecl PickRockExec(task* tp)
{
	if (!CheckRangeOut(tp))
	{
		auto twp = tp->twp;

		switch (twp->mode)
		{
		case PicAction_Floor:
			ObjectMovableSRegularExecute(tp);

			if (twp->flag & ObjFlag_Held)
			{
				twp->mode = PicAction_Grabbed;
			}

			break;
		case PicAction_Grabbed:
			ObjectMovableSRegularExecute(tp);

			EntityData1* player = GetCollidingEntityA((EntityData1*)twp);

			if (player)
			{
				twp->ang = { player->Rotation.x, -player->Rotation.y, player->Rotation.z };
			}

			if (twp->flag & ObjFlag_Held)
			{
				SetContinue(tp);
			}
			else
			{
				SetObjectStatusNotHeld(tp);
				twp->mode = PicAction_Floor;
			}

			break;
		}

		tp->disp(tp);
	}
}

void __cdecl PickRock(task* tp)
{
	auto twp = tp->twp;
	
	twp->value.ptr = ht_pickrock->getmodel();
	
	CCL_Init(tp, arrayptrandlength(PickRockCol), 4);
	ObjectMovableInitialize(twp, tp->mwp, 4);

	tp->dest = PickRockDestroy;
	tp->exec = PickRockExec;
	tp->disp = PickRockDisplay;
}

void PickRock_LoadAssets()
{
	LoadModelFile(&ht_pickrock, "ht_pickrock", ModelFormat_Basic);
}

void PickRocket_FreeAssets()
{
	FreeFileInfo(&ht_pickrock);
}