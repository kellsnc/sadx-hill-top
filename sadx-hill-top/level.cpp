#include "pch.h"

#include "paths.h"
#include "deathzones.h"

#include "objects/o_lavafall.h"

Angle HT_WindDirection = 0;

NJS_TEXNAME HillTop_TexNames[5];
NJS_TEXLIST HillTop_TexList = { arrayptrandlength(HillTop_TexNames) };

LandTable* LavaTables[3] = { nullptr };

StartPosition StartPoses[] = {
	{ LevelIDs_RedMountain, 0, { 0, 0, 0 }, 0 }, // Sonic Act 1
	{ LevelIDs_RedMountain, 1, { 0, 0, 0 }, 0 }, // Sonic Act 2
	{ LevelIDs_RedMountain, 1, { 0, 0, 0 }, 0 }, // Gamma Act 2
	{ LevelIDs_RedMountain, 2, { 0, 0, 0 }, 0 }  // Knuckles Act 3
};

// The actual lava texture we are going to draw
static NJS_TEXNAME CurrentLavaNames[2] = { 0 };
static NJS_TEXLIST CurrentLavaTex = { CurrentLavaNames, 2 };

// Draw the animated lava models
void __cdecl HillTopLava_Display(ObjectMaster* obj) {
	if (!MissedFrames) {
		EntityData1* data = obj->Data1;
		LandTable* land = LavaTables[CurrentAct];

		CurrentLavaNames[0] = YOUGAN_ANIM_TEXNAMES[data->Index];
		CurrentLavaNames[1] = LAVAFALL_TexList.textures[data->Index];

		njSetTexture(&CurrentLavaTex);

		if (land) {
			for (int i = 0; i < land->COLCount; ++i) {
				COL* col = &land->Col[i];

				njPushMatrixEx();
				njTranslateEx((NJS_VECTOR*)&col->Model->pos);
				njRotateEx((Angle*)&col->Model->ang, 0);
				njDrawModel_SADX(col->Model->basicdxmodel);
				njPopMatrixEx();
			}
		}
	}
}

void __cdecl HillTopLava_Main(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;

	if (FrameCounterUnpaused % 2 == 0) {
		++data->Index;
	}

	if (data->Index >= 12) {
		data->Index = 0;
	}

	obj->DisplaySub(obj);
}

void __cdecl HillTopZone_Main(ObjectMaster* obj) {
	
}

void __cdecl HillTopZone_Init(ObjectMaster* obj) {
	
	// This initializes the music after the timer in InvulnerableTime is elpased 
	// and no event is running
	ObjectMaster* musicobj = LoadObject(LoadObj_Data1, 1, LoadMusic_EventDelayed);
	
	musicobj->Data1->Action = MusicIDs_redmntn1;
	musicobj->Data1->InvulnerableTime = 3;

	// Load the lava handler

	ObjectMaster* lavaobj = LoadObject(LoadObj_Data1, 3, HillTopLava_Main);
	lavaobj->DisplaySub = HillTopLava_Display;

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

	LavaTables[0] = LoadLandTable(helperFunctions, "system\\hilltopzone0_lava.sa1lvl");
	LavaTables[1] = LoadLandTable(helperFunctions, "system\\hilltopzone1_lava.sa1lvl");
	LavaTables[2] = LoadLandTable(helperFunctions, "system\\hilltopzone2_lava.sa1lvl");

	helperFunctions.RegisterPathList(hilltop0_pathdata);
	helperFunctions.RegisterPathList(hilltop1_pathdata);
	helperFunctions.RegisterPathList(hilltop2_pathdata);

	SonicStartArray[11] = StartPoses[0];
	SonicStartArray[12] = StartPoses[1];
	GammaStartArray[3]	= StartPoses[2];
	KnucklesStartArray[1] = StartPoses[3];

	LevelObjects[LevelIDs_RedMountain] = HillTopZone_Init;

	DeathZoneList[LevelIDs_RedMountain][0] = hilltope0_deathzones;
	DeathZoneList[LevelIDs_RedMountain][1] = hilltope1_deathzones;
	DeathZoneList[LevelIDs_RedMountain][2] = hilltope2_deathzones;

	FogData_RedMountain1[0].Layer = 167;
	FogData_RedMountain1[0].Distance = 6000;
	FogData_RedMountain1[0].Color = 0x80FFFFFF;
	FogData_RedMountain1[0].Toggle = 1;

	MusicList[MusicIDs_redmntn1].Name = "hilltop";
}