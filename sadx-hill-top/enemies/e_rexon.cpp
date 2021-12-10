#include "pch.h"
#include "e_rexon.h"
#include "e_fireball.h"
#include "..\objects\o_growlava.h"

/*

REXON ENEMY

RotX = lava ID
RotZ = Emerald ID
ScaleX = Size;
ScaleY = Move Radius (stand if 0);

*/

NJS_TEXNAME REXON_TexNames[12];
NJS_TEXLIST REXON_TexList = { arrayptrandlength(REXON_TexNames) };

ModelInfo* e_rexon = nullptr;
ModelInfo* e_rexoncol = nullptr;

CollisionData Rexon_Col[] =
{
	{ 0, CollisionShape_Sphere, 0x10, 0x21, 0x400, { 0.0, 0.0f, 0.0 }, 2.0f, 0.0f, 0.0f, 0, 0, 0, 0 },
	{ 0, CollisionShape_Sphere, 0x10, 0x21, 0x400, { 0.0, 0.0f, 0.0 }, 2.0f, 0.0f, 0.0f, 0, 0, 0, 0 },
	{ 0, CollisionShape_Sphere, 0x10, 0x21, 0x400, { 0.0, 0.0f, 0.0 }, 2.0f, 0.0f, 0.0f, 0, 0, 0, 0 }
};

CollisionData RexonHead_Col = { 0, CollisionShape_Sphere, 0x10, 0x21, 0x400, { 0, 3.0f, 1.0f }, 3.0f, 0.0f, 0.0f, 0, 0, 0, 0 };

enum RexonHeadActs
{
	RexonHeadAct_Normal,
	RexonHeadAct_Attacking
};

enum RexonActs
{
	RexonAct_Stand,
	RexonAct_Move,
	RexonAct_Dead
};

#pragma region RexonHead

void RexonHead_LoadFireBall(ObjectMaster* obj, EntityData1* data)
{
	NJS_VECTOR pos =
	{ 0 };
	Angle roty = 0;
	Angle rotx = 0;

	njPushMatrix(_nj_unit_matrix_);
	njTranslateEx(&data->Position);
	njRotateY_(obj->Parent->Data1->Rotation.y);
	njTranslate(nullptr, 0, 3.0f, 1.0f);
	njGetTranslation(nullptr, &pos);
	njPopMatrixEx();

	njLookAt(&pos, &EntityData1Ptrs[GetClosestPlayerID(&data->Position)]->Position, &rotx, &roty);

	LoadFireBall(obj, &pos, roty, rotx, 1.5f, 1.5f, 0.01f, 0);

	PlaySound3D(464, nullptr, 0, 60, 120, data);
}

void RexonHead_DrawMouth(EntityData1* data, NJS_OBJECT* object, float mouthopen)
{
	njPushMatrixEx();
	njTranslate(nullptr, Pos3(object->pos));
	njRotateXYZ(nullptr, Pos3(object->ang));
	njRotateX(0, static_cast<Angle>(mouthopen));
	DrawModel(object->basicdxmodel);
	njPopMatrixEx();

	DrawObjectRoot(object->sibling);
}

bool RexonHead_IsInAttackRange(NJS_VECTOR* position, Angle roty, Float radius)
{
	NJS_VECTOR pos = { 0 };

	// Calculate point in front of the head
	njPushMatrix(_nj_unit_matrix_);
	njTranslateEx(position);
	njRotateY_(roty);
	njTranslateZ(50.0f);
	njGetTranslation(0, &pos);
	njPopMatrixEx();

	return IsPlayerInsideSphere_(&pos, radius);
}

void __cdecl RexonHead_Display(ObjectMaster* obj)
{
	if (!MissedFrames)
	{
		EntityData1* data = obj->Data1;
		EntityData1* pdata = obj->Parent->Data1;

		njSetTexture(&REXON_TexList);
		njPushMatrixEx();
		njTranslateEx(&data->Position);
		njRotateY_(pdata->Rotation.y);
		njScalef(pdata->Scale.x);

		DrawModel(data->Object->basicdxmodel);

		RexonHead_DrawMouth(data, data->Object->child, data->Scale.x);

		njPopMatrixEx();
	}
}

