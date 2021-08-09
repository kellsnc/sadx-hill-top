#include "pch.h"

#include "paths.h"
#include "deathzones.h"
#include "event.h"
#include "lava.h"

#include "enemies/e_subeggman.h"

Angle HT_WindDirection = 0;

static bool CFG_NoBoss = false;

static int MusicIDs_hilltop1 = MusicIDs_redmntn1;
static int MusicIDs_hilltop2 = MusicIDs_redmntn2;

MusicInfo Music_HillTop1 = { "hilltop1", true };
MusicInfo Music_HillTop2 = { "hilltop2", true };

NJS_TEXNAME HillTop_TexNames[5];
NJS_TEXLIST HillTop_TexList = { arrayptrandlength(HillTop_TexNames) };

LandTableInfo* HillTopLands[3] = { nullptr };

StartPosition StartPoses[] = {
	{ LevelIDs_RedMountain, 0, { 0.0f, 0.0f, 0.0f }, 0x8000 }, // Sonic Act 1
	{ LevelIDs_RedMountain, 1, { -1018.0f, -50.0f, -1235.0f }, 0 }, // Sonic Act 2
	{ LevelIDs_RedMountain, 1, { 15.0f, 700.0f, 225.0f }, 0 }, // Gamma Act 2
	{ LevelIDs_RedMountain, 2, { 895.0f, 295.0f, 515.0f }, 0 },  // Knuckles Act 3
	{ LevelIDs_RedMountain, 3, { 0.0f, -20.0f, 0.0f }, 0 }  // Sonic Act 4 (boss)
};

FogData HillTopFogData[] = {
	{ 167, 6000, 0x80FFFFFF, 1 },
	{ 167, 6000, 0x80FFFFFF, 1 },
	{ 167, 6000, 0x80FFFFFF, 1 },
	{ 167, 6000, 0x80FFFFFF, 1 }
};

DrawDistance HillTopSkyDrawDist[] = {
	{ -1.0f, -9500.0f },
	{ -1.0f, -5000.0f },
	{ -1.0f, -2000.0f }
};

DrawDistance HillTopDrawDists[] = {
	{ -1.0f, -2500.0f },
	{ -1.0f, -1600.0f },
	{ -1.0f, -1000.0f }
};

void HillTop_SetViewData() {
	SkyboxDrawDistance = HillTopSkyDrawDist[ClipLevel];
	LevelDrawDistance = HillTopDrawDists[ClipLevel];
	LevelFogData = HillTopFogData[CurrentAct];
}

#pragma region Level Handler
void LoadCurrentActMusic() {
	ObjectMaster* musicobj = LoadObject(LoadObj_Data1, 1, LoadMusic_EventDelayed);
	musicobj->Data1->Action = CurrentAct == 1 ? MusicIDs_hilltop2 : MusicIDs_hilltop1;
	musicobj->Data1->InvulnerableTime = 3;
}

void __cdecl HillTopZone_Main(ObjectMaster* obj) {
	if (CurrentAct == 0) {
		// Act 1-2 swap
		if (IsSpecificPlayerInSphere(301.0f, 164.0f, 3145.0f, 100.0f, 0)) {
			SoundManager_Delete2();
			NextAct_FreeLandTable(1);
			NextAct_IncrementCurrentStageAndAct(1);
			NextAct_SetCameraData(1);
			NextAct_IncrementAct(1);
			ForcePlayerAction(0, 24);
			MovePlayerToStartPoint(EntityData1Ptrs[0]);
			HillTop_SetViewData();
			LoadCurrentActMusic();
		}
	}
	else if (CurrentAct == 1) {
		// Act 2-4 swap
		int player = IsPlayerInsideSphere_(15.0f, 950.0f, 225.0f, 50.0f) - 1;

		if (player >= 0) {
			ForcePlayerAction(player, 24);

			// If level has been completed once, go to boss instead
			if (player == 0 && GetEventFlag(EventFlags_Sonic_RedMountainClear) == true && CFG_NoBoss == false) {
				NextAct_FreeLandTable(2);
				NextAct_IncrementCurrentStageAndAct(2);
				NextAct_SetCameraData(2);
				NextAct_IncrementAct(2);
				MovePlayerToStartPoint(EntityData1Ptrs[0]);
				HillTop_SetViewData();
				Boss_SubEggman_Init(obj);
			}
			else {
				// teleport to end of level that's further way
				SetPlayerPosition(player, -480.0f, 935.0f, 3030.0f);
				EntityData1Ptrs[player]->Rotation.y = 0x3E80;
				CharObj2Ptrs[player]->Speed = { 2, 8, 0 };
			}
		}
	}
}

void __cdecl HillTopZone_Init(ObjectMaster* obj) {
	HillTop_SetViewData(); // Set fog, view distance, etc.
	LoadLavaManager(); // Load the object that handles animated lava geometry

	// If current act is Eggman boss, load that instead
	if (CurrentAct == 3) {
		obj->MainSub = Boss_SubEggman_Init;
	}
	else {
		// This initializes the music only if no event is running.
		LoadCurrentActMusic();

		// Main level function ran every frame, used mostly for act swaps.
		obj->MainSub = HillTopZone_Main;
	}
}
#pragma endregion

// Replace Red Mountain with our level:

void LoadSkyboxObject_r() {
	SetGlobalPoint2Col_Colors(GlobalColorsLevel[CurrentLevel].c1, GlobalColorsLevel[CurrentLevel].c2, GlobalColorsLevel[CurrentLevel].c3);
	
	if (SkyboxObjects[CurrentLevel]) {
		if (CurrentLevel == LevelIDs_RedMountain) {
			LoadObject(LoadObj_Data1, 2, SkyboxObjects[CurrentLevel]); // Put this in object index 2 to fix transparency issues
		}
		else {
			LoadObject(LoadObj_Data1, 1, SkyboxObjects[CurrentLevel]);
		}
	}
}

void LoadHillTopLandTables() {
	LoadLandTableFile(&HillTopLands[0], "system\\hilltopzone0.sa1lvl", &HillTop_TexList);
	LoadLandTableFile(&HillTopLands[1], "system\\hilltopzone1.sa1lvl", &HillTop_TexList);
	LoadLandTableFile(&HillTopLands[2], "system\\hilltopzone3.sa1lvl", &HillTop_TexList);

	GeoLists[LevelIDs_RedMountain * 8] = HillTopLands[0]->getlandtable();
	GeoLists[LevelIDs_RedMountain * 8 + 1] = HillTopLands[1]->getlandtable();
	GeoLists[LevelIDs_RedMountain * 8 + 2] = HillTopLands[0]->getlandtable();
	GeoLists[LevelIDs_RedMountain * 8 + 3] = HillTopLands[2]->getlandtable();
}

void FreeHillTopLandTables() {
	FreeLandTableFile(&HillTopLands[0]);
	FreeLandTableFile(&HillTopLands[1]);
	FreeLandTableFile(&HillTopLands[2]);
}

void __cdecl UnloadHillTopFiles() {
	FreeHillTopLandTables();
	FreeLavaLandTables();
	FreeObjectFiles();
	Boss_FreeAssets();
}

void __cdecl LoadHillTopFiles() {
	PrintDebug("[Hill Top] Loading level files...\n");
	LoadHillTopLandTables();
	LoadLavaLandTables();
	LoadObjectFiles();
	Boss_LoadAssets();

	LevelDestructor = UnloadHillTopFiles;
}

__declspec(naked) void HookLoadLevelFilesRM() {
	__asm {
		call LoadHillTopFiles
		push 004237B3h
		ret
	}
}

void __cdecl RunLevelDestructor_r(int mode) {
	if (mode != 5 && LevelDestructor != LevelDestructor_MissionMode) {
		if (mode == 0) {
			ReleaseSetFile();
			ReleaseCamFile();
		}

		if (LevelDestructor) {
			LevelDestructor();
			LevelDestructor = nullptr;
		}
	}
}

void Level_Init(const HelperFunctions& helperFunctions, const IniFile* config) {
	// Replace the Red Mountain switch case from LoadLevelFiles to use our own set/cam/level files
	// This effectively removes what Dreamcast Conversion does in it
	WriteJump((void*)0x422D0A, HookLoadLevelFilesRM);
	
	// Fix an obvious error in RunLevelDestructor (OR instead of AND)
	// Vanilla levels don't use the level destructor in SADX PC since it doesn't load levels externally so it doesn't crash.
	WriteJump(RunLevelDestructor, RunLevelDestructor_r);

	// Paths
	helperFunctions.RegisterPathList(hilltop0_pathdata);
	helperFunctions.RegisterPathList(hilltop1_pathdata);
	helperFunctions.RegisterPathList(hilltop2_pathdata);

	// Start positions
	helperFunctions.RegisterStartPosition(Characters_Sonic, StartPoses[0]);
	helperFunctions.RegisterStartPosition(Characters_Sonic, StartPoses[1]);
	helperFunctions.RegisterStartPosition(Characters_Gamma, StartPoses[2]);
	helperFunctions.RegisterStartPosition(Characters_Knuckles, StartPoses[3]);
	helperFunctions.RegisterStartPosition(Characters_Sonic, StartPoses[4]);

	// Level Handler
	LevelObjects[LevelIDs_RedMountain] = HillTopZone_Init;

	// Deathzones
	DeathZoneList[LevelIDs_RedMountain][0] = hilltope0_deathzones;
	DeathZoneList[LevelIDs_RedMountain][1] = hilltope1_deathzones;
	DeathZoneList[LevelIDs_RedMountain][2] = hilltope2_deathzones;
	DeathZoneList[LevelIDs_RedMountain][3] = hilltope0_deathzones;

	// Music
	if (helperFunctions.Version >= 9) {
		MusicIDs_hilltop1 = helperFunctions.RegisterMusicFile(Music_HillTop1);
		MusicIDs_hilltop2 = helperFunctions.RegisterMusicFile(Music_HillTop2);
	}
	else {
		MusicList[MusicIDs_redmntn1] = Music_HillTop1;
		MusicList[MusicIDs_redmntn2] = Music_HillTop2;
	}
	
	// Sky color
	GlobalColorsLevel[LevelIDs_RedMountain] = { 0xFF1844FF, 0xFF2149FF, 0xFF002EFF };

	// Events
	HookRedMountainEvent();

	// Fix skybox transparency
	WriteJump(LoadSkyboxObject, LoadSkyboxObject_r);

	// Read config
	CFG_NoBoss = config->getBool("Boss", "NoBoss", false);
}