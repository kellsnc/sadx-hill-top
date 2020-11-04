#include "pch.h"
#include "o_htskybox.h"

NJS_TEXNAME HillTopBG_TexNames[2];
NJS_TEXLIST HillTopBG_TexList = { arrayptrandlength(HillTopBG_TexNames) };

ModelInfo* ht_skybox = nullptr;

void __cdecl HillTopZone_SkyBox_Display(ObjectMaster* obj) {
	if (!MissedFrames) {
		EntityData1* data = obj->Data1;

		Direct3D_SetNearFarPlanes(SkyboxDrawDistance.Minimum, SkyboxDrawDistance.Maximum);

		DisableFog();
		njSetTexture(&HillTopBG_TexList);
		njPushMatrixEx();
		njTranslateEx(&Camera_Data1->Position);
		njDrawModel_SADX(data->Object->basicdxmodel);
		njPopMatrixEx();
		ToggleStageFog();

		Direct3D_SetNearFarPlanes(LevelDrawDistance.Minimum, LevelDrawDistance.Maximum);
	}
}

void __cdecl HillTopZone_SkyBox_Main(ObjectMaster* obj) {
	obj->DisplaySub(obj);
}

void __cdecl HillTopZone_SkyBox(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;

	SetGlobalPoint2Col_Colors(0, 0, 0);

	data->Object = ht_skybox->getmodel();
	
	obj->MainSub = HillTopZone_SkyBox_Main;
	obj->DisplaySub = HillTopZone_SkyBox_Display;
}