void __cdecl RexonHead_Main(ObjectMaster* obj)
{
	EntityData1* data = obj->Data1;
	EntityData1* pdata = obj->Parent->Data1;

	switch (data->Action)
	{
	case RexonHeadAct_Normal:
		if (data->InvulnerableTime == 0 && RexonHead_IsInAttackRange(&data->Position, pdata->Rotation.y, 50.0f) && (rand() % 30) == 0)
		{
			data->InvulnerableTime = 80 + (rand() % 50);
			data->field_A = 1;
			data->Action = RexonHeadAct_Attacking;
		}

		if (data->InvulnerableTime > 0)
		{
			--data->InvulnerableTime;
		}
		else
		{
			data->InvulnerableTime = 0;
		}

		break;
	case RexonHeadAct_Attacking:
		++data->field_A;
		data->Scale.x = -(0x2000 * fmin(0.0f, njSin(static_cast<int>(data->field_A * 1000))));

		if (data->field_A == 0x25)
		{
			RexonHead_LoadFireBall(obj, data);
		}

		if (data->field_A > 0x40 && data->Scale.x == 0)
		{
			data->Action = RexonHeadAct_Normal;
			data->field_A = 1;
		}

		break;
	}

	if (OhNoImDead(data, (ObjectData2*)obj->Data2))
	{
		LoadEnemyExplosion(data->Position.x, data->Position.y + 3.0f, data->Position.z, 1.0f);
		SpawnAnimal(3, data->Position.x, data->Position.y + 6.0f, data->Position.z);
		SetEmeraldObtained(data->Rotation.z, &data->Position);
		DeleteObject_(obj);
		DeleteGammaMissileIfNoTarget(obj);

		obj->Parent->SETData.SETData->Flags |= 0x1000;

		return;
	}

	Object_CheckEmerald(data->Rotation.z, &data->Position);
	RunObjectChildren(obj);
	AddToCollisionList(data);
	obj->DisplaySub(obj);
}

void __cdecl RexonHead(ObjectMaster* obj)
{
	EntityData1* data = obj->Data1;
	EntityData1* pdata = obj->Parent->Data1;
	enemywk* enmwk = (enemywk*)AllocateObjectData2(obj, (EntityData1*)data);

	Collision_Init(obj, &RexonHead_Col, 1, 3);

	data->Object = pdata->Object->child->sibling->sibling->sibling->sibling->child->child->child->child;
	data->Scale.x = 0; // mouth opening
	data->InvulnerableTime = 0;

	enmwk->bound_ceiling = 0;
	enmwk->bound_floor = 0;
	enmwk->bound_friction = 0;
	enmwk->bound_side = 0;
	enmwk->colli_top = 0;
	enmwk->colli_bottom = 0;
	enmwk->colli_center = { 0, 0, 0 };
	enmwk->colli_radius = 1.0f;
	enmwk->home = data->Position;

	data->Rotation.z = pdata->Rotation.z;
	Object_CheckEmerald(data->Rotation.z, &data->Position);

	obj->DeleteSub = Enemy_Delete;
	obj->MainSub = RexonHead_Main;
	obj->DisplaySub = RexonHead_Display;
}

#pragma endregion

#pragma region Rexon

inline float Rexon_NeckMovement(int i, int action, float timer)
{
	float move = 0;

	if (i > 0)
	{
		if (action == RexonAct_Stand)
		{
			move = 0.25f * njSin(static_cast<int>(timer * 100));
		}
		else
		{
			move = 0.35f * njSin(static_cast<int>(timer * 500));
		}
	}

	if (i == 2)
	{
		move = -move;
	}

	return move;
}

void Rexon_GetHeadPos(EntityData1* data, NJS_VECTOR* headpos)
{
	NJS_OBJECT* object = data->Object->child->sibling->sibling->sibling->sibling;

	njPushMatrix(_nj_unit_matrix_);
	njRotateY_(data->Rotation.y);
	njScalef(data->Scale.x);

	for (int i = 0; i < 4; ++i)
	{
		njTranslate(nullptr, Pos3(object->pos));
		njRotateXYZ(nullptr, Pos3(object->ang));

		if (i > 0)
		{
			njPushMatrixEx();
			njTranslateZ(Rexon_NeckMovement(i, data->Action, data->InvulnerableTime));

			// get the head position
			if (i == 3)
			{
				njGetTranslation(nullptr, headpos);
				njAddVector(headpos, &data->Position);
			}

			njPopMatrixEx();
		}

		object = object->child;
	}

	njPopMatrixEx();
}

