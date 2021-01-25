#include "pch.h"
#include "o_transporter.h"
#include "o_zipvine.h"

// Spline object

enum class ZipVineActs : Uint16 {
	Wait,
	Run,
	Stop
};

struct TransporterPathData1 {
	ZipVineActs Action;
	Uint16 State;
	Float Progress;
	NJS_OBJECT* Object;
	NJS_OBJECT* VineObject;
	LoopHead* PathData; // "LoopData"
	Rotation3 Rotation;
	NJS_VECTOR Position;
	NJS_VECTOR Scale;
	CollisionInfo* CollisionInfo;
};

CollisionData ZipVine_Col[] = {
	{ 0, CollisionShape_Capsule2, 0x77, 0, 0x430, { 0 }, 2.0f, 40.0f, 0, 0, 0, 0, 0 },
	{ 0, CollisionShape_Capsule2, 0x77, 0, 0x430, { 0 }, 2.0f, 40.0f, 0, 0, 0, 0, 0 },
	{ 1, CollisionShape_Sphere, 0xF0, 0, 0, { 0 }, 8.0f, 0, 0, 0, 0, 0, 0 }
};

void GetVinePoint(NJS_VECTOR* vec, Angle* angle, LoopHead* PathData, int state, float progress) {
	Loop* spoint = &PathData->LoopList[state];
	Loop* epoint = &PathData->LoopList[state + 1];

	*vec = GetPositionBetweenPoints(&spoint->Position, &epoint->Position, progress);

	// Get the bending offset, same code as in the parent display
	float bend = progress;

	if (bend > 0.5f) {
		bend = fabsf(1.0f - bend);
	}

	// Adjust y position accordingly
	vec->y += 7.5f - ((sqrtf(powf(epoint->Position.x - spoint->Position.x, 2) + powf(epoint->Position.z - spoint->Position.z, 2)) / 10.0f) * bend) - 37.0f;
	*angle = spoint->Ang_Y;
}

void DrawZipVine(LoopHead* PathData, NJS_OBJECT* vine, int state, float progress) {
	for (int i = 0; i < PathData->Count - 1; ++i) { // Loop through all vines
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

void DrawZipVinePole(NJS_OBJECT* poles, NJS_VECTOR* pos, Angle rot) {
	njPushMatrixEx();
	njTranslateEx(pos);
	njRotateY_(rot);
	njDrawModel_SADX(poles->basicdxmodel);
	njDrawModel_SADX(poles->child->basicdxmodel);
	njPopMatrixEx();
}

void __cdecl ZipVine_Display(ObjectMaster* obj) {
	if (!MissedFrames) {
		TransporterPathData1* data = (TransporterPathData1*)obj->Data1;
		LoopHead* PathData = data->PathData;

		njSetTexture(LevelObjTexlists[1]);
		
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
		njDrawModel_SADX(data->Object->child->child->basicdxmodel);
		njPopMatrixEx();
	}
}

void __cdecl ZipVine_Main(ObjectMaster* obj) {
	TransporterPathData1* data = (TransporterPathData1*)obj->Data1;

	switch (data->Action) {
	case ZipVineActs::Wait:
		if (IsSpecificPlayerInSphere(&data->Position, 8.0f, 0)) {
			data->Action = ZipVineActs::Run;
			ForcePlayerAction(0, 16);
		}

		break;
	case ZipVineActs::Run:
		data->Progress += 0.005f;

		if (data->Progress >= 1.0f) {
			data->Progress = 0.0f;
			data->State += 1;
		}

		if (data->State == data->PathData->Count - 1) {
			data->Action = ZipVineActs::Stop;
			ForcePlayerAction(0, 24);
			break;
		}

		GetVinePoint(&data->Position, &data->Rotation.y, data->PathData, data->State, data->Progress); // Get new position
		SetPlayerPosition(0, &data->Position); // Update player position
		RotatePlayer(0, data->Rotation.y); // Update player rotation
		
		break;
	}
	
	AddToCollisionList((EntityData1*)data);
	obj->DisplaySub(obj);
}

void __cdecl ZipVine(ObjectMaster* obj) {
	TransporterPathData1* data = (TransporterPathData1*)obj->Data1;
	LoopHead* PathData = data->PathData;

	obj->DisplaySub = ZipVine_Display;
	obj->MainSub = ZipVine_Main;

	data->Object = ht_transporter->getmodel();
	data->VineObject = ht_vine->getmodel();

	Collision_Init(obj, arrayptrandlength(ZipVine_Col), 4);

	// Set start position and end position for poles and their collisions
	data->CollisionInfo->CollisionArray[0].center = PathData->LoopList[0].Position;
	data->CollisionInfo->CollisionArray[0].center.y -= 37.0f;
	data->CollisionInfo->CollisionArray[1].center = PathData->LoopList[PathData->Count - 1].Position;
	data->CollisionInfo->CollisionArray[1].center.y -= 37.0f;

	// Rotate poles to face next/previous point
	njLookAt(&PathData->LoopList[0].Position, &PathData->LoopList[1].Position, nullptr, &data->Rotation.y);
	njLookAt(&PathData->LoopList[PathData->Count - 2].Position, &PathData->LoopList[PathData->Count - 1].Position, nullptr, &data->CollisionInfo->CollisionArray[1].angy);
	
	// Calculate rotations until I'm not lazy to bake them
	for (int i = 0; i < PathData->Count - 1; ++i) {
		Loop* spoint = &PathData->LoopList[i];
		Loop* epoint = &PathData->LoopList[i + 1];

		njLookAt(&spoint->Position, &epoint->Position, nullptr, (Angle*)&spoint->Ang_Y);
	}

	// Start position of the hanging vine
	GetVinePoint(&data->Position, &data->Rotation.y, data->PathData, 0, 0.0f);
}
#pragma endregion