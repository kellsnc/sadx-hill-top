#include "pch.h"
#include "o_transporter.h"

/*

Complex object using Ninja Chunk format for bending vines
and some more complex objet structure.

Scale: destination
RotZ: speed

*/

enum class TranspPlatformActs : Uint16 {
	Input,
	Move,
	Fall
};

struct TranspPlatformData1 {
	TranspPlatformActs Action;
	Uint16 timer;
	Float speed;
	Float progress;
	NJS_OBJECT* Object;
	NJS_OBJECT* DynCol;
	Rotation3 ang;
	NJS_VECTOR pos;
	NJS_VECTOR scl;
	CollisionInfo* CollisionInfo;
};

struct TransporterData1 {
	Uint16 Action;
	Uint16 SpeedParam;
	Float VineZ;
	Float VineY;
	NJS_OBJECT* PoleObject;
	Float progress;
	Rotation3 ang;
	NJS_VECTOR pos;
	NJS_VECTOR destination;
	CollisionInfo* CollisionInfo;
};

ModelInfo* ht_transporter = nullptr;
ModelInfo* ht_transportercol = nullptr;
ModelInfo* ht_vine = nullptr;

CCL_INFO HillTransporter_Col[] = {
	{ 0, CI_FORM_CAPSULE, 0x77, 0, 0, {-10.0f, 20.0f, -2.0f}, 2.0f, 20.0f, 0.0f, 0.0f, 0, 0, 0 },
	{ 0, CI_FORM_CAPSULE, 0x77, 0, 0, {10.0f, 20.0f, -2.0f}, 2.0f, 20.0f, 0.0f, 0.0f, 0, 0, 0 }
};

void DrawVine(NJS_OBJECT* vine, NJS_VECTOR* orig, float Y, float Z, float progress)
{
	njPushMatrixEx();
	
	// Adjust bending
	if (progress > 0.0f)
	{
		float bend = progress;
		if (bend > 0.5f) bend = fabsf(1.0f - bend);

		// center of vine
		vine->child->pos[1] = Y * progress - (Z / 10.0f * bend);
		vine->child->pos[2] = Z * progress;
	}
	else
	{
		vine->child->pos[1] = Y * 0.5f;
		vine->child->pos[2] = Z * 0.5f;
	}

	// end of vine
	vine->child->sibling->sibling->pos[1] = Y;
	vine->child->sibling->sibling->pos[2] = Z;

	SetupWorldMatrix();
	DrawChunkObject(vine); // Draw the vine as chunk model, allows for real time bending
	njPopMatrixEx();
}

void DrawPoles(NJS_OBJECT* PoleObject)
{
	njTranslateZ(-2.0f);

	njPushMatrixEx();
	njTranslateX(-10.f);
	dsDrawModel(PoleObject->basicdxmodel); // Pole model
	dsDrawModel(PoleObject->child->basicdxmodel); // Pole vine model
	njPopMatrixEx();

	njPushMatrixEx();
	njTranslateX(10.f);
	dsDrawModel(PoleObject->basicdxmodel); // Pole model
	njScaleX(-1.0f);
	dsDrawModel(PoleObject->child->basicdxmodel); // Pole vine model
	njPopMatrixEx();
}

void __cdecl EndPolesDisplay(task* tp)
{
	if (!MissedFrames)
	{
		auto twp = tp->twp;

		SetSecondObjectTexture();
		njPushMatrixEx();
		njTranslateEx(&twp->pos);
		njRotateY_(twp->ang.y);
		DrawPoles((NJS_OBJECT*)twp->value.ptr);
		njPopMatrixEx();
	}
}

void __cdecl EndPolesExec(task* tp)
{
	EntryColliList(tp->twp);
	tp->disp(tp);
}

void LoadEndPoles(task* tp, NJS_OBJECT* PoleObject, NJS_VECTOR* destination, Angle rot)
{
	auto ctp = CreateChildTask(IM_TWK, EndPolesExec, tp);
	auto ctwp = ctp->twp;

	ctp->disp = EndPolesDisplay;
	ctwp->pos = *destination;
	ctwp->ang.y = rot;
	ctwp->value.ptr = PoleObject;
	CCL_Init(ctp, arrayptrandlength(HillTransporter_Col), 4);
}

