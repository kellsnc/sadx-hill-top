#include "pch.h"
#include "o_transporter.h"

// Complex object using Ninja Chunk format for bending vines
// And some more complex objet structure

ModelInfo* ht_transporter = nullptr;
ModelInfo* ht_transportercol = nullptr;
ModelInfo* ht_vine = nullptr;

CollisionData HillTransporter_Col[] = {
	{ 0, CollisionShape_Capsule2, 0x77, 0, 0, {-10, 20.0f, -2.0f}, 2.0f, 20.0f, 0, 0, 0, 0, 0 },
	{ 0, CollisionShape_Capsule2, 0x77, 0, 0, {10, 20.0f, -2.0f}, 2.0f, 20.0f, 0, 0, 0, 0, 0 }
};

void DrawVine(NJS_OBJECT* vine, NJS_VECTOR* orig, float Y, float Z, float progress) {
	njPushMatrixEx(); {
		NJS_OBJECT* node = vine;

		// Adjust bending
		if (progress > 0.0f) {
			float bend = progress;
			if (bend > 0.5f) bend = fabsf(1.0f - bend);

			// center of vine
			node->child->pos[1] = Y * progress - (Z / 10.0f * bend);
			node->child->pos[2] = Z * progress;
		}
		else {
			node->child->pos[1] = Y * 0.5f;
			node->child->pos[2] = Z * 0.5f;
		}

		// end of vine
		node->child->sibling->sibling->pos[1] = Y;
		node->child->sibling->sibling->pos[2] = Z;

		SetupWorldMatrix();
		DrawChunkObject(node); // Draw the vine as chunk model, allows for real time bending
		njPopMatrixEx();
	}
}

void DrawPoles(NJS_OBJECT* PoleObject) {
	njTranslateZ(-2.0f);

	njPushMatrixEx(); {
		njTranslateX(-10.f);
		njDrawModel_SADX(PoleObject->basicdxmodel); // Pole model
		njDrawModel_SADX(PoleObject->child->basicdxmodel); // Pole vine model
		njPopMatrixEx();
	}

	njPushMatrixEx(); {
		njTranslateX(10.f);
		njDrawModel_SADX(PoleObject->basicdxmodel); // Pole model
		njScaleX(-1.0f);
		njDrawModel_SADX(PoleObject->child->basicdxmodel); // Pole vine model
		njPopMatrixEx();
	}
}

void __cdecl EndPoles_Display(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;

	njSetTexture(LevelObjTexlists[1]);
	njPushMatrixEx();
	njTranslateEx(&data->Position);
	njRotateY_(data->Rotation.y);
	DrawPoles(data->Object);
	njPopMatrixEx();
}

void __cdecl EndPoles_Main(ObjectMaster* obj) {
	AddToCollisionList(obj->Data1);
	obj->DisplaySub(obj);
}

void LoadEndPoles(ObjectMaster* obj, NJS_OBJECT* PoleObject, NJS_VECTOR* destination, Angle rot) {
	ObjectMaster* child = LoadChildObject(LoadObj_Data1, EndPoles_Main, obj);

	child->DisplaySub = EndPoles_Display;
	child->Data1->Position = *destination;
	child->Data1->Rotation.y = rot;
	child->Data1->Object = PoleObject;
	Collision_Init(child, arrayptrandlength(HillTransporter_Col), 4);
}

#pragma region Transporter Object
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
	Rotation3 Rotation;
	NJS_VECTOR Position;
	NJS_VECTOR PreviousPosition;
	CollisionInfo* CollisionInfo;
};

struct TransporterData1 {
	Uint16 Action;
	Uint16 SpeedParam;
	Float VineZ;
	Float VineY;
	NJS_OBJECT* PoleObject;
	Float progress;
	Rotation3 Rotation;
	NJS_VECTOR Position;
	NJS_VECTOR Destination;
	CollisionInfo* CollisionInfo;
};

void LoadTranspPlatform(ObjectMaster* obj, TransporterData1* data, float progress);

