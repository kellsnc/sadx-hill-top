#include "pch.h"
#include "o_transporter.h"

ModelInfo* ht_transporter = nullptr;
ModelInfo* ht_transportercol = nullptr;
ModelInfo* ht_vine = nullptr;

CollisionData HillTransporter_Col[] = {
	{ 0, CollisionShape_Capsule2, 0x77, 0, 0, {-10, 20.0f, -4.0f}, 3.0f, 20.0f, 0, 0, 0, 0, 0 },
	{ 0, CollisionShape_Capsule2, 0x77, 0, 0, {10, 20.0f, -4.0f}, 3.0f, 20.0f, 0, 0, 0, 0, 0 }
};

enum class TranspPlatformActs : Uint16 {
	Input,
	Move,
	Fall
};

struct TransporterData1 {
	Uint32 Action;
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

void __cdecl MovePlayerOnPlatform(ObjectMaster* obj, EntityData1* player) {
	TranspPlatformData1* data = (TranspPlatformData1*)obj->Data1;
	NJS_VECTOR offset = data->Position;
	njSubVector(&offset, &data->PreviousPosition);
	njAddVector(&player->Position, &offset);
}

void __cdecl TranspPlatform_Delete(ObjectMaster* obj) {
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
			njTranslateY(data->PreviousPosition.y);
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
	float bend = 0;

	switch (data->Action) {
	case TranspPlatformActs::Input:
		if (IsPlayerOnDyncol(obj)) {
			data->Action = TranspPlatformActs::Move;
		}

		break;
	case TranspPlatformActs::Move:
		data->progress += 0.001f;
		pdata->progress = data->progress;

		data->PreviousPosition = data->Position;
		data->Position = GetPositionBetweenPoints(&pdata->Position, &pdata->Destination, data->progress);
		
		bend = data->progress;
		if (bend > 0.5f) bend = fabsf(1.0f - bend);

		data->Position.y += data->Object->pos[1] - ((pdata->VineZ / 10.0f) * bend);

		data->DynCol->pos[0] = data->Position.x;
		data->DynCol->pos[1] = data->Position.y;
		data->DynCol->pos[2] = data->Position.z;

		ForEveryPlayerOnDyncol(obj, MovePlayerOnPlatform);

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
	cdata->progress = progress;
	cdata->Object = ht_transporter->getmodel()->child;
	cdata->Position.y += child->Data1->Object->pos[1];
	cdata->Rotation.y += 0x4000;
	cdata->PreviousPosition = cdata->Position;

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
	data->progress = progress;
}

void __cdecl EndPoles_Display(ObjectMaster* obj) {
	TransporterData1* pdata = (TransporterData1*)obj->Parent->Data1;
	EntityData1* data = obj->Data1;

	njSetTexture(LevelObjTexlists[1]);
	njPushMatrixEx();
	njTranslateEx(&data->Position);
	njRotateY_(data->Rotation.y);
	njTranslateZ(-4.0f);

	njTranslateX(-10);
	njDrawModel_SADX(pdata->PoleObject->basicdxmodel);
	njDrawModel_SADX(pdata->PoleObject->child->basicdxmodel);

	njTranslateX(20);
	njDrawModel_SADX(pdata->PoleObject->basicdxmodel);
	njScaleX(-1.0f);
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

	float bend = data->progress;
	if (bend > 0.5f) bend = fabsf(1.0f - bend);

	node->child->pos[1] = data->VineY * data->progress - (data->VineZ / 10.0f * bend);
	node->child->pos[2] = data->VineZ * data->progress;

	node->child->sibling->sibling->pos[1] = data->VineY;
	node->child->sibling->sibling->pos[2] = data->VineZ;
	
	SetupWorldMatrix();
	DrawChunkObject(node);

	njPopMatrixEx();
}

inline void DrawPole(TransporterData1* data, float offset, bool invert) {
	njPushMatrixEx();
	njTranslateX(offset);

	njPushMatrixEx();
	njTranslateZ(-4.0f);
	njDrawModel_SADX(data->PoleObject->basicdxmodel);

	if (invert) {
		njScaleX(-1.0f);
	}
	
	njDrawModel_SADX(data->PoleObject->child->basicdxmodel);
	njPopMatrixEx();
	
	njTranslateY(37.0f);
	DrawVine(data);
	
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

	data->VineY = data->Destination.y - data->Position.y;
	data->VineZ = sqrtf(powf(data->Destination.x - data->Position.x, 2) + powf(data->Destination.z - data->Position.z, 2));

	data->PoleObject  = ht_transporter->getmodel()->child->sibling;
	data->PoleObject->child->sibling = ht_vine->getmodel();
	data->PoleObject->child->sibling->child->sibling->pos[2] = 0;

	njLookAt(&data->Position, &data->Destination, nullptr, &data->Rotation.y);

	Collision_Init(obj, arrayptrandlength(HillTransporter_Col), 4);

	obj->DisplaySub	= HillTransporter_Display;
	obj->MainSub	= HillTransporter_Main;

	LoadTranspPlatform(obj, data, static_cast<float>(data->Rotation.x % 100) / 100.0f);
	LoadEndPoles(obj, data);

	data->Rotation.x = 0;
	data->Rotation.z = 0;
}