void LoadTranspPlatform(task* tp, TransporterData1* twp, float progress);

void MovePlatform(TransporterData1* ptwp, TranspPlatformData1* twp, float progress)
{
	twp->pos = LerpPoints(&ptwp->pos, &ptwp->destination, twp->progress); // Move along the vine

	// Get the bending offset, same code as in the parent display
	float bend = twp->progress;
	if (bend > 0.5f) bend = fabsf(1.0f - bend);

	// Adjust y position accordingly
	twp->pos.y += twp->Object->pos[1] - ((ptwp->VineZ / 10.0f) * bend);
}

void __cdecl TranspPlatformDestroy(task* tp)
{
	auto twp = (TranspPlatformData1*)tp->twp;

	if (twp->DynCol)
	{
		WithdrawCollisionEntry(tp, twp->DynCol);  // Destroy the geometry collision
		ReleaseMobileLandObject(twp->DynCol);     // Release the entry
	}
}

void __cdecl TranspPlatformDisplay(task* tp)
{
	if (!MissedFrames)
	{
		auto ptwp = (TransporterData1*)tp->ptp->twp;
		auto twp = (TranspPlatformData1*)tp->twp;

		SetSecondObjectTexture();
		njPushMatrixEx();
		njTranslateEx(&twp->pos);

		njRotateY_(twp->ang.y);

		njPushMatrixEx();
		{
			if (twp->Action == TranspPlatformActs::Fall)
			{
				njTranslateY(twp->scl.y); // I use this to offset the platform
			}

			dsDrawModel(twp->Object->basicdxmodel); // Platform
			njPopMatrixEx();
		}

		// Draw vines
		njTranslateY(twp->Object->child->pos[1]);
		njTranslateZ(-10.0f);
		dsDrawModel(twp->Object->child->basicdxmodel);
		njTranslateZ(20.0f);
		dsDrawModel(twp->Object->child->basicdxmodel);
		njPopMatrixEx();
	}
}

void __cdecl TranspPlatformExec(task* tp)
{
	auto ptwp = (TransporterData1*)tp->ptp->twp;
	auto twp = (TranspPlatformData1*)tp->twp;

	switch (twp->Action)
	{
	case TranspPlatformActs::Input: // If the player is on the dyncol of our object, launch
		if (IsPlayerOnGeoCol((task*)tp))
		{
			twp->Action = TranspPlatformActs::Move;
			dsPlay_oneshot(466, 0, 0, 160);
		}

		break;
	case TranspPlatformActs::Move:
		dsPlay_timer_v(465, 0, 0, 0, 1, twp->pos.x, twp->pos.y, twp->pos.z);

		twp->progress += 0.001f * (static_cast<float>(ptwp->SpeedParam) / 100.0f); // Speed
		ptwp->progress = twp->progress;	// Hand that to the parent object to bend the vine

		MovePlatform(ptwp, twp, twp->progress);
		MoveGeoCollision(tp, twp->DynCol, &twp->pos);

		// Check if we're at the end, sphere check to detach the object a bit sooner
		if (twp->progress >= 1.0f || CheckCollisionPointSphere(&ptwp->destination, &twp->pos, 25.0f))
		{
			twp->Action = TranspPlatformActs::Fall;
			twp->scl.y = 0.0f;
		}

		break;
	case TranspPlatformActs::Fall:
		twp->timer += 1;

		if (twp->timer == 30)
		{
			TranspPlatformDestroy(tp); // remove dynamic collision
			twp->DynCol = nullptr;
		}
		else if (twp->timer > 30)
		{
			twp->scl.y -= 3.0f; // make platform fall

			// Removes the object after a while, creates a new platform at the beginning
			if (twp->timer > 200)
			{
				LoadTranspPlatform(tp->ptp, ptwp, 0.0f);
				FreeTask(tp);
				return;
			}
		}

		break;
	}

	tp->disp(tp);
}