void Rexon_MoveColliNeck(EntityData1* data)
{
	NJS_OBJECT* object = data->Object->child->sibling->sibling->sibling->sibling;

	njPushMatrix(_nj_unit_matrix_);
	njScalef(data->Scale.x);

	for (int i = 0; i < 4; ++i)
	{
		njTranslate(nullptr, Pos3(object->pos));
		njRotateXYZ(nullptr, Pos3(object->ang));

		if (i > 0)
		{
			njPushMatrixEx();
			njTranslateZ(Rexon_NeckMovement(i, data->Action, data->InvulnerableTime));
			njGetTranslation(nullptr, &data->CollisionInfo->CollisionArray[i - 1].center);
			njPopMatrixEx();
		}

		object = object->child;
	}

	njPopMatrixEx();
}

void Rexon_MoveDynCol(ObjectMaster* obj, EntityData1* data)
{
	// Move up and down, like floating
	data->field_A += 200;
	data->Position.y = data->Scale.z + 1.0f + (1.0f * njSin(data->field_A));

	// Move the dyncol
	NJS_OBJECT* dyncol = (NJS_OBJECT*)data->LoopData;

	NJS_VECTOR mov = data->Position;
	njSubVector(&mov, (NJS_VECTOR*)dyncol->pos);

	dyncol->pos[0] = data->Position.x;
	dyncol->pos[1] = data->Position.y;
	dyncol->pos[2] = data->Position.z;
	dyncol->ang[1] = data->Rotation.y;

	for (int i = 0; i < MaxPlayers; ++i)
	{
		CharObj2* co2 = CharObj2Ptrs[i];

		if (co2 && co2->DynColObject == obj)
		{
			EntityData1* player = EntityData1Ptrs[i];

			njAddVector(&player->Position, &mov);
		}
	}
}

void Rexon_DrawFins(EntityData1* data, NJS_OBJECT* object, float timer)
{
	for (int i = 0; i < 4; ++i)
	{
		int rot = 0;

		if (data->Action == RexonAct_Stand)
		{
			rot = 0x2000 * njSin(static_cast<int>(timer * 100));
		}
		else
		{
			rot = 0x3000 * njSin(static_cast<int>(timer * 500));
		}

		njPushMatrixEx();
		njTranslate(nullptr, Pos3(object->pos));
		njRotateXYZ(nullptr, Pos3(object->ang));
		njRotateY_(rot);
		njRotateX_(rot / 4);
		njScale(nullptr, Pos3(object->scl));
		DrawModel(object->basicdxmodel);
		njPopMatrixEx();

		object = object->sibling;
	}
}

void Rexon_DrawNeck(EntityData1* data, NJS_OBJECT* object, float timer)
{
	njPushMatrixEx();

	for (int i = 0; i < 4; ++i)
	{
		njTranslate(nullptr, Pos3(object->pos));
		njRotateXYZ(nullptr, Pos3(object->ang));

		njPushMatrixEx();
		njTranslateZ(Rexon_NeckMovement(i, data->Action, timer));
		DrawModel(object->basicdxmodel);
		njPopMatrixEx();

		object = object->child;
	}

	njPopMatrixEx();
}

void __cdecl Rexon_Display(ObjectMaster* obj)
{
	if (!MissedFrames)
	{
		EntityData1* data = obj->Data1;

		njSetTexture(&REXON_TexList);
		njPushMatrixEx();
		njTranslateEx(&data->Position);
		njRotateY_(data->Rotation.y);
		njScalef(data->Scale.x);

		DrawModel(data->Object->basicdxmodel);

		if (data->Action != RexonAct_Dead)
		{
			Rexon_DrawFins(data, data->Object->child, data->InvulnerableTime);
			Rexon_DrawNeck(data, data->Object->child->sibling->sibling->sibling->sibling, data->InvulnerableTime);
		}

		njPopMatrixEx();
	}
}

