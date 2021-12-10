#include "pch.h"
#include "o_transporter.h"
#include "o_zipvine.h"

/*

Long vines that acts like a zipline.

Path object, data->LoopData contains the path information.
See paths.cpp for implementation.

*/

enum class ZipVineActs : Uint8 {
	Wait,
	Run,
	Stop
};

struct TransporterPathData1 {
	ZipVineActs Action;
	Uint8 PlayerDetached;
	Uint8 State;
	Uint8 Timer;
	Float Progress;
	NJS_OBJECT* Object;
	NJS_OBJECT* VineObject;
	LoopHead* PathData;
	Rotation3 Rotation;
	NJS_VECTOR Position;
	NJS_VECTOR Scale;
	CollisionInfo* CollisionInfo;
};

CollisionData ZipVine_Col[] = {
	{ 0, CI_FORM_CAPSULE, 0x77, 0, 0x430, { 0 }, 2.0f, 40.0f, 0, 0, 0, 0, 0 },
	{ 0, CI_FORM_CAPSULE, 0x77, 0, 0x430, { 0 }, 2.0f, 40.0f, 0, 0, 0, 0, 0 },
	{ 1, CI_FORM_SPHERE, 0xF0, 0, 0, { 0 }, 14.0f, 0, 0, 0, 0, 0, 0 }
};

void GetVinePoint(NJS_VECTOR* vec, Angle* angle, LoopHead* PathData, int state, float progress)
{
	Loop* spoint = &PathData->LoopList[state];
	Loop* epoint = &PathData->LoopList[state + 1];

	*vec = GetPositionBetweenPoints(&spoint->Position, &epoint->Position, progress);

	// Get the bending offset, same code as in the parent display
	float bend = progress;

	if (bend > 0.5f)
	{
		bend = fabsf(1.0f - bend);
	}

	// Adjust y position accordingly
	vec->y += 7.5f - ((sqrtf(powf(epoint->Position.x - spoint->Position.x, 2) + powf(epoint->Position.z - spoint->Position.z, 2)) / 10.0f) * bend) - 37.0f;
	*angle = spoint->Ang_Y;
}

void DrawZipVine(LoopHead* PathData, NJS_OBJECT* vine, int state, float progress)
{
	for (int i = 0; i < PathData->Count - 1; ++i)
	{ // Loop through all vines
		Loop* spoint = &PathData->LoopList[i];
		Loop* epoint = &PathData->LoopList[i + 1];

		float p = i == state ? progress : 0.0f; // Bend if the player is on the current vine
		float Y = epoint->Position.y - spoint->Position.y; // Vine height
		float Z = sqrtf(powf(epoint->Position.x - spoint->Position.x, 2) + powf(epoint->Position.z - spoint->Position.z, 2)); // Vine length

		njPushMatrixEx();
		njTranslateEx(&spoint->Position);
		njRotateY_(spoint->Ang_Y);
		DrawVine(vine, &spoint->Position, Y, Z, p);
		njPopMatrixEx();
	}
}

void DrawZipVinePole(NJS_OBJECT* poles, NJS_VECTOR* pos, Angle rot)
{
	njPushMatrixEx();
	njTranslateEx(pos);
	njRotateY_(rot);
	DrawModel(poles->basicdxmodel);
	DrawModel(poles->child->basicdxmodel);
	njPopMatrixEx();
}

void __cdecl ZipVine_Display(ObjectMaster* obj)
{
	if (!MissedFrames)
	{
		TransporterPathData1* data = (TransporterPathData1*)obj->Data1;
		LoopHead* PathData = data->PathData;

		njSetTexture(&HillTopOBJ_TexList);

		// Draw start pole
		DrawZipVinePole(data->Object->child->sibling, &data->CollisionInfo->CollisionArray[0].center, data->CollisionInfo->CollisionArray[0].angy);

		// Draw end pole
		DrawZipVinePole(data->Object->child->sibling, &data->CollisionInfo->CollisionArray[1].center, data->CollisionInfo->CollisionArray[1].angy);

		// Draw vine line
		DrawZipVine(data->PathData, data->VineObject, data->State, data->Progress);

		// Draw hanging vine
		njPushMatrixEx();
		njTranslateEx(&data->Position);
		njRotateY_(data->Rotation.y);
		njRotateZ_(data->Rotation.z);
		njTranslateY(7.5f);
		DrawModel(data->Object->child->child->basicdxmodel);
		njPopMatrixEx();
	}
}