void MovePlatform(TransporterData1* pdata, TranspPlatformData1* data, float progress) {
	data->PreviousPosition = data->Position; // Store the previous position to move the player later
	data->Position = GetPositionBetweenPoints(&pdata->Position, &pdata->Destination, data->progress); // Move along the vine

	// Get the bending offset, same code as in the parent display
	float bend = data->progress;
	if (bend > 0.5f) bend = fabsf(1.0f - bend);

	// Adjust y position accordingly
	data->Position.y += data->Object->pos[1] - ((pdata->VineZ / 10.0f) * bend);
}

void MovePlayerOnPlatform(ObjectMaster* obj, EntityData1* player) {
	// Compares the position from the previous frame to move the player accordingly

	TranspPlatformData1* data = (TranspPlatformData1*)obj->Data1;
	NJS_VECTOR offset = data->Position;
	njSubVector(&offset, &data->PreviousPosition);
	njAddVector(&player->Position, &offset);
}

void __cdecl TranspPlatform_Delete(ObjectMaster* obj) {
	// Removes the dyncol before deleting the object

	if (obj->Data1->LoopData) {
		DynamicCOL_Remove(obj, (NJS_OBJECT*)obj->Data1->LoopData);
		ObjectArray_Remove((NJS_OBJECT*)obj->Data1->LoopData);
	}
}

void __cdecl TranspPlatform_Display(ObjectMaster* obj) {
	TransporterData1* pdata = (TransporterData1*)obj->Parent->Data1;
	TranspPlatformData1* data = (TranspPlatformData1*)obj->Data1;

	njSetTexture(LevelObjTexlists[1]);
	njPushMatrixEx();
	njTranslateEx(&data->Position);

	njRotateY_(data->Rotation.y);

	njPushMatrixEx(); {
		if (data->Action == TranspPlatformActs::Fall) {
			njTranslateY(data->PreviousPosition.y); // I use this to offset the platform
		}

		njDrawModel_SADX(data->Object->basicdxmodel); // Platform
		njPopMatrixEx();
	}

	// Draw vines
	njTranslateY(data->Object->child->pos[1]);
	njTranslateZ(-10.0f);
	njDrawModel_SADX(data->Object->child->basicdxmodel);
	njTranslateZ(20.0f);
	njDrawModel_SADX(data->Object->child->basicdxmodel);
	njPopMatrixEx();
}

void __cdecl TranspPlatform_Main(ObjectMaster* obj) {
	TransporterData1* pdata = (TransporterData1*)obj->Parent->Data1;
	TranspPlatformData1* data = (TranspPlatformData1*)obj->Data1;

	switch (data->Action) {
	case TranspPlatformActs::Input: // If the player is on the dyncol of our object, launch
		if (IsPlayerOnDyncol(obj)) {
			data->Action = TranspPlatformActs::Move;
		}

		break;
	case TranspPlatformActs::Move:
		data->progress += 0.001f * (static_cast<float>(pdata->SpeedParam) / 100.0f); // Speed
		pdata->progress = data->progress;	// Hand that to the parent object to bend the vine

		MovePlatform(pdata, data, data->progress);

		// Update the dyncol position
		data->DynCol->pos[0] = data->Position.x;
		data->DynCol->pos[1] = data->Position.y;
		data->DynCol->pos[2] = data->Position.z;

		// Move the player
		ForEveryPlayerOnDyncol(obj, MovePlayerOnPlatform);

		// Check if we're at the end, sphere check to detach the object a bit sooner
		if (data->progress >= 1.0f || IsPointInsideSphere(&pdata->Destination, &data->Position, 25.0f)) {
			data->Action = TranspPlatformActs::Fall;
			data->PreviousPosition.y = 0;
		}

		break;
	case TranspPlatformActs::Fall:
		data->timer += 1;

		if (data->timer == 30) {
			TranspPlatform_Delete(obj); // remove dynamic collision
		}
		else if (data->timer > 30) {
			data->PreviousPosition.y -= 3.0f;

			// Removes the object after a while, creates a new platform at the beginning
			if (data->timer > 200) {
				LoadTranspPlatform(obj->Parent, pdata, 0.0f);
				DeleteObject_(obj);
				return;
			}
		}

		break;
	}

	obj->DisplaySub(obj);
}

