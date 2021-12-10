#include "pch.h"
#include "o_pickrock.h"

/*

A little rock that can be held and pushed.
Example of an holdable & pushable object.

*/

extern NJS_TEXLIST HillTop_TexList;

ModelInfo* ht_pickrock = nullptr;

CollisionData PickRock_Col[] = {
	{ 0, CollisionShape_PushCylinder, 0x7 /* Pushable flag */, 0, 0,{ 0, 2, 0 }, 4.0f, 3.0f, 0.0f },
	{ 0, CollisionShape_Cylinder, 0x70, 0, 0x2400, { 0, 2, 0 }, 6.0f, 4.0f, 0.0f } // For detection, not solid
};

enum PicActions
{
	PicAction_Floor,
	PicAction_Grabbed
};

void PickRock_Delete(ObjectMaster* obj)
{
	SetObjectStatusNotHeld(obj);
}

void __cdecl PickRock_Display(ObjectMaster* obj)
{
	if (!MissedFrames)
	{
		EntityData1* data = obj->Data1;

		njSetTexture(&HillTop_TexList);
		njPushMatrixEx();
		njTranslateEx(&data->Position);
		njRotateEx((Angle*)&data->Rotation, false);
		DrawObject(data->Object);
		njPopMatrixEx();
	}
}

void __cdecl PickRock_Main(ObjectMaster* obj)
{
	if (!ClipSetObject(obj))
	{
		EntityData1* data = obj->Data1;

		switch (data->Action)
		{
		case PicAction_Floor:
			SecondaryObjectPhysics(obj);

			if (data->Status & 0x1000)
			{
				data->Action = PicAction_Grabbed;
			}

			break;
		case PicAction_Grabbed:
			SecondaryObjectPhysics(obj);

			EntityData1* player = GetCollidingEntityA(data);

			if (player)
			{
				data->Rotation = { player->Rotation.x, -player->Rotation.y, player->Rotation.z };
			}

			if (data->Status & 0x1000)
			{
				SetObjectStatusHeld(obj);
			}
			else
			{
				SetObjectStatusNotHeld(obj);
				data->Action = PicAction_Floor;
			}

			break;
		}

		obj->DisplaySub(obj);
	}
}

void __cdecl PickRock(ObjectMaster* obj)
{
	EntityData1* data = obj->Data1;
	EntityData2* data2 = (EntityData2*)obj->Data2;

	data->Object = ht_pickrock->getmodel();

	Collision_Init(obj, arrayptrandlength(PickRock_Col), 4);

	DoStatusThing(data, data2, 4);
	data->Status |= 0x2;

	obj->DeleteSub = PickRock_Delete;
	obj->MainSub = PickRock_Main;
	obj->DisplaySub = PickRock_Display;
}

void PickRock_LoadAssets()
{
	LoadModelFile(&ht_pickrock, "ht_pickrock", ModelFormat_Basic);
}

void PickRocket_FreeAssets()
{
	FreeFileInfo(&ht_pickrock);
}