void __cdecl ZipVine_Main(ObjectMaster* obj)
{
	TransporterPathData1* data = (TransporterPathData1*)obj->Data1;

	if (obj->UnknownB_ptr != (void*)CurrentAct)
	{
		DeleteObject_(obj);
	}

	switch (data->Action)
	{
	case ZipVineActs::Wait:
		if (IsSpecificPlayerInSphere(&data->Position, 14.0f, 0))
		{
			data->Action = ZipVineActs::Run;
			ForcePlayerAction(0, 16);
		}

		break;
	case ZipVineActs::Run:
		data->Progress += (data->PathData->TotalDist / data->PathData->LoopList[data->State].Dist) / data->PathData->TotalDist * 4.0f;

		if (data->Progress >= 1.0f)
		{
			data->Progress = 0.0f;
			data->State += 1;
		}

		if (data->State == data->PathData->Count - 1 || (data->State == data->PathData->Count - 2 && data->Progress > 0.95f))
		{
			data->Action = ZipVineActs::Stop;
			ForcePlayerAction(0, 24);
			break;
		}

		GetVinePoint(&data->Position, &data->Rotation.y, data->PathData, data->State, data->Progress); // Get new position

		if (data->PlayerDetached == false)
		{
			SetPlayerPosition(0, &data->Position); // Update player position
			RotatePlayer(0, data->Rotation.y); // Update player rotation

			if (FrameCounterUnpaused % 300 == 0)
			{
				PlaySound3D(456, nullptr, 0, 200, 120, EntityData1Ptrs[0]);
			}

			if ((data->State > 0 || data->Progress > 0.3f) && CheckJump(0))
			{
				data->PlayerDetached = true;
			}
		}

		break;
	case ZipVineActs::Stop:
		if (++data->Timer > 100)
		{
			data->Progress = 0.01f;
			data->State = 0;
			GetVinePoint(&data->Position, &data->Rotation.y, data->PathData, 0, data->Progress);
			data->Action = ZipVineActs::Wait;
			data->Timer = 0;
			data->PlayerDetached = false;
		}

		break;
	}

	AddToCollisionList((EntityData1*)data);
	obj->DisplaySub(obj);
}

void __cdecl ZipVine(ObjectMaster* obj)
{
	TransporterPathData1* data = (TransporterPathData1*)obj->Data1;
	LoopHead* PathData = data->PathData;

	obj->DisplaySub = ZipVine_Display;
	obj->MainSub = ZipVine_Main;

	data->Object = ht_transporter->getmodel();
	data->VineObject = ht_vine->getmodel();

	Collision_Init(obj, arrayptrandlength(ZipVine_Col), 3);

	// Set start position and end position for poles and their collisions
	data->CollisionInfo->CollisionArray[0].center = PathData->LoopList[0].Position;
	data->CollisionInfo->CollisionArray[0].center.y -= 37.0f;
	data->CollisionInfo->CollisionArray[1].center = PathData->LoopList[PathData->Count - 1].Position;
	data->CollisionInfo->CollisionArray[1].center.y -= 37.0f;

	// Rotate poles to face next/previous point
	njLookAt(&PathData->LoopList[0].Position, &PathData->LoopList[1].Position, nullptr, &data->Rotation.y);
	njLookAt(&PathData->LoopList[PathData->Count - 2].Position, &PathData->LoopList[PathData->Count - 1].Position, nullptr, &data->CollisionInfo->CollisionArray[1].angy);

	// Calculate rotations until I'm not lazy to bake them
	for (int i = 0; i < PathData->Count - 1; ++i)
	{
		Loop* spoint = &PathData->LoopList[i];
		Loop* epoint = &PathData->LoopList[i + 1];

		njLookAt(&spoint->Position, &epoint->Position, nullptr, (Angle*)&spoint->Ang_Y);
	}

	// Start position of the hanging vine
	data->Progress = 0.01f;
	GetVinePoint(&data->Position, &data->Rotation.y, data->PathData, 0, data->Progress);

	obj->UnknownB_ptr = (void*)CurrentAct;
}
#pragma endregion