#include "pch.h"
#include "o_htskybox.h"

void __cdecl HillTopZone_SkyBox_Display(ObjectMaster* obj) {

}

void __cdecl HillTopZone_SkyBox_Main(ObjectMaster* obj) {

}

void __cdecl HillTopZone_SkyBox(ObjectMaster* obj) {
	SetGlobalPoint2Col_Colors(0, 0, 0);
	Direct3D_SetNearFarPlanes(LevelDrawDistance.Minimum, LevelDrawDistance.Maximum);

	obj->MainSub = HillTopZone_SkyBox_Main;
	obj->DisplaySub = HillTopZone_SkyBox_Display;
}