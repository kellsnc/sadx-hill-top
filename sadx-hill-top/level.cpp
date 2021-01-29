#include "pch.h"

#include "paths.h"
#include "deathzones.h"
#include "event.h"
#include "lava.h"

Angle HT_WindDirection = 0;

NJS_TEXNAME HillTop_TexNames[5];
NJS_TEXLIST HillTop_TexList = { arrayptrandlength(HillTop_TexNames) };

StartPosition StartPoses[] = {
	{ LevelIDs_RedMountain, 0, { 0.0f, 0.0f, 0.0f }, 0x8000 }, // Sonic Act 1
	{ LevelIDs_RedMountain, 1, { 0.0f, 50.0f, 0.0f }, 0 }, // Sonic Act 2
	{ LevelIDs_RedMountain, 1, { 0.0f, 0.0f, 0.0f }, 0 }, // Gamma Act 2
	{ LevelIDs_RedMountain, 2, { 895.0f, 295.0f, 515.0f }, 0 }  // Knuckles Act 3
};

FogData HillTopFogData[] = {
	{ 167, 6000, 0x80FFFFFF, 1 },
	{ 167, 6000, 0x80FFFFFF, 1 }
};

DrawDistance HillTopSkyDrawDist[] = {
	{ -1.0f, -10000.0f },
	{ -1.0f, -5000.0f },
	{ -1.0f, -2000.0f }
};

DrawDistance HillTopDrawDists[] = {
	{ -1.0f, -2500.0f },
	{ -1.0f, -1600.0f },
	{ -1.0f, -1000.0f }
};

// Level handler (init level-specific stuff):

void HillTop_SetViewData() {
	SkyboxDrawDistance = HillTopSkyDrawDist[ClipLevel];
	LevelDrawDistance = HillTopDrawDists[ClipLevel];
	LevelFogData = HillTopFogData[CurrentAct];
}

void __cdecl HillTopZone_Display(ObjectMaster* obj) {
	if (CurrentAct < 2) {
		njPushMatrixEx();
		Direct3D_SetNearFarPlanes(SkyboxDrawDistance.Minimum, SkyboxDrawDistance.Maximum);
		njSetTexture((NJS_TEXLIST*)0x2BF4F2C); // EC_Light (Low poly Egg Carrier textures)
		njTranslate(_nj_current_matrix_ptr_, 1215.0f, 839.0f, 3520.0f);
		njRotateY_(0x8725);
		njScalef(0.1f);
		njAction((NJS_ACTION*)0x24983CC, obj->Data1->Scale.x); // Egg Carrier LOD Action
		Direct3D_SetNearFarPlanes(LevelDrawDistance.Minimum, LevelDrawDistance.Maximum);
		njPopMatrixEx();
	}
}

void __cdecl HillTopZone_Main(ObjectMaster* obj) {
	if (CurrentAct == 0) {
		// Act swap
		if (IsSpecificPlayerInSphere(301.0f, 164.0f, 3145.0f, 100.0f, 0)) {
			NextAct_FreeLandTable(1);
			NextAct_IncrementCurrentStageAndAct(1);
			NextAct_SetCameraData(1);
			NextAct_IncrementAct(1);
			ForcePlayerAction(0, 24);
			MovePlayerToStartPoint(EntityData1Ptrs[0]);
			HillTop_SetViewData();
		}

		// Egg Carrier display
		if (obj->DisplaySub) {
			obj->Data1->Scale.x += 0.5f;
			obj->DisplaySub(obj);
		}
	}
}

void __cdecl HillTopZone_Init(ObjectMaster* obj) {
	HillTop_SetViewData(); // Set fog, view distance, etc.
	LoadLavaManager(); // Load the object that handles animated lava geometry

	// This initializes the music only if no event is running.
	ObjectMaster* musicobj = LoadObject(LoadObj_Data1, 1, LoadMusic_EventDelayed);
	musicobj->Data1->Action = MusicIDs_redmntn1;
	musicobj->Data1->InvulnerableTime = 3;

	// Main level function ran every frame, used mostly for act swaps.
	obj->MainSub = HillTopZone_Main;

	// Provide the Egg Carrier display if the level was never completed.
	if (GetEventFlag(EventFlags_Sonic_RedMountainClear) == false && CurrentAct == 0) {
		obj->DisplaySub = HillTopZone_Display;
	}
}

// Replace Red Mountain with our level:

void LoadSkyboxObject_r() {
	SetGlobalPoint2Col_Colors(GlobalColorsLevel[CurrentLevel].c1, GlobalColorsLevel[CurrentLevel].c2, GlobalColorsLevel[CurrentLevel].c3);
	
	if (SkyboxObjects[CurrentLevel]) {
		if (CurrentLevel == LevelIDs_RedMountain) {
			LoadObject(LoadObj_Data1, 2, SkyboxObjects[CurrentLevel]);
		}
		else {
			LoadObject(LoadObj_Data1, 1, SkyboxObjects[CurrentLevel]);
		}
	}
}

void LoadHillTopLandTables(const HelperFunctions& helperFunctions) {
	GeoLists[LevelIDs_RedMountain * 8] = LoadLandTable(helperFunctions, "system\\hilltopzone0.sa1lvl", &HillTop_TexList);
	GeoLists[LevelIDs_RedMountain * 8 + 1] = LoadLandTable(helperFunctions, "system\\hilltopzone1.sa1lvl", &HillTop_TexList);
	GeoLists[LevelIDs_RedMountain * 8 + 2] = GeoLists[LevelIDs_RedMountain * 8];
}

void Level_Init(const HelperFunctions& helperFunctions) {
	LoadHillTopLandTables(helperFunctions); // Main geometry
	LoadLavaLandTables(helperFunctions); // Animated lava geometry
	
	// Paths
	helperFunctions.RegisterPathList(hilltop0_pathdata);
	helperFunctions.RegisterPathList(hilltop1_pathdata);
	helperFunctions.RegisterPathList(hilltop2_pathdata);

	// Start positions
	helperFunctions.RegisterStartPosition(Characters_Sonic, StartPoses[0]);
	helperFunctions.RegisterStartPosition(Characters_Sonic, StartPoses[1]);
	helperFunctions.RegisterStartPosition(Characters_Gamma, StartPoses[2]);
	helperFunctions.RegisterStartPosition(Characters_Knuckles, StartPoses[3]);

	// Level Handler
	LevelObjects[LevelIDs_RedMountain] = HillTopZone_Init;

	// Deathzones
	DeathZoneList[LevelIDs_RedMountain][0] = hilltope0_deathzones;
	DeathZoneList[LevelIDs_RedMountain][1] = hilltope1_deathzones;
	DeathZoneList[LevelIDs_RedMountain][2] = hilltope0_deathzones;

	// Music
	MusicList[MusicIDs_redmntn1].Name = "hilltop";

	// Sky color
	GlobalColorsLevel[LevelIDs_RedMountain] = { 0xFF1844FF, 0xFF2149FF, 0xFF002EFF };

	// Event
	HookRedMountainEvent();

	// Fix skybox transparency
	WriteJump(LoadSkyboxObject, LoadSkyboxObject_r);
}