void LoadTranspPlatform(ObjectMaster* obj, TransporterData1* data, float progress) {
	ObjectMaster* child = LoadChildObject(LoadObj_Data1, TranspPlatform_Main, obj);
	TranspPlatformData1* cdata = (TranspPlatformData1*)child->Data1;

	child->DisplaySub = TranspPlatform_Display;
	child->DeleteSub = TranspPlatform_Delete;
	cdata->Object = ht_transporter->getmodel()->child;
	cdata->Rotation.y += 0x4000;	// todo: apply rotations in blender

	cdata->progress = progress;	// Set parameter to position the platform manually
	data->progress = progress;
	MovePlatform(data, cdata, data->progress);

	// Create the dynamic collision
	NJS_OBJECT* object = ObjectArray_GetFreeObject();

	object->pos[0] = child->Data1->Position.x;
	object->pos[1] = child->Data1->Position.y;
	object->pos[2] = child->Data1->Position.z;

	object->ang[0] = 0;
	object->ang[1] = child->Data1->Rotation.y;
	object->ang[2] = 0;

	object->scl[0] = 1.0f;
	object->scl[1] = 1.0f;
	object->scl[2] = 1.0f;

	object->basicdxmodel = ht_transportercol->getmodel()->basicdxmodel;

	DynamicCOL_Add((ColFlags)0x08000001, child, object);

	cdata->DynCol = object;
}

void __cdecl HillTransporter_Display(ObjectMaster* obj) {
	if (!MissedFrames) {
		TransporterData1* data = (TransporterData1*)obj->Data1;

		njSetTexture(LevelObjTexlists[1]);
		njPushMatrixEx();
		njTranslateEx(&data->Position);
		njRotateY_(data->Rotation.y);

		njPushMatrixEx();
		DrawPoles(data->PoleObject);
		njPopMatrixEx();

		njTranslateY(37.0f);
		njTranslateX(10.0f);
		DrawVine(data->PoleObject->child->sibling, &data->Position, data->VineY, data->VineZ, data->progress);
		njTranslateX(-20.0f);
		DrawVine(data->PoleObject->child->sibling, &data->Position, data->VineY, data->VineZ, data->progress);
		njPopMatrixEx();
	}
}

void __cdecl HillTransporter_Main(ObjectMaster* obj) {
	if (!ClipSetObject(obj)) {
		TransporterData1* data = (TransporterData1*)obj->Data1;
		
		RunObjectChildren(obj);
		AddToCollisionList((EntityData1*)data);
		obj->DisplaySub(obj);
	}
}

void __cdecl HillTransporter(ObjectMaster* obj) {
	TransporterData1* data = (TransporterData1*)obj->Data1;

	// Calculates the direction of the zipline vine
	data->VineY = data->Destination.y - data->Position.y;
	data->VineZ = sqrtf(powf(data->Destination.x - data->Position.x, 2) + powf(data->Destination.z - data->Position.z, 2));

	data->PoleObject = ht_transporter->getmodel()->child->sibling;
	data->PoleObject->child->sibling = ht_vine->getmodel();
	data->PoleObject->child->sibling->child->sibling->pos[2] = 0;

	// Rotate the objet to face the destination
	njLookAt(&data->Position, &data->Destination, nullptr, &data->Rotation.y);

	Collision_Init(obj, arrayptrandlength(HillTransporter_Col), 4);

	obj->DisplaySub	= HillTransporter_Display;
	obj->MainSub	= HillTransporter_Main;

	// Load child objects
	LoadTranspPlatform(obj, data, static_cast<float>(data->Rotation.x % 100) / 100.0f); // Moving platform
	LoadEndPoles(obj, data->PoleObject, &data->Destination, data->Rotation.y + 0x8000); // Poles at the destination

	data->SpeedParam = data->Rotation.z;

	if (data->SpeedParam == 0) {
		data->SpeedParam = 100;
	}

	data->Rotation.x = 0;
	data->Rotation.z = 0;
}
#pragma endregion

