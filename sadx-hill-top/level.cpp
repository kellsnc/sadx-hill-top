#include "pch.h"

#include "paths.h"
#include "deathzones.h"

void __cdecl HillTopZone_SkyBox(ObjectMaster* obj) {

}

void __cdecl HillTopZone_Handler(ObjectMaster* obj) {

}

LandTable* LoadLandTable(const HelperFunctions& helperFunctions, const char* name) {
	LandTableInfo* info = new LandTableInfo(helperFunctions.GetReplaceablePath(name));
	
	return info->getlandtable();
}

void Level_Init(const HelperFunctions& helperFunctions) {
	GeoLists[LevelIDs_RedMountain * 8] = LoadLandTable(helperFunctions, "system\\hilltopzone0.sa1lvl");
	GeoLists[LevelIDs_RedMountain * 8 + 1] = LoadLandTable(helperFunctions, "system\\hilltopzone1.sa1lvl");
	GeoLists[LevelIDs_RedMountain * 8 + 2] = LoadLandTable(helperFunctions, "system\\hilltopzone2.sa1lvl");

	helperFunctions.RegisterPathList(hilltop0_pathdata);
	helperFunctions.RegisterPathList(hilltop1_pathdata);
	helperFunctions.RegisterPathList(hilltop2_pathdata);

	LevelObjects[LevelIDs_RedMountain] = HillTopZone_Handler;
	SkyboxObjects[LevelIDs_RedMountain] = HillTopZone_SkyBox;

	DeathZoneList[LevelIDs_RedMountain][0] = hilltope0_deathzones;
	DeathZoneList[LevelIDs_RedMountain][1] = hilltope0_deathzones;
	DeathZoneList[LevelIDs_RedMountain][2] = hilltope0_deathzones;

	MusicList[MusicIDs_redmntn1].Name = "hilltop";
	MusicList[MusicIDs_redmntn2].Name = "hilltop";
}