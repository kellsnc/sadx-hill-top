#include "pch.h"
#include "o_lavafall.h"

/*

A chunk model representing a fall of lava, the model bends with animations.

ScaleX: Global scale
ScaleY: Additional Y scale
RotZ: animation ID

*/

static ModelInfo* ht_lavafall = nullptr;

static AnimationFile* ht_lavafall0_anm = nullptr;
static AnimationFile* ht_lavafall1_anm = nullptr;
static AnimationFile* ht_lavafall2_anm = nullptr;
static AnimationFile* ht_lavafall3_anm = nullptr;

static NJS_ACTION LavaFall0_Action = { 0 };
static NJS_ACTION LavaFall1_Action = { 0 };
static NJS_ACTION LavaFall2_Action = { 0 };
static NJS_ACTION LavaFall3_Action = { 0 };

NJS_TEXNAME LAVAFALL_TexNames[16];
NJS_TEXLIST LAVAFALL_TexList = { arrayptrandlength(LAVAFALL_TexNames) };

// The actual lava texture we are going to draw
static NJS_TEXLIST CurrentLavaTex = { nullptr, 1 };

static CCL_INFO LavaFallColS = { 0, CI_FORM_CYLINDER, 0x77, 0x2F /* Hurt flag */, 0, {0.0f, -25.0f, 0.0f}, 8.0f, 25.0f, 0.0f, 0.0f, 0, 0, 0 };
static CCL_INFO LavaFallColM = { 0, CI_FORM_CAPSULE,  0x77, 0x2F, 0, {0.0f, -14.0f, -13.0f}, 10.0f, 8.0f, 0.0f, 0.0f, 0x2000, 0, 0 };
static CCL_INFO LavaFallColL = { 0, CI_FORM_CAPSULE,  0x77, 0x2F, 0, {0.0f, -15.0f, -24.0f}, 10.0f, 8.0f, 0.0f, 0.0f, 0x3000, 0, 0 };

void __cdecl LavaFallDisplay(task* tp)
{
	if (!MissedFrames)
	{
		auto twp = tp->twp;

		// Change tex address to animate textures
		CurrentLavaTex.textures = &LAVAFALL_TexNames[twp->btimer];

		njSetTexture(&CurrentLavaTex);
		njPushMatrixEx();
		njTranslateEx(&twp->pos);
		njRotateY_(twp->ang.y);
		njRotateX_(twp->ang.x);
		njScale(0, twp->scl.x, twp->scl.x + twp->scl.y, twp->scl.x);

		SetupWorldMatrix();
		Direct3D_SetChunkModelRenderState();
		njCnkAction((NJS_ACTION*)twp->value.ptr, twp->scl.z);
		Direct3D_UnsetChunkModelRenderState();

		njPopMatrixEx();
	}
}

void __cdecl LavaFallExec(task* tp)
{
	if (!CheckRangeOut(tp))
	{
		auto twp = tp->twp;

		twp->scl.z = 15.0f + (njSin(twp->wtimer * 0x200) * 15.0f); // animation frame
		
		// Texture animation
		if (!MissedFrames)
		{
			twp->wtimer += 1;

			if (FrameCounterUnpaused % 2 == 0)
			{
				twp->btimer += 1;
			}

			if (twp->btimer >= LAVAFALL_TexList.nbTexture)
			{
				twp->btimer = 0;
			}
		}
		
		EntryColliList(twp);
		tp->disp(tp);
	}
}

void __cdecl LavaFall(task* tp)
{
	auto twp = tp->twp;

	switch (static_cast<int>(twp->ang.z) % 4) {
	case 0:
	default:
		twp->value.ptr = &LavaFall0_Action;
		CCL_Init(tp, &LavaFallColS, 1, 4);
		break;
	case 1:
		twp->value.ptr = &LavaFall1_Action;
		CCL_Init(tp, &LavaFallColS, 1, 4);
		break;
	case 2:
		twp->value.ptr = &LavaFall2_Action;
		//CCL_Init(tp, &LavaFallColM, 1, 4);
		twp->ang.x -= 0x4000;
		break;
	case 3:
		twp->value.ptr = &LavaFall3_Action;
		//CCL_Init(tp, &LavaFallColL, 1, 4);;
		twp->ang.x -= 0x4000;
		break;
	}

	if (twp->scl.x == 0)
	{
		twp->scl.x = 1; // default scale
	}

	auto cwp = twp->cwp;
	
	if (cwp)
	{
		auto cclinfo = cwp->info;

		// Adjust collision size with scale
		cclinfo->center.y *= twp->scl.x + (twp->scl.y / 8);
		cclinfo->a *= twp->scl.x;
		cclinfo->b *= twp->scl.x + (twp->scl.y / 8);
	}

	twp->scl.x *= 8.0f; // adjust model scaling

	tp->exec = LavaFallExec;
	tp->disp = LavaFallDisplay;
}

void LavaFall_LoadAssets()
{
	LoadModelFile(&ht_lavafall, "ht_lavafall", ModelFormat_Chunk);

	LoadAnimationFile(&ht_lavafall0_anm, "ht_lavafall0");
	LoadAnimationFile(&ht_lavafall1_anm, "ht_lavafall1");
	LoadAnimationFile(&ht_lavafall2_anm, "ht_lavafall2");
	LoadAnimationFile(&ht_lavafall3_anm, "ht_lavafall3");

	auto object = ht_lavafall->getmodel();

	LavaFall0_Action.object = object;
	LavaFall0_Action.motion = ht_lavafall0_anm->getmotion();

	LavaFall1_Action.object = object;
	LavaFall1_Action.motion = ht_lavafall1_anm->getmotion();

	LavaFall2_Action.object = object;
	LavaFall2_Action.motion = ht_lavafall2_anm->getmotion();

	LavaFall3_Action.object = object;
	LavaFall3_Action.motion = ht_lavafall3_anm->getmotion();
}

void LavaFall_FreeAssets()
{
	FreeFileInfo(&ht_lavafall);
	FreeFileInfo(&ht_lavafall0_anm);
	FreeFileInfo(&ht_lavafall1_anm);
	FreeFileInfo(&ht_lavafall2_anm);
	FreeFileInfo(&ht_lavafall3_anm);
}