#pragma region TransporterPath
struct TransporterPathData1 {
	Uint16 Action;
	Uint16 State;
	Float Progress;
	Uint16 test;
	NJS_VECTOR* PreviousPosition;
	LoopHead* PathData;
	Angle RotX;
	Angle RotY;
	Angle RotZ;
	NJS_VECTOR Position;
	NJS_OBJECT* TransporterObject;
	NJS_OBJECT* VineObject;
	NJS_OBJECT* CollisionObject;
	CollisionInfo* CollisionInfo;
};

void DrawTransporter(NJS_OBJECT* transporter, NJS_OBJECT* collision) {
	njPushMatrixEx();
	njTranslateEx((NJS_VECTOR*)collision->pos);
	njRotateY_(collision->ang[1] + 0x4000);
	njDrawModel_SADX(transporter->basicdxmodel);
	njTranslateY(1.5f);
	njTranslateZ(10.0f);
	njDrawModel_SADX(transporter->child->basicdxmodel);
	njTranslateZ(-20.0f);
	njDrawModel_SADX(transporter->child->basicdxmodel);
	njPopMatrixEx();
}

void DrawVines(LoopHead* PathData, NJS_OBJECT* transporter, NJS_OBJECT* vine, int state, float progress) {
	for (int i = 0; i < PathData->Count - 1; ++i) {
		Loop* spoint = &PathData->LoopList[i];
		Loop* epoint = &PathData->LoopList[i + 1];

		float p = 0.0f;

		if (i == state) {
			p = progress;
		}

		float Y = epoint->Position.y - spoint->Position.y;
		float Z = sqrtf(powf(epoint->Position.x - spoint->Position.x, 2) + powf(epoint->Position.z - spoint->Position.z, 2));

		njPushMatrixEx();
		njTranslateEx(&spoint->Position);
		njRotateY_(spoint->Ang_Y);
		njTranslateX(10.0f);
		DrawVine(vine, &spoint->Position, Y, Z, p);
		njTranslateX(-20.0f);
		DrawVine(vine, &spoint->Position, Y, Z, p);
		njPopMatrixEx();
	}
}

void SetUpVineRotations(LoopHead* PathData) {
	for (int i = 0; i < PathData->Count - 1; ++i) {
		Loop* spoint = &PathData->LoopList[i];
		Loop* epoint = &PathData->LoopList[i + 1];

		njLookAt(&spoint->Position, &epoint->Position, nullptr, (Angle*)&spoint->Ang_Y);
	}
}

void MovePlatformPath(NJS_OBJECT* collision, NJS_VECTOR* PeviousPosition, LoopHead* PathData, int state, float progress) {
	NJS_VECTOR* pos = (NJS_VECTOR*)collision->pos;

	*PeviousPosition = *pos;

	Loop* spoint = &PathData->LoopList[state];
	Loop* epoint = &PathData->LoopList[state + 1];

	*pos = GetPositionBetweenPoints(&spoint->Position, &epoint->Position, progress);

	// Get the bending offset, same code as in the parent display
	float bend = progress;

	if (bend > 0.5f) {
		bend = fabsf(1.0f - bend);
	}

	// Adjust y position accordingly
	pos->y +=  13.5f -((sqrtf(powf(epoint->Position.x - spoint->Position.x, 2) + powf(epoint->Position.z - spoint->Position.z, 2)) / 10.0f) * bend) - 37.0f;
	
	collision->ang[1] = spoint->Ang_Y;
}

void MovePlayerOnPlatformPath(ObjectMaster* obj, EntityData1* player) {
	// Compares the position from the previous frame to move the player accordingly

	TransporterPathData1* data = (TransporterPathData1*)obj->Data1;
	NJS_VECTOR offset = { data->CollisionObject->pos[0], data->CollisionObject->pos[1], data->CollisionObject->pos[2] };
	njSubVector(&offset, data->PreviousPosition);
	njAddVector(&player->Position, &offset);
}

