#include "pch.h"
#include "o_htskybox.h"

/*

Skybox of the level.

The global color set in the level.cpp simulates infinite sky,
the only things we draw from the skybox are clouds with transparency, simulating horizon.

Also handles the Egg Carrier if the level has never been completed.

*/

extern Angle HT_WindDirection;

NJS_TEXNAME HillTopBG_TexNames[2];
NJS_TEXLIST HillTopBG_TexList = { arrayptrandlength(HillTopBG_TexNames) };

static ModelInfo* ht_clouds = nullptr;
static ModelInfo* ht_cloudlayers = nullptr;

struct CloudData {
	NJS_VECTOR pos;
	Float scl;
	Float spd;
	Float alpha;
};

static const NJS_VECTOR SunPositions[2] = {
	{ 4000, 2000, 500 },
	{ -2000, 600, 1000 }
};

static NJS_VECTOR EggCarrierPosition;
static Angle3 EggCarrierRotation;

#pragma region Clouds
static void SpawnCloud(CloudData* cloud)
{
	cloud->pos.x = Camera_Data1->Position.x + 2000 - rand() % 4000;
	cloud->pos.y = -150 + rand() % 150;
	cloud->pos.z = Camera_Data1->Position.z + 2000 - rand() % 4000;
	cloud->spd = 1.0f + (static_cast<float>(rand() % 100) / 100.0f);
	cloud->alpha = 0.0f;
	cloud->scl = 0.5f + (static_cast<float>(rand() % 100) / 100.0f);
}

static void __cdecl CloudHandlerDestroy(task* tp)
{
	if (tp->twp)
	{
		auto clouds = reinterpret_cast<CloudData*>(tp->twp->counter.ptr);

		if (clouds)
		{
			delete[] clouds; // remove allocated memory
		}
	}
}

static void __cdecl CloudHandlerDisplay(task* tp)
{
	if (!MissedFrames)
	{
		auto twp = tp->twp;
		auto clouds = reinterpret_cast<CloudData*>(twp->counter.ptr);
		auto object = reinterpret_cast<NJS_OBJECT*>(twp->value.ptr);

		njSetTexture(&HillTopBG_TexList);

		SetMaterial(1.0f, 1.0f, 1.0f, 1.0f);
		___njClipZ(gClipSky.Near, gClipSky.Far);

		for (int i = 0; i < 30; ++i)
		{
			object->basicdxmodel->mats->diffuse.argb.a = static_cast<Uint8>(clouds[i].alpha * 255);

			njPushMatrixEx();
			njTranslateY(twp->pos.y);
			njTranslateEx(&clouds[i].pos);
			njRotateY(0, HT_WindDirection);
			njScalef(clouds[i].scl);
			DrawModel(object->basicdxmodel);
			njPopMatrixEx();
		}

		___njClipZ(gClipMap.Near, gClipMap.Far);
		ResetMaterial();
	}
}

static void __cdecl CloudHandlerExec(task* tp)
{
	auto twp = tp->twp;
	auto clouds = reinterpret_cast<CloudData*>(twp->counter.ptr);

	for (int i = 0; i < 30; ++i)
	{
		auto player = playertwp[GetTheNearestPlayerNumber(&clouds[i].pos)];

		njPushMatrix(_nj_unit_matrix_);
		njTranslateEx(&clouds[i].pos);
		njRotateY(0, HT_WindDirection);
		njTranslateX(clouds[i].spd);
		njGetTranslation(nullptr, &clouds[i].pos);
		njPopMatrixEx();

		if (clouds[i].alpha < 0.8f)
		{
			clouds[i].alpha += 0.01f;
		}

		if (GetDistance(&player->pos, &clouds[i].pos) > 2000.0f)
		{
			SpawnCloud(&clouds[i]);
		}
	}

	tp->disp(tp);
}

static void CloudHandler(task* tp)
{
	auto twp = tp->twp;
	auto clouds = new CloudData[30]; // Allocate extra memory for cloud data

	twp->counter.ptr = clouds;
	twp->value.ptr = ht_clouds->getmodel();

	tp->exec = CloudHandlerExec;
	tp->disp = CloudHandlerDisplay;
	tp->dest = CloudHandlerDestroy;

	for (int i = 0; i < 30; ++i)
	{
		SpawnCloud(&clouds[i]);
	}
}
#pragma endregion

