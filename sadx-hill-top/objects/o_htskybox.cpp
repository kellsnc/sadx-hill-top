#include "pch.h"
#include "o_htskybox.h"

/*

Skybox of the level.

The global color set in the level.cpp simulates infinite sky,
the only things we draw from the skybox are clouds with transparency, simulating horizon.

Also handles the Egg Carrier if the level has never been completed.

*/

extern Angle HT_WindDirection;

static NJS_TEXNAME HillTopBG_TexNames[2];
NJS_TEXLIST HillTopBG_TexList = { arrayptrandlength(HillTopBG_TexNames) };

static ModelInfo* ht_clouds = nullptr;
static ModelInfo* ht_cloudlayers = nullptr;

struct CloudData {
	NJS_VECTOR pos;
	Float size;
	Float speed;
	Float spawn;
};

static const NJS_VECTOR SunPositions[2] = {
	{ 4000, 2000, 500 },
	{ -2000, 600, 1000 }
};

static NJS_VECTOR EggCarrierPosition;
static Rotation3 EggCarrierRotation;

#pragma region Clouds
static void SpawnCloud(CloudData* cloud)
{
	cloud->pos.x = Camera_Data1->Position.x + 2000 - rand() % 4000;
	cloud->pos.y = -150 + rand() % 150;
	cloud->pos.z = Camera_Data1->Position.z + 2000 - rand() % 4000;
	cloud->speed = 1.0f + (static_cast<float>(rand() % 100) / 100.0f);
	cloud->spawn = 0.0f;
	cloud->size = 0.5f + (static_cast<float>(rand() % 100) / 100.0f);
}

static void __cdecl CloudHandler_Delete(ObjectMaster* obj)
{
	if (obj->Data1->LoopData)
	{
		delete[] obj->Data1->LoopData;
	}
}

static void __cdecl CloudHandler_Display(ObjectMaster* obj)
{
	if (!MissedFrames)
	{
		auto data = obj->Data1;
		CloudData* clouds = (CloudData*)data->LoopData;

		njSetTexture(&HillTopBG_TexList);

		for (int i = 0; i < 30; ++i)
		{
			njPushMatrixEx();
			njTranslateY(data->Position.y);
			njTranslateEx(&clouds[i].pos);
			njRotateY(0, HT_WindDirection);
			njScalef(clouds[i].size);

			data->Object->basicdxmodel->mats->diffuse.argb.a = static_cast<Uint8>(clouds[i].spawn * 255);

			Direct3D_SetNearFarPlanes(SkyboxDrawDistance.Minimum, SkyboxDrawDistance.Maximum);
			DrawModel(data->Object->basicdxmodel);
			Direct3D_SetNearFarPlanes(LevelDrawDistance.Minimum, LevelDrawDistance.Maximum);

			njPopMatrixEx();
		}
	}
}

static void __cdecl CloudHandler_Main(ObjectMaster* obj)
{
	auto data = obj->Data1;
	auto clouds = reinterpret_cast<CloudData*>(data->LoopData);

	for (int i = 0; i < 30; ++i)
	{
		auto player = EntityData1Ptrs[GetTheNearestPlayerNumber(&clouds[i].pos)];

		njPushMatrix(_nj_unit_matrix_);
		njTranslateEx(&clouds[i].pos);
		njRotateY(0, HT_WindDirection);
		njTranslateX(clouds[i].speed);
		njGetTranslation(nullptr, &clouds[i].pos);
		njPopMatrixEx();

		if (clouds[i].spawn < 0.8f)
		{
			clouds[i].spawn += 0.01f;
		}

		if (GetDistance(&player->Position, &clouds[i].pos) > 2000.0f)
		{
			SpawnCloud(&clouds[i]);
		}
	}

	obj->DisplaySub(obj);
}

static void CloudHandler(ObjectMaster* obj)
{
	auto data = obj->Data1;
	auto clouds = new CloudData[30];

	data->LoopData = (Loop*)clouds;
	data->Object = ht_clouds->getmodel();

	obj->MainSub = CloudHandler_Main;
	obj->DisplaySub = CloudHandler_Display;
	obj->DeleteSub = CloudHandler_Delete;

	for (int i = 0; i < 30; ++i)
	{
		SpawnCloud(&clouds[i]);
	}
}
#pragma endregion