void __cdecl Rexon_Main(ObjectMaster* obj)
{
	if (!ClipSetObject(obj))
	{
		EntityData1* data = obj->Data1;

		// Handle growing lava in act 2
		int lava_id = data->Rotation.x;
		if (CurrentAct == 1 && grow_workers[lava_id].Enabled == true)
		{
			if (data->Scale.z < grow_workers[lava_id].Height)
			{
				data->Scale.z = grow_workers[lava_id].Height;
			}
		}

		if (data->Action != RexonAct_Dead)
		{
			data->InvulnerableTime += 1;

			// If the rexon is a moving one
			if (data->Action == RexonAct_Move)
			{
				// Turn around if outside of range
				if (IsPointInsideSphere(&obj->SETData.SETData->SETEntry->Position, &data->Position, data->Scale.y) == false)
				{
					njLookAt(&data->Position, &obj->SETData.SETData->SETEntry->Position, nullptr, &data->Rotation.y);
				}

				// Move forward
				njPushMatrix(_nj_unit_matrix_);
				njTranslateEx(&data->Position);
				njRotateY(0, data->Rotation.y);
				njTranslateZ(data->Scale.y / 1000);
				njGetTranslation(0, &data->Position);
				njPopMatrixEx();
			}

			if (obj->Child)
			{
				Rexon_MoveColliNeck(data);
				Rexon_GetHeadPos(data, &obj->Child->Data1->Position);
				AddToCollisionList(data);
			}
			else
			{
				data->Action = RexonAct_Dead;
				Collision_Free(obj);
			}

			RunObjectChildren(obj);
		}

		Rexon_MoveDynCol(obj, data);
		obj->DisplaySub(obj);
	}
}

void __cdecl Rexon_Delete(ObjectMaster* obj)
{
	// Removes the dyncol before deleting the object

	if (obj->Data1->LoopData)
	{
		DynamicCOL_Remove(obj, (NJS_OBJECT*)obj->Data1->LoopData);
		ObjectArray_Remove((NJS_OBJECT*)obj->Data1->LoopData);
	}

	Enemy_Delete(obj);
}

void __cdecl Rexon(ObjectMaster* obj)
{
	EntityData1* data = obj->Data1;

	// If already dead, load only the floating shell
	if (obj->SETData.SETData->Flags & 0x1000)
	{
		data->Action = RexonAct_Dead;
	}
	else
	{
		LoadChildObject(LoadObj_Data1, RexonHead, obj);
		Collision_Init(obj, arrayptrandlength(Rexon_Col), 4);
	}

	if (data->Scale.x == 0.0f)
	{
		data->Scale.x = 1.0f;
	}

	if (data->Scale.y != 0.0f)
	{
		data->Action = RexonAct_Move;
	}

	data->Scale.z = data->Position.y;

	data->Object = e_rexon->getmodel();

	obj->DeleteSub = Rexon_Delete;
	obj->MainSub = Rexon_Main;
	obj->DisplaySub = Rexon_Display;

	// Create the dynamic collision
	NJS_OBJECT* object = ObjectArray_GetFreeObject();

	object->pos[0] = data->Position.x;
	object->pos[1] = data->Position.y;
	object->pos[2] = data->Position.z;

	object->ang[0] = 0;
	object->ang[1] = data->Rotation.y;
	object->ang[2] = 0;

	object->scl[0] = data->Scale.x;
	object->scl[1] = data->Scale.x;
	object->scl[2] = data->Scale.x;

	object->basicdxmodel = e_rexoncol->getmodel()->basicdxmodel;

	DynamicCOL_Add((ColFlags)0x08000001, obj, object);
	data->LoopData = (Loop*)object;
}

#pragma endregion

void Rexon_LoadAssets()
{
	LoadModelFile(&e_rexon, "e_rexon", ModelFormat_Basic);
	LoadModelFile(&e_rexoncol, "e_rexoncol", ModelFormat_Basic);
}

void Rexon_FreeAssets()
{
	FreeFileInfo(&e_rexon);
	FreeFileInfo(&e_rexoncol);
}