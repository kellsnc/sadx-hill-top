#include "pch.h"

#include "paths.h"
#include "deathzones.h"
#include "objects/o_htskybox.h"

NJS_TEXNAME HillTop_TexNames[5];
NJS_TEXLIST HillTop_TexList = { arrayptrandlength(HillTop_TexNames) };

StartPosition StartPoses[] = {
	{ LevelIDs_RedMountain, 0, { 0, 0, 0 }, 0 }, // Sonic Act 1
	{ LevelIDs_RedMountain, 0, { 0, 0, 0 }, 0 }, // Gamma Act 1
	{ LevelIDs_RedMountain, 1, { 0, 0, 0 }, 0 }, // Sonic Act 2
	{ LevelIDs_RedMountain, 1, { 0, 0, 0 }, 0 }, // Gamma Act 2
	{ LevelIDs_RedMountain, 2, { 0, 0, 0 }, 0 }  // Knuckles Act 3
};

void __cdecl HillTopZone_Main(ObjectMaster* obj) {

}

void __cdecl HillTopZone_Init(ObjectMaster* obj) {

	// This initialize the music after the timer in InvulnerableTime is elpased 
	// and no event is running
	ObjectMaster* musicobj = LoadObject(LoadObj_Data1, 1, LoadMusic_EventDelayed);
	
	musicobj->Data1->Action = MusicIDs_redmntn1;
	musicobj->Data1->InvulnerableTime = 3;

	obj->MainSub = HillTopZone_Main;
}

LandTable* LoadLandTable(const HelperFunctions& helperFunctions, const char* name) {
	LandTableInfo* info = new LandTableInfo(helperFunctions.GetReplaceablePath(name));

	info->getlandtable()->TexList = &HillTop_TexList;
	
	return info->getlandtable();
}

void Level_Init(const HelperFunctions& helperFunctions) {
	GeoLists[LevelIDs_RedMountain * 8] = LoadLandTable(helperFunctions, "system\\hilltopzone0.sa1lvl");
	GeoLists[LevelIDs_RedMountain * 8 + 1] = LoadLandTable(helperFunctions, "system\\hilltopzone1.sa1lvl");
	GeoLists[LevelIDs_RedMountain * 8 + 2] = LoadLandTable(helperFunctions, "system\\hilltopzone2.sa1lvl");

	helperFunctions.RegisterPathList(hilltop0_pathdata);
	helperFunctions.RegisterPathList(hilltop1_pathdata);
	helperFunctions.RegisterPathList(hilltop2_pathdata);

	helperFunctions.RegisterStartPosition(Characters_Sonic, StartPoses[0]);
	helperFunctions.RegisterStartPosition(Characters_Gamma, StartPoses[1]);
	helperFunctions.RegisterStartPosition(Characters_Sonic, StartPoses[2]);
	helperFunctions.RegisterStartPosition(Characters_Gamma, StartPoses[3]);
	helperFunctions.RegisterStartPosition(Characters_Knuckles, StartPoses[4]);

	LevelObjects[LevelIDs_RedMountain] = HillTopZone_Init;
	SkyboxObjects[LevelIDs_RedMountain] = HillTopZone_SkyBox;

	DeathZoneList[LevelIDs_RedMountain][0] = hilltope0_deathzones;
	DeathZoneList[LevelIDs_RedMountain][1] = hilltope1_deathzones;
	DeathZoneList[LevelIDs_RedMountain][2] = hilltope2_deathzones;

	MusicList[MusicIDs_redmntn1].Name = "hilltop";
}