static void LoadSkyboxAct(ObjectMaster* obj)
{
	switch (CurrentAct)
	{
	case 0:
	case 2:
		LoadLenseFlareAtPosition(&SunPositions[0]);
		LoadChildObject(LoadObj_Data1, CloudHandler, obj)->Data1->Position.y = -150.0f;
		LoadChildObject(LoadObj_Data1, CloudHandler, obj)->Data1->Position.y = 1100.0f;
		obj->Data1->Position.y = -300.0f;
		break;
	case 1:
		obj->Data1->Position.y = -300.0f;
		break;
	case 3:
		LoadLenseFlareAtPosition(&SunPositions[1]);
		LoadChildObject(LoadObj_Data1, CloudHandler, obj)->Data1->Position.y = -500.0f;
		LoadChildObject(LoadObj_Data1, CloudHandler, obj)->Data1->Position.y = 800.0f;
		obj->Data1->Position.y = -600.0f;
		break;
	}

	if (GetEventFlag(EventFlags_Sonic_RedMountainClear) == false
		&& CurrentCharacter != Characters_Gamma && CurrentAct < 2)
	{
		if (CurrentAct == 0)
		{
			EggCarrierPosition = { 1215.0f, 839.0f, 3520.0f };
			EggCarrierRotation = { 0, 0x8725, 0 };
		}
		else if (EntityData1Ptrs[0]->Position.z < 2200.0f)
		{
			EggCarrierPosition = { 3100.0f, 1400.0f, -1300.0f };
			EggCarrierRotation = { 0x1000, 0xF725, 0 };
		}

		obj->Data1->field_A = 1; // display egg carrier
	}
	else
	{
		obj->Data1->field_A = 0;
	}

	obj->Data1->Index = CurrentAct;
}

static void __cdecl HillTopZone_SkyBox_Display(ObjectMaster* obj)
{
	if (!MissedFrames)
	{
		auto data = obj->Data1;
		auto clouds = data->Object;
		auto skybox = reinterpret_cast<NJS_OBJECT*>(data->LoopData);

		njSetTexture(&HillTopBG_TexList);

		Direct3D_SetNearFarPlanes(SkyboxDrawDistance.Minimum, SkyboxDrawDistance.Maximum);

		// Cloud layers
		njPushMatrixEx();
		{
			njTranslate(0, Camera_Data1->Position.x, data->Position.y, Camera_Data1->Position.z);

			while (clouds)
			{
				njTranslateEx((NJS_VECTOR*)&clouds->pos);
				njScaleEx((NJS_VECTOR*)&clouds->scl);

				njPushMatrixEx();
				njRotateY(0, 0xC000 + HT_WindDirection); // Rotated in direction of the wind
				DrawModel(clouds->basicdxmodel);
				njPopMatrixEx();

				clouds = clouds->child;
			}

			njPopMatrixEx();
		}

		// Egg Carrier display
		if (data->field_A == 1)
		{
			njPushMatrixEx();
			njSetTexture((NJS_TEXLIST*)0x2BF4F2C); // EC_Light (Low poly Egg Carrier textures)
			njTranslateEx(&EggCarrierPosition);
			njRotateEx((Angle*)&EggCarrierRotation, false);
			njScalef(0.1f);
			njAction((NJS_ACTION*)0x24983CC, obj->Data1->Scale.x); // Egg Carrier LOD Action
			njPopMatrixEx();
		}

		Direct3D_SetNearFarPlanes(LevelDrawDistance.Minimum, LevelDrawDistance.Maximum);
	}
}

static void __cdecl HillTopZone_SkyBox_Main(ObjectMaster* obj)
{
	auto data = obj->Data1;
	auto clouds = data->Object;

	if (data->Index != CurrentAct)
	{
		LoadSkyboxAct(obj);
		return;
	}

	// Move the cloud layers texture to simulate wind
	if (FrameCounterUnpaused % 2 == 0)
	{
		int speed = 0;

		while (clouds)
		{
			NJS_MESHSET_SADX* mesh = &clouds->basicdxmodel->meshsets[0];

			for (int i = 0; i < 96; ++i)
			{
				mesh->vertuv[i].u += 1 + speed;
				mesh->vertuv[i].v += 2 - speed;
			}

			clouds = clouds->child;
			++speed;
		}
	}

	// Egg Carrier animation frame
	if (data->field_A == 1)
	{
		obj->Data1->Scale.x += 0.5f;

		// Remove the Egg Carrier at the end of act 1
		if (CurrentAct == 1 && EntityData1Ptrs[0]->Position.z > 2200)
		{
			data->field_A = 0;
		}
	}

	// Display cloud layers
	obj->DisplaySub(obj);

	// Display clouds
	RunObjectChildren(obj);
}

void __cdecl HillTopZone_SkyBox(ObjectMaster* obj)
{
	auto data = obj->Data1;

	data->Object = ht_cloudlayers->getmodel();

	obj->MainSub = HillTopZone_SkyBox_Main;
	obj->DisplaySub = HillTopZone_SkyBox_Display;

	LoadSkyboxAct(obj);
}

void SkyBox_LoadAssets()
{
	LoadModelFile(&ht_clouds, "ht_clouds", ModelFormat_Basic);
	LoadModelFile(&ht_cloudlayers, "ht_cloudlayers", ModelFormat_Basic);
}

void SkyBox_FreeAssets()
{
	FreeFileInfo(&ht_clouds);
	FreeFileInfo(&ht_cloudlayers);
}