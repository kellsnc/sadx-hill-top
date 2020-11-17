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

enum class TranspPlatformActs : Uint16 {
	Input,
	Move,
	Fall
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

inline void LoadTranspPlatform(ObjectMaster* obj, TransporterData1* data, float progress);

inline void MovePlatform(TransporterData1* pdata, TranspPlatformData1* data, float progress) {
	data->PreviousPosition = data->Position; // Store the previous position to move the player later
	data->Position = GetPositionBetweenPoints(&pdata->Position, &pdata->Destination, data->progress); // Move along the vine

	// Get the bending offset, same code as in the parent display
	float bend = data->progress;
	if (bend > 0.5f) bend = fabsf(1.0f - bend);

	// Adjust y position accordingly
	data->Position.y += data->Object->pos[1] - ((pdata->VineZ / 10.0f) * bend);
}

void __cdecl MovePlayerOnPlatform(ObjectMaster* obj, EntityData1* player) {
	// Compares the position from the previous frame to move the player accordingly

	TranspPlatformData1* data = (TranspPlatformData1*)obj->Data1;
	NJS_VECTOR offset = data->Position;
	njSubVector(&offset, &data->PreviousPosition);
	njAddVector(&player->Position, &offset);
}

void __cdecl TranspPlatform_Delete(ObjectMaster* obj) {
	// Removes the dyncol before deleting the object

	if (obj->Data1->LoopData) {
		DynamicCOL_DeleteObject(obj);
		obj->Data1->LoopData = nullptr;
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

inline void LoadTranspPlatform(ObjectMaster* obj, TransporterData1* data, float progress) {
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

void __cdecl EndPoles_Display(ObjectMaster* obj) {
	TransporterData1* pdata = (TransporterData1*)obj->Parent->Data1;
	EntityData1* data = obj->Data1;

	njSetTexture(LevelObjTexlists[1]);
	njPushMatrixEx();
	njTranslateEx(&data->Position);
	njRotateY_(data->Rotation.y);
	njTranslateZ(-2.0f);

	// Draw poles and vines

	njTranslateX(-10);
	njDrawModel_SADX(pdata->PoleObject->basicdxmodel);
	njDrawModel_SADX(pdata->PoleObject->child->basicdxmodel);

	njTranslateX(20);
	njDrawModel_SADX(pdata->PoleObject->basicdxmodel);
	njScaleX(-1.0f); // Mirror the second vine
	njDrawModel_SADX(pdata->PoleObject->child->basicdxmodel);

	njPopMatrixEx();
}

void __cdecl EndPoles_Main(ObjectMaster* obj) {
	AddToCollisionList(obj->Data1);
	obj->DisplaySub(obj);
}

inline void LoadEndPoles(ObjectMaster* obj, TransporterData1* data) {
	ObjectMaster* child = LoadChildObject(LoadObj_Data1, EndPoles_Main, obj);
	
	child->DisplaySub = EndPoles_Display;
	child->Data1->Position = data->Destination;
	child->Data1->Rotation.y += 0x8000;
	Collision_Init(child, arrayptrandlength(HillTransporter_Col), 4);
}

inline void DrawVine(TransporterData1* data) {
	NJS_OBJECT* node = data->PoleObject->child->sibling;

	njPushMatrixEx();

	// Adjust bending
	float bend = data->progress;
	if (bend > 0.5f) bend = fabsf(1.0f - bend);

	// center of vine
	node->child->pos[1] = data->VineY * data->progress - (data->VineZ / 10.0f * bend);
	node->child->pos[2] = data->VineZ * data->progress;

	// end of vine
	node->child->sibling->sibling->pos[1] = data->VineY;
	node->child->sibling->sibling->pos[2] = data->VineZ;
	
	SetupWorldMatrix();
	DrawChunkObject(node); // Draw the vine as chunk model, allows for real time bending

	njPopMatrixEx();
}

inline void DrawPole(TransporterData1* data, float offset, bool invert) {
	njPushMatrixEx();
	njTranslateX(offset);

	njPushMatrixEx();
	njTranslateZ(-2.0f);
	njDrawModel_SADX(data->PoleObject->basicdxmodel); // Pole model

	if (invert) {
		njScaleX(-1.0f); // mirror the second pole vine
	}
	
	njDrawModel_SADX(data->PoleObject->child->basicdxmodel); // Pole vine model
	njPopMatrixEx();
	
	njTranslateY(37.0f);
	DrawVine(data); // Draw the zipline vine
	
	njPopMatrixEx();
}

void __cdecl HillTransporter_Display(ObjectMaster* obj) {
	if (!MissedFrames) {
		TransporterData1* data = (TransporterData1*)obj->Data1;

		njSetTexture(LevelObjTexlists[1]);
		njPushMatrixEx();
		njTranslateEx(&data->Position);
		njRotateY_(data->Rotation.y);
		DrawPole(data, -10, false);
		DrawPole(data, 10, true);
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

	data->PoleObject  = ht_transporter->getmodel()->child->sibling;
	data->PoleObject->child->sibling = ht_vine->getmodel();
	data->PoleObject->child->sibling->child->sibling->pos[2] = 0;

	// Rotate the objet to face the destination
	njLookAt(&data->Position, &data->Destination, nullptr, &data->Rotation.y);

	Collision_Init(obj, arrayptrandlength(HillTransporter_Col), 4);

	obj->DisplaySub	= HillTransporter_Display;
	obj->MainSub	= HillTransporter_Main;

	// Load child objects
	LoadTranspPlatform(obj, data, static_cast<float>(data->Rotation.x % 100) / 100.0f); // Moving platform
	LoadEndPoles(obj, data); // Poles at the destination

	data->SpeedParam = data->Rotation.z;

	if (data->SpeedParam == 0) {
		data->SpeedParam = 100;
	}

	data->Rotation.x = 0;
	data->Rotation.z = 0;
}