void LoadTranspPlatform(task* tp, TransporterData1* twp, float progress)
{
	auto ctp = CreateChildTask(IM_TWK, TranspPlatformExec, tp);
	auto ctwp = (TranspPlatformData1*)ctp->twp;

	ctp->disp = TranspPlatformDisplay;
	ctp->dest = TranspPlatformDestroy;
	ctwp->Object = ht_transporter->getmodel()->child;
	ctwp->ang.y += 0x4000;

	ctwp->progress = progress;
	twp->progress = progress;
	MovePlatform(twp, ctwp, twp->progress);

	// Create the dynamic collision
	NJS_OBJECT* object = GetMobileLandObject();

	object->pos[0] = ctwp->pos.x;
	object->pos[1] = ctwp->pos.y;
	object->pos[2] = ctwp->pos.z;

	object->ang[0] = 0;
	object->ang[1] = ctwp->ang.y;
	object->ang[2] = 0;

	object->scl[0] = 1.0f;
	object->scl[1] = 1.0f;
	object->scl[2] = 1.0f;

	object->basicdxmodel = ht_transportercol->getmodel()->basicdxmodel;

	RegisterCollisionEntry(ColFlags_Dynamic | ColFlags_Solid, ctp, object);

	ctwp->DynCol = object;
}

void __cdecl HillTransporterDisplay(task* tp)
{
	if (!MissedFrames)
	{
		auto twp = (TransporterData1*)tp->twp;

		SetSecondObjectTexture();
		njPushMatrixEx();
		njTranslateEx(&twp->pos);
		njRotateY_(twp->ang.y);

		njPushMatrixEx();
		DrawPoles(twp->PoleObject);
		njPopMatrixEx();

		njTranslateY(37.0f);
		njTranslateX(10.0f);
		DrawVine(twp->PoleObject->child->sibling, &twp->pos, twp->VineY, twp->VineZ, twp->progress);
		njTranslateX(-20.0f);
		DrawVine(twp->PoleObject->child->sibling, &twp->pos, twp->VineY, twp->VineZ, twp->progress);
		njPopMatrixEx();
	}
}

void __cdecl HillTransporterExec(task* tp)
{
	if (!CheckRangeOut(tp))
	{
		auto twp = (TransporterData1*)tp->twp;

		LoopTaskC(tp);
		EntryColliList((taskwk*)twp);
		tp->disp(tp);
	}
}

void __cdecl HillTransporter(task* tp)
{
	auto twp = (TransporterData1*)tp->twp;

	// Calculates the direction of the zipline vine
	twp->VineY = twp->destination.y - twp->pos.y;
	twp->VineZ = sqrtf(powf(twp->destination.x - twp->pos.x, 2) + powf(twp->destination.z - twp->pos.z, 2));

	twp->PoleObject = ht_transporter->getmodel()->child->sibling;
	twp->PoleObject->child->sibling = ht_vine->getmodel();
	twp->PoleObject->child->sibling->child->sibling->pos[2] = 0;

	// Rotate the objet to face the destination
	njLookAt(&twp->pos, &twp->destination, nullptr, &twp->ang.y);

	CCL_Init(tp, arrayptrandlength(HillTransporter_Col), 4);

	tp->disp = HillTransporterDisplay;
	tp->exec = HillTransporterExec;

	// Load child objects
	LoadTranspPlatform(tp, twp, static_cast<float>(twp->ang.x % 100) / 100.0f); // Moving platform
	LoadEndPoles(tp, twp->PoleObject, &twp->destination, twp->ang.y + 0x8000); // Poles at the destination

	twp->SpeedParam = twp->ang.z;

	if (twp->SpeedParam == 0)
	{
		twp->SpeedParam = 100;
	}

	twp->ang.x = 0;
	twp->ang.z = 0;
}

void HillTransporter_LoadAssets()
{
	LoadModelFile(&ht_transporter, "ht_transporter", ModelFormat_Basic);
	LoadModelFile(&ht_transportercol, "ht_transportercol", ModelFormat_Basic);
	LoadModelFile(&ht_vine, "ht_vine", ModelFormat_Chunk);
}

void HillTransporter_FreeAssets()
{
	FreeFileInfo(&ht_transporter);
	FreeFileInfo(&ht_transportercol);
	FreeFileInfo(&ht_vine);
}