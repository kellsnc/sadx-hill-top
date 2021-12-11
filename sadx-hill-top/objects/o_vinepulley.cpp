#include "pch.h"
#include "o_vinepulley.h"
#include "o_lantern.h"

/*

Vine that acts like a pulley.

Position: where the vine starts
Scale X: distance to where the player grabs the vine
Scale Y: distance to where the vine stops

*/

extern ModelInfo* ht_transporter;

CollisionData VinePulley_Col = {
	0, CI_FORM_SPHERE, 0xF0, 0, 0, {0.0f, 0.0f, 0.0f}, 8.0f, 0.0f, 0.0f, 0, 0, 0, 0
};

void __cdecl VinePulleyTarget(ObjectMaster* obj)
{
	EntityData1* data = obj->Data1;
	RunObjectChildren(obj);
	AddToCollisionList(data);
}

void __cdecl VinePulley_Display(ObjectMaster* obj)
{
	if (!MissedFrames)
	{
		EntityData1* data = obj->Data1;
		EntityData1* child = obj->Child->Data1;

		SetSecondObjectTexture();
		njPushMatrixEx();
		njTranslateEx(&child->Position);
		njScaleY(data->Scale.z / 23);
		DrawModel(data->Object->basicdxmodel);
		njPopMatrixEx();
	}
}

void __cdecl VinePulley_Main(ObjectMaster* obj)
{
	if (!ClipSetObject(obj))
	{
		EntityData1* data = obj->Data1;
		EntityData1* child = obj->Child->Data1;

		if (data->Action == 0)
		{
			if (data->Scale.z < data->Scale.x)
			{
				data->Scale.z += 5.0f;
				child->Position.y = data->Position.y - data->Scale.z;
			}

			if (data->InvulnerableTime > 0)
			{
				--data->InvulnerableTime;
			}
			else
			{
				EntityData1* player = GetCollidingEntityA(child);

				if (player)
				{
					data->CharIndex = player->CharIndex;
					ForcePlayerAction(0, 16);
					dsPlay_oneshot(463, 0, 0, 0);
					data->Action = 1;
				}
			}
		}
		else
		{
			if (data->Scale.z > data->Scale.y)
			{
				data->Scale.z -= 5.0f;
				child->Position.y = data->Position.y - data->Scale.z;
			}

			if (CheckJump(data->CharIndex))
			{
				data->Action = 0;
				data->InvulnerableTime = 100;
			}

			ForcePlayerPos(data->CharIndex, &child->Position);
			EntityData1Ptrs[data->CharIndex]->Rotation.y = data->Rotation.y;
		}

		obj->DisplaySub(obj);
	}

	RunObjectChildren(obj);
}

void __cdecl VinePulley(ObjectMaster* obj)
{
	EntityData1* data = obj->Data1;

	data->Object = ht_transporter->getmodel()->child->child;

	ObjectMaster* child = LoadChildObject(LoadObj_Data1, VinePulleyTarget, obj);
	child->Data1->Position.y = data->Position.y - data->Scale.x;
	Collision_Init(child, &VinePulley_Col, 1, 3);
	FireFly_Load((task*)child, 3);

	data->Scale.z = data->Scale.x;

	obj->MainSub = VinePulley_Main;
	obj->DisplaySub = VinePulley_Display;
}