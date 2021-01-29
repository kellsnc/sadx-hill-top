#include "pch.h"
#include "o_htskybox.h"

/*

Skybox of the level.

The global color set in the level.cpp simulates infinite sky,
the only things we draw from the skybox are clouds with transparency, simulating horizon.

*/

extern Angle HT_WindDirection;

NJS_TEXNAME HillTopBG_TexNames[2];
NJS_TEXLIST HillTopBG_TexList = { arrayptrandlength(HillTopBG_TexNames) };

ModelInfo* ht_clouds = nullptr;
ModelInfo* ht_cloudlayers = nullptr;

typedef struct {
	NJS_VECTOR pos;
	Float size;
	Float speed;
	Float spawn;
} CloudData;

void SpawnCloud(CloudData* cloud) {
	cloud->pos.x = Camera_Data1->Position.x + 2000 - rand() % 4000;
	cloud->pos.y = -150 + rand() % 150;
	cloud->pos.z = Camera_Data1->Position.z + 2000 - rand() % 4000;
	cloud->speed = 1.0f + (static_cast<float>(rand() % 100) / 100.0f);
	cloud->spawn = 0.0f;
	cloud->size = 0.5f + (static_cast<float>(rand() % 100) / 100.0f);
}

void __cdecl CloudHandler_Delete(ObjectMaster* obj) {
	delete[] obj->Data1->LoopData;
}

void __cdecl CloudHandler_Display(ObjectMaster* obj) {
	if (!MissedFrames) {
		EntityData1* data = obj->Data1;
		CloudData* clouds = (CloudData*)data->LoopData;

		njSetTexture(&HillTopBG_TexList);

		for (int i = 0; i < 30; ++i) {
			njPushMatrixEx();
			njTranslateY(data->Position.y);
			njTranslateEx(&clouds[i].pos);
			njRotateY(0, HT_WindDirection);
			njScalef(clouds[i].size);

			data->Object->basicdxmodel->mats->diffuse.argb.a = static_cast<Uint8>(clouds[i].spawn * 255);

			Direct3D_SetNearFarPlanes(SkyboxDrawDistance.Minimum, SkyboxDrawDistance.Maximum);
			njDrawModel_SADX(data->Object->basicdxmodel);
			Direct3D_SetNearFarPlanes(LevelDrawDistance.Minimum, LevelDrawDistance.Maximum);

			njPopMatrixEx();
		}
	}
}

void __cdecl CloudHandler_Main(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	CloudData* clouds = (CloudData*)data->LoopData;

	for (int i = 0; i < 30; ++i) {
		EntityData1* player = EntityData1Ptrs[GetClosestPlayerID(&clouds[i].pos)];

		njPushMatrix(_nj_unit_matrix_);
		njTranslateEx(&clouds[i].pos);
		njRotateY(0, HT_WindDirection);
		njTranslateX(clouds[i].speed);
		njGetTranslation(nullptr, &clouds[i].pos);
		njPopMatrixEx();

		if (clouds[i].spawn < 0.8f) {
			clouds[i].spawn += 0.01f;
		}

		if (GetDistance(&player->Position, &clouds[i].pos) > 2000.0f) {
			SpawnCloud(&clouds[i]);
		}
	}

	obj->DisplaySub(obj);
}

void CloudHandler(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	CloudData* clouds = new CloudData[30];

	data->LoopData = (Loop*)clouds;
	data->Object = ht_clouds->getmodel();
	
	obj->MainSub = CloudHandler_Main;
	obj->DisplaySub = CloudHandler_Display;
	obj->DeleteSub = CloudHandler_Delete;

	for (int i = 0; i < 30; ++i) {
		SpawnCloud(&clouds[i]);
	}
}

void LoadSkyboxAct(ObjectMaster* obj) {
	NJS_VECTOR sunpos1 = { 4000, 2000, 500 };

	switch (CurrentAct) {
	case 0:
		LoadLenseFlareAtPosition(&sunpos1);
		LoadChildObject(LoadObj_Data1, CloudHandler, obj)->Data1->Position.y = -150.0f;
		LoadChildObject(LoadObj_Data1, CloudHandler, obj)->Data1->Position.y = 1100.0f;
		obj->Data1->Position.y = -300.0f;
		break;
	case 1:
		LoadChildObject(LoadObj_Data1, CloudHandler, obj)->Data1->Position.y = -200.0f;
		obj->Data1->Position.y = -200.0f;
		break;
	}

	obj->Data1->Index = CurrentAct;
}

void __cdecl HillTopZone_SkyBox_Display(ObjectMaster* obj) {
	if (!MissedFrames) {
		EntityData1* data = obj->Data1;
		NJS_OBJECT* clouds = data->Object;

		njSetTexture(&HillTopBG_TexList);

		Direct3D_SetNearFarPlanes(SkyboxDrawDistance.Minimum, SkyboxDrawDistance.Maximum);
		njPushMatrixEx();
		njTranslate(0, Camera_Data1->Position.x, data->Position.y, Camera_Data1->Position.z);

		// Clouds
		while (clouds) {
			njTranslateEx((NJS_VECTOR*)&clouds->pos);
			njScaleEx((NJS_VECTOR*)&clouds->scl);

			njPushMatrixEx();
			njRotateY(0, 0xC000 + HT_WindDirection); // Rotated in direction of the wind
			njDrawModel_SADX(clouds->basicdxmodel);
			njPopMatrixEx();

			clouds = clouds->child;
		}

		njPopMatrixEx();
		Direct3D_SetNearFarPlanes(LevelDrawDistance.Minimum, LevelDrawDistance.Maximum);
	}
}

void __cdecl HillTopZone_SkyBox_Main(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	NJS_OBJECT* clouds = data->Object;

	if (data->Index != CurrentAct) {
		LoadSkyboxAct(obj);
		return;
	}

	// Move the cloud layers texture to simulate wind
	if (FrameCounterUnpaused % 2 == 0) {
		int speed = 0;

		while (clouds) {
			NJS_MESHSET_SADX* mesh = &clouds->basicdxmodel->meshsets[0];

			for (int i = 0; i < 96; ++i) {
				mesh->vertuv[i].u += 1 + speed;
				mesh->vertuv[i].v += 2 - speed;
			}

			clouds = clouds->child;
			++speed;
		}
	}

	// Display cloud layers
	obj->DisplaySub(obj);

	// Display clouds
	RunObjectChildren(obj);
}

void __cdecl HillTopZone_SkyBox(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;

	data->Object = ht_cloudlayers->getmodel();
	
	obj->MainSub = HillTopZone_SkyBox_Main;
	obj->DisplaySub = HillTopZone_SkyBox_Display;

	LoadSkyboxAct(obj);
}

void SkyBox_LoadAssets() {
	LoadModel(&ht_clouds, "ht_clouds", ModelFormat_Basic);
	LoadModel(&ht_cloudlayers, "ht_cloudlayers", ModelFormat_Basic);
}