static void LoadSkyboxAct(task* tp)
{
	auto twp = tp->twp;

	// Remove any existing cloud subtask
	FreeTaskC(tp);

	switch (ActNumber) {
	case 0:
	case 2:
		LoadLenseFlareAtPosition(&SunPositions[0]);
		CreateChildTask(LoadObj_Data1, CloudHandler, tp)->twp->pos.y = -150.0f;
		CreateChildTask(LoadObj_Data1, CloudHandler, tp)->twp->pos.y = 1100.0f;
		twp->pos.y = -300.0f;
		break;
	case 1:
		twp->pos.y = -300.0f;
		break;
	case 3:
		LoadLenseFlareAtPosition(&SunPositions[1]);
		CreateChildTask(LoadObj_Data1, CloudHandler, tp)->twp->pos.y = -500.0f;
		CreateChildTask(LoadObj_Data1, CloudHandler, tp)->twp->pos.y = 800.0f;
		twp->pos.y = -600.0f;
		break;
	}

	if (GetEventFlag(EventFlags_Sonic_RedMountainClear) == false
		&& CurrentCharacter != Characters_Gamma && ActNumber < 2)
	{
		if (ActNumber == 0)
		{
			EggCarrierPosition = { 1215.0f, 839.0f, 3520.0f };
			EggCarrierRotation = { 0, 0x8725, 0 };
		}
		else if (EntityData1Ptrs[0]->Position.z < 2200.0f)
		{
			EggCarrierPosition = { 3100.0f, 1400.0f, -1300.0f };
			EggCarrierRotation = { 0x1000, 0xF725, 0 };
		}

		twp->wtimer = 1; // display egg carrier
	}
	else
	{
		twp->wtimer = 0;
	}

	twp->btimer = ActNumber;
}

static void __cdecl HillTopSkyDisplay(task* tp)
{
	if (!MissedFrames)
	{
		auto twp = tp->twp;
		auto clouds = reinterpret_cast<NJS_OBJECT*>(twp->value.ptr);

		njSetTexture(&HillTopBG_TexList);

		___njClipZ(gClipSky.Near, gClipSky.Far); // farther draw distance

		// Cloud layers
		njPushMatrixEx();
		{
			njTranslate(0, Camera_Data1->Position.x, twp->pos.y, Camera_Data1->Position.z);

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
		if (twp->wtimer == 1)
		{
			njPushMatrixEx();
			njSetTexture((NJS_TEXLIST*)0x2BF4F2C); // EC_Light (Low poly Egg Carrier textures)
			njTranslateEx(&EggCarrierPosition);
			njRotateEx((Angle*)&EggCarrierRotation, false);
			njScalef(0.1f);
			njAction((NJS_ACTION*)0x24983CC, twp->scl.x); // Egg Carrier LOD Action
			njPopMatrixEx();
		}
		
		___njClipZ(gClipMap.Near, gClipMap.Far);
	}
}

static void __cdecl HillTopSkyExec(task* tp)
{
	auto twp = tp->twp;
	auto clouds = (NJS_OBJECT*)twp->value.ptr;

	if (twp->btimer != ActNumber)
	{
		LoadSkyboxAct(tp);
		return;
	}

	// Move the cloud layers texture to simulate wind
	if (LevelFrameCount % 2 == 0)
	{
		int spd = 0;

		while (clouds)
		{
			NJS_MESHSET_SADX* mesh = &clouds->basicdxmodel->meshsets[0];

			for (int i = 0; i < 96; ++i)
			{
				mesh->vertuv[i].u += 1 + spd;
				mesh->vertuv[i].v += 2 - spd;
			}

			clouds = clouds->child;
			++spd;
		}
	}

	// Egg Carrier animation frame
	if (twp->wtimer == 1)
	{
		twp->scl.x += 0.5f;

		// Remove the Egg Carrier at the end of act 1
		if (ActNumber == 1 && playertwp[0]->pos.z > 2200.0f)
		{
			twp->wtimer = 0; // don't draw Egg Carrier
		}
	}

	// Display cloud layers
	tp->disp(tp);

	// Run cloud subtasks
	LoopTaskC(tp);
}

void __cdecl HillTopZone_SkyBox(task* tp)
{
	auto twp = tp->twp;

	twp->value.ptr = ht_cloudlayers->getmodel();

	tp->exec = HillTopSkyExec;
	tp->disp = HillTopSkyDisplay;

	LoadSkyboxAct(tp);
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