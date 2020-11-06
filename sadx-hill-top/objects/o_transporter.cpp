#include "pch.h"
#include "o_transporter.h"

ModelInfo* ht_transporter = nullptr;
ModelInfo* ht_vine = nullptr;

CollisionData HillTransporter_Col[] = {
	{ 0, CollisionShape_Capsule2, 0x77, 0, 0, {-15, 5.0f, 0}, 1.0f, 5.0f, 0, 0, 0, 0, 0 },
	{ 0, CollisionShape_Capsule2, 0x77, 0, 0, {15, 5.0f, 0}, 1.0f, 5.0f, 0, 0, 0, 0, 0 }
};

struct TransporterData1 {
	Uint32 Action;
	Angle Yaw;
	Float progress;
	NJS_OBJECT* TranspObject;
	NJS_OBJECT* VineObject;
	Rotation3 Rotation;
	NJS_VECTOR Position;
	NJS_VECTOR Scale;
	CollisionInfo* CollisionInfo;
};

void DrawTransporterVine(TransporterData1* data, Float offset) {
	NJS_OBJECT* node = data->VineObject;

	njPushMatrixEx();
	njTranslateX(offset);
	
	node->child->pos[2] = 0;
	node->child->sibling->pos[2] = 0;
	node->child->sibling->sibling->pos[2] = GetDistance(&data->Position, &data->Scale);
	
	SetupWorldMatrix();
	DrawChunkObject(node);

	njPopMatrixEx();
}

void __cdecl HillTransporter_Display(ObjectMaster* obj) {
	if (!MissedFrames) {
		TransporterData1* data = (TransporterData1*)obj->Data1;

		njSetTexture(LevelObjTexlists[1]);
		njPushMatrixEx();
		njTranslateEx(&data->Position);
		njRotateY_(data->Rotation.y);
		
		njDrawModel_SADX(data->TranspObject->basicdxmodel);

		njTranslateY(50.0f);
		njRotateX_(data->Yaw);

		DrawTransporterVine(data, 15.0f);
		DrawTransporterVine(data, -15.0f);

		njPopMatrixEx();
	}
}

void __cdecl HillTransporter_Main(ObjectMaster* obj) {
	if (!ClipSetObject(obj)) {
		TransporterData1* data = (TransporterData1*)obj->Data1;
		
		AddToCollisionList((EntityData1*)data);
		obj->DisplaySub(obj);
	}
}

void __cdecl HillTransporter(ObjectMaster* obj) {
	TransporterData1* data = (TransporterData1*)obj->Data1;

	data->TranspObject  = ht_transporter->getmodel();
	data->VineObject = ht_vine->getmodel();

	njLookAt(&data->Position, &data->Scale, &data->Yaw, &data->Rotation.y);

	Collision_Init(obj, arrayptrandlength(HillTransporter_Col), 4);

	obj->DisplaySub	= HillTransporter_Display;
	obj->MainSub	= HillTransporter_Main;
}