void __cdecl HillTransporterPath_Display(ObjectMaster* obj) {
	if (!MissedFrames) {
		TransporterPathData1* data = (TransporterPathData1*)obj->Data1;
		LoopHead* PathData = data->PathData;

		njSetTexture(LevelObjTexlists[1]);
		
		// Draw start poles
		njPushMatrixEx(); {
			njTranslateEx(&data->Position);
			njRotateY_(data->RotY);
			DrawPoles(data->TransporterObject->child->sibling);
			njPopMatrixEx();
		}
		
		DrawVines(data->PathData, data->TransporterObject->child, data->VineObject, data->State, data->Progress);
		DrawTransporter(data->TransporterObject->child, data->CollisionObject);
	}
}

void __cdecl HillTransporterPath_Main(ObjectMaster* obj) {
	TransporterPathData1* data = (TransporterPathData1*)obj->Data1;

	if (data->Action == 0) {
		if (IsPlayerOnDyncol(obj)) {
			data->Action = 1;
		}
	}
	else if (data->Action == 1) {
		data->Progress += 0.005f;

		if (data->Progress >= 1.0f) {
			data->Progress = 0.0f;
			data->State += 1;
		}

		if (data->State == data->PathData->Count - 1) {
			data->Action = 2;
		}
		else {
			MovePlatformPath(data->CollisionObject, data->PreviousPosition, data->PathData, data->State, data->Progress);
			ForEveryPlayerOnDyncol(obj, MovePlayerOnPlatformPath);
		}

		if (IsPlayerInsideSphere_((NJS_VECTOR*)&data->CollisionObject->pos, 500) == 0) {
			data->Action = 0;
			data->State = 0;
			data->Progress = 0.0f;
			MovePlatformPath(data->CollisionObject, data->PreviousPosition, data->PathData, 0, 0.0f);
		}
	}
	
	AddToCollisionList((EntityData1*)data);
	RunObjectChildren(obj);
	obj->DisplaySub(obj);
}

void __cdecl HillTransporterPath_Delete(ObjectMaster* obj) {
	TransporterPathData1* data = (TransporterPathData1*)obj->Data1;

	if (data && data->CollisionObject) {
		DynamicCOL_Remove(obj, data->CollisionObject);
		ObjectArray_Remove(data->CollisionObject);
	}

	delete data->PreviousPosition;
}

void __cdecl HillTransporterPath(ObjectMaster* obj) {
	TransporterPathData1* data = (TransporterPathData1*)obj->Data1;
	LoopHead* PathData = data->PathData;

	obj->DisplaySub = HillTransporterPath_Display;
	obj->MainSub = HillTransporterPath_Main;
	obj->DeleteSub = HillTransporterPath_Delete;

	data->Position = PathData->LoopList[0].Position;
	data->Position.y -= 37.0f;
	data->TransporterObject = ht_transporter->getmodel();
	data->VineObject = ht_vine->getmodel();

	Collision_Init(obj, arrayptrandlength(HillTransporter_Col), 4);

	njLookAt(&data->Position, &PathData->LoopList[1].Position, nullptr, &data->RotY);
	SetUpVineRotations(PathData);

	Angle endrot = 0;
	njLookAt(&PathData->LoopList[PathData->Count - 2].Position, &PathData->LoopList[PathData->Count - 1].Position, nullptr, &endrot);
	
	NJS_VECTOR dest = PathData->LoopList[PathData->Count - 1].Position;
	dest.y -= 37.0f;
	
	LoadEndPoles(obj, data->TransporterObject->child->sibling, &dest, endrot);

	// Create the dynamic collision
	NJS_OBJECT* object = ObjectArray_GetFreeObject();
	object->ang[0] = 0;
	object->ang[2] = 0;
	object->scl[0] = 1.0f;
	object->scl[1] = 1.0f;
	object->scl[2] = 1.0f;
	object->basicdxmodel = ht_transportercol->getmodel()->basicdxmodel;
	DynamicCOL_Add((ColFlags)0x08000001, obj, object);
	data->CollisionObject = object;

	data->PreviousPosition = new NJS_VECTOR;

	MovePlatformPath(data->CollisionObject, data->PreviousPosition, data->PathData, 0, 0.0f);
}
#pragma endregion