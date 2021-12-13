#include "pch.h"
#include "o_hillplatform.h"

/*

Platform that goes up and down in lava.

ScaleX = scale
ScaleY = movement length
RotZ = speed (default to 150)

*/

ModelInfo* ht_platform = nullptr;

static void __cdecl HillPlatformDestroy(task* tp)
{
	if (tp->twp)
	{
		auto object = (NJS_OBJECT*)tp->twp->value.ptr;

		if (object)
		{
			WithdrawCollisionEntry(tp, object);  // Destroy the geometry collision
			ReleaseMobileLandObject(object);     // Release the entry
		}
	}
}

static void __cdecl HillPlatformDisplay(task* tp)
{
	if (!MissedFrames)
	{
		auto twp = tp->twp;
		auto object = (NJS_OBJECT*)twp->value.ptr;

		SetLevelTexture();
		njPushMatrixEx();
		njTranslateEx(&twp->pos);
		njRotateY_(twp->ang.y);
		njScalef(twp->scl.x);
		dsDrawModel(object->basicdxmodel);
		njPopMatrixEx();
	}
}

static void __cdecl HillPlatformExec(task* tp)
{
	if (!CheckRangeOut(tp))
	{
		auto twp = tp->twp;

		// Update height
		twp->pos.y = twp->scl.z + static_cast<float>(1.0 - pow(njSin(GameTimer * twp->ang.z), 2.0)) * twp->scl.y;

		// Update geometry collision
		reinterpret_cast<NJS_OBJECT*>(twp->value.ptr)->pos[1] = twp->pos.y;

		tp->disp(tp);
	}
}

void __cdecl HillPlatform(task* tp)
{
	auto twp = tp->twp;

	if (twp->scl.x == 0.0f)
	{
		twp->scl.x = 1.0f; // default scale
	}

	if (twp->ang.z == 0)
	{
		twp->ang.z = 150; // default speed
	}

	twp->scl.z = twp->pos.y; // store original height

	// Create the dynamic collision
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

	object->basicdxmodel = ht_platform->getmodel()->basicdxmodel;

	RegisterCollisionEntry(0x4000 | ColFlags_Dynamic | ColFlags_Solid | ColFlags_UseRotation, tp, object);

	twp->value.ptr = object;
	
	tp->exec = HillPlatformExec;
	tp->disp = HillPlatformDisplay;
	tp->dest = HillPlatformDestroy;
}

void HillPlatform_LoadAssets()
{
	LoadModelFile(&ht_platform, "ht_platform", ModelFormat_Basic);
}

void HillPlatform_FreeAssets()
{
	FreeFileInfo(&ht_platform);
}