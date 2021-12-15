#include "pch.h"
#include "objects.h"
#include "o_lantern.h"

/*

A tiny lantern object with fireflies flying around it.

ScaleX: amount of firefiles.

*/

ModelInfo* ht_lantern = nullptr;

static NJS_TEXANIM	FIREFLY_TEXANIM = { 0x1, 0x1, 0, 0, 0, 0, 0x100, 0x100, 7, 0x20 };
static NJS_SPRITE FIREFLY_SPRITE = { { 0.0f, 0.0f, 0.0f }, 1.0f, 1.0f, 0, &HillTopOBJ_TexList, &FIREFLY_TEXANIM };

void __cdecl FireFlyDisplay(task* tp)
{
	if (!MissedFrames)
	{
		auto twp = tp->twp;

		njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
		njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
		SetMaterial(1.0f, 1.0f, 1.0f, 1.0f);
		SetSecondObjectTexture();
		njPushMatrixEx();
		njTranslateEx(&twp->scl);
		njRotateY_(Camera_Data1->Rotation.y);
		njRotateX_(Camera_Data1->Rotation.x);
		njDrawSprite3D(&FIREFLY_SPRITE, 0, NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR);
		njPopMatrixEx();
		ResetMaterial();
		SetDefaultAlphaBlend();
	}
}

void __cdecl FireFlyExec(task* tp)
{
	auto twp = tp->twp;

	// Rotate around axis
	tp->twp->ang.y += static_cast<Angle>(tp->twp->wtimer);

	// Precalculate position
	njPushMatrix(_nj_unit_matrix_);
	njTranslateEx(&twp->pos); // center position
	njRotateZXY(&twp->ang); // rotate
	njTranslateX(twp->value.f); // move by distance from the center
	njGetTranslation(_nj_current_matrix_ptr_, &twp->scl); // get position
	njPopMatrixEx();

	// Draw
	tp->disp(tp);
}

void __cdecl FireFly_Load(task* ptp, int count)
{
	for (int i = 0; i < count; ++i)
	{
		auto ctp = CreateChildTask(IM_TWK, FireFlyExec, ptp);
		auto twp = ctp->twp;
		
		twp->value.f = 4.0f + static_cast<float>(rand() % 5);
		twp->wtimer = 500 + (rand() % 250);

		twp->ang.x = rand();
		twp->ang.z = rand();

		ctp->disp = FireFlyDisplay;
	}
}

void __cdecl LanternDisplay(task* tp)
{
	if (!MissedFrames)
	{
		auto twp = tp->twp;
		auto object = (NJS_OBJECT*)twp->value.ptr;

		SetSecondObjectTexture();
		njPushMatrixEx();
		njTranslateEx(&twp->pos);
		njRotateEx((Angle*)&twp->ang, false);
		njRotateX_(twp->counter.l); // balancing rotation
		dsDrawModel(object->basicdxmodel);
		njPopMatrixEx();
	}
}

void __cdecl LanternExec(task* tp)
{
	if (!CheckRangeOut(tp))
	{
		auto twp = tp->twp;

		// Animate
		twp->wtimer += 0x300;
		twp->counter.l = static_cast<int>(1.0f - njSin(twp->wtimer) * 500.0f);

		LoopTaskC(tp); // run firefly tasks if any
		tp->disp(tp);
	}
}

void __cdecl HillLantern(task* tp)
{
	auto twp = tp->twp;

	twp->wtimer = rand(); // randomize animation start
	twp->value.ptr = ht_lantern->getmodel();

	if (twp->scl.x)
	{
		FireFly_Load(tp, static_cast<int>(twp->scl.x) % 4);
	}

	tp->exec = LanternExec;
	tp->disp = LanternDisplay;
}

void Lantern_LoadAssets()
{
	LoadModelFile(&ht_lantern, "ht_lantern", ModelFormat_Basic);
}

void Lantern_FreeAssets()
{
	FreeFileInfo(&ht_lantern);
}