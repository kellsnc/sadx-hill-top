#include "pch.h"
#include "o_htskybox.h"

extern Angle HT_WindDirection;

NJS_TEXNAME HillTopBG_TexNames[2];
NJS_TEXLIST HillTopBG_TexList = { arrayptrandlength(HillTopBG_TexNames) };

ModelInfo* ht_clouds = nullptr;
ModelInfo* ht_cloudlayers = nullptr;

void __cdecl HillTopZone_SkyBox_Display(ObjectMaster* obj) {
	if (!MissedFrames) {
		EntityData1* data = obj->Data1;
		NJS_OBJECT* clouds = data->Object;

		njSetTexture(&HillTopBG_TexList);

		njPushMatrixEx();
		njTranslate(0, Camera_Data1->Position.x, -300.0f, Camera_Data1->Position.z);

		// Clouds
		while (clouds) {
			njTranslateEx((NJS_VECTOR*)&clouds->pos);

			njPushMatrixEx();
			njRotateY(0, 0xC000 + HT_WindDirection); // Rotated in direction of the wind
			njScalef(1.5f);
			DrawModel_Queue(clouds->basicdxmodel, QueuedModelFlagsB_EnableZWrite); // Queue alpha to see all layers
			njPopMatrixEx();

			clouds = clouds->child;
		}

		njPopMatrixEx();
	}
}

void __cdecl HillTopZone_SkyBox_Main(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	NJS_OBJECT* clouds = data->Object;

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
}

void __cdecl HillTopZone_SkyBox(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	NJS_VECTOR sunpos = { 4000, 2000, 500 };

	LoadLenseFlareAtPosition(&sunpos);

	SetGlobalPoint2Col_Colors(0xFF1844FF, 0xFF2149FF, 0xFF002EFF);

	data->Object = ht_cloudlayers->getmodel();
	
	obj->MainSub = HillTopZone_SkyBox_Main;
	obj->DisplaySub = HillTopZone_SkyBox_Display;
}

void SkyBox_LoadAssets() {
	LoadModel(&ht_clouds, "ht_clouds", ModelFormat_Basic);
	LoadModel(&ht_cloudlayers, "ht_cloudlayers", ModelFormat_Basic);
}