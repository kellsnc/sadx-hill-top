#include "pch.h"

#include "paths.h"
#include "deathzones.h"
#include "event.h"
#include "lava.h"

#include "enemies/e_subeggman.h"

static bool CFG_NoBoss = false;

static int MusicIDs_hilltop1 = MusicIDs_redmntn1;
static int MusicIDs_hilltop2 = MusicIDs_redmntn2;

MusicInfo Music_HillTop1 = { "hilltop1", true };
MusicInfo Music_HillTop2 = { "hilltop2", true };

NJS_TEXNAME HillTop_TexNames[8];
NJS_TEXLIST HillTop_TexList = { arrayptrandlength(HillTop_TexNames) };

LandTableInfo* HillTopLands[3] = { nullptr };

StartPosition StartPoses[] = {
	{ LevelIDs_RedMountain, 0, { 0.0f, 0.0f, 0.0f }, 0x8000 }, // Sonic Act 1
	{ LevelIDs_RedMountain, 1, { -1018.0f, -50.0f, -1235.0f }, 0 }, // Sonic Act 2
	{ LevelIDs_RedMountain, 1, { 15.0f, 700.0f, 225.0f }, 0 }, // Gamma Act 2
	{ LevelIDs_RedMountain, 2, { 895.0f, 295.0f, 515.0f }, 0 },  // Knuckles Act 3
	{ LevelIDs_RedMountain, 3, { 0.0f, -20.0f, 0.0f }, 0 }  // Sonic Act 4 (boss)
};

___stcFog HillTopFogData[] = {
	{ 167.0f, 6000.0f, 0x80FFFFFF, 1 },
	{ 167.0f, 6000.0f, 0x80FFFFFF, 1 },
	{ 167.0f, 6000.0f, 0x80FFFFFF, 1 },
	{ 167.0f, 6000.0f, 0x80FFFFFF, 1 }
};

___stcClip HillTopSkyDrawDist[] = {
	{ -1.0f, -9500.0f },
	{ -1.0f, -5000.0f },
	{ -1.0f, -2000.0f }
};

___stcClip HillTopDrawDists[] = {
	{ -1.0f, -2500.0f },
	{ -1.0f, -1600.0f },
	{ -1.0f, -1000.0f }
};

static const NJS_POINT3 ACT0TRIGGER_POS = { 301.0f, 164.0f, 3145.0f };
static const NJS_POINT3 ACT1TRIGGER_POS = { 15.0f, 950.0f, 225.0f };
static const NJS_POINT3 ACT1END_POS = { -480.0f, 935.0f, 3030.0f };

void HillTop_SetViewData()
{
	gClipSky = HillTopSkyDrawDist[ClipLevel];
	gClipMap = HillTopDrawDists[ClipLevel];
	gFog = HillTopFogData[ssActNumber];
}

#pragma region Level Handler
void LoadCurrentActMusic()
{
	auto musictp = CreateElementalTask(LoadObj_Data1, LEV_1, mt_gdcontrol);
	musictp->twp->mode = ssActNumber == 1 ? MusicIDs_hilltop2 : MusicIDs_hilltop1; // music id
	musictp->twp->wtimer = 3; // wait time
}

void __cdecl HillTopZoneExec(task* tp)
{
	if (ssActNumber == 0)
	{
		// Act 1-2 swap
		if (CheckCollisionP_num((NJS_POINT3*)&ACT0TRIGGER_POS, 100.0f, 0))
		{
			ADX_Close();
			LandChangeStage(1);
			AddSetStage(1);
			AddCameraStage(1);
			AdvanceAct(1);
			SetPlayerInitialPosition(playertwp[0]);
			SetInputP(0, 24);
			HillTop_SetViewData();
			LoadCurrentActMusic();
		}
	}
	else if (ssActNumber == 1)
	{
		// Act 2-4 swap
		int player = CheckCollisionP((NJS_POINT3*)&ACT1TRIGGER_POS, 50.0f) - 1;

		if (player >= 0)
		{
			SetInputP(player, 24);

			// If level has been completed once, go to boss instead
			if (player == 0 && GetEventFlag(EventFlags_Sonic_RedMountainClear) == true && CFG_NoBoss == false)
			{
				LandChangeStage(2);
				AddSetStage(2);
				AddCameraStage(2);
				AdvanceAct(2);
				SetPlayerInitialPosition(playertwp[0]);
				HillTop_SetViewData();
				Boss_SubEggman_Init((ObjectMaster*)tp);
			}
			else
			{
				// Teleport to end of level that is further way
				ForcePlayerPos(player, (NJS_POINT3*)&ACT1END_POS);
				playertwp[player]->ang.y = 0x3E80;
				playerpwp[player]->spd = { 2.0f, 8.0f, 0.0f };
			}
		}
	}
}

void __cdecl HillTopZone_Init(task* tp)
{
	HillTop_SetViewData(); // Set fog, view distance, etc.
	LoadLavaManager(); // Load the object that handles animated lava geometry

	// If current act is Eggman boss, load that instead
	if (ssActNumber == 3)
	{
		tp->exec = (TaskFuncPtr)Boss_SubEggman_Init;
	}
	else
	{
		// This initializes the music once events are done
		LoadCurrentActMusic();

		// Main level function ran every frame, used mostly for act swaps.
		tp->exec = HillTopZoneExec;
	}
}
#pragma endregion

// Fix RM skybox draw order
void SetScrollTask_r()
{
	___njSetBackColor(BackColorList[ssStageNumber].c1, BackColorList[ssStageNumber].c2, BackColorList[ssStageNumber].c3);

	if (ScrollMasterList[ssStageNumber])
	{
		if (ssStageNumber == LevelIDs_RedMountain)
		{
			CreateElementalTask(LoadObj_Data1, LEV_2, ScrollMasterList[ssStageNumber]); // Put this in object index 2 to fix transparency issues
		}
		else
		{
			CreateElementalTask(LoadObj_Data1, LEV_1, ScrollMasterList[ssStageNumber]);
		}
	}
}

// Replace landtables
void LoadHillTopLandTables()
{
	LoadLandTableFile(&HillTopLands[0], "system\\hilltopzone0.sa1lvl", &HillTop_TexList);
	LoadLandTableFile(&HillTopLands[1], "system\\hilltopzone1.sa1lvl", &HillTop_TexList);
	LoadLandTableFile(&HillTopLands[2], "system\\hilltopzone3.sa1lvl", &HillTop_TexList);

	objLandTable[LevelIDs_RedMountain * 8] = (_OBJ_LANDTABLE*)HillTopLands[0]->getlandtable();
	objLandTable[LevelIDs_RedMountain * 8 + 1] = (_OBJ_LANDTABLE*)HillTopLands[1]->getlandtable();
	objLandTable[LevelIDs_RedMountain * 8 + 2] = (_OBJ_LANDTABLE*)HillTopLands[0]->getlandtable();
	objLandTable[LevelIDs_RedMountain * 8 + 3] = (_OBJ_LANDTABLE*)HillTopLands[2]->getlandtable();
}

void FreeHillTopLandTables()
{
	FreeFileInfo(&HillTopLands[0]);
	FreeFileInfo(&HillTopLands[1]);
	FreeFileInfo(&HillTopLands[2]);
}

void __cdecl UnloadHillTopFiles()
{
	FreeHillTopLandTables();
	FreeLavaLandTables();
	FreeObjectFiles();
	Boss_FreeAssets();
}

void __cdecl LoadHillTopFiles()
{
	PrintDebug("[Hill Top] Loading level files...\n");
	LoadHillTopLandTables();
	LoadLavaLandTables();
	LoadObjectFiles();
	Boss_LoadAssets();

	___epilogfunc = UnloadHillTopFiles;
}

__declspec(naked) void HookLoadLevelFilesRM()
{
	__asm
	{
		call LoadHillTopFiles
		push 004237B3h
		ret
	}
}

void __cdecl ReleaseModule_r(int mode)
{
	if (mode != 5 && ___epilogfunc != LevelDestructor_MissionMode)
	{
		if (mode == 0)
		{
			ReleaseSetFile();
			ReleaseCamFile();
		}

		if (___epilogfunc)
		{
			___epilogfunc();
			___epilogfunc = nullptr;
		}
	}
}

void Level_Init(const HelperFunctions& helperFunctions, const IniFile* config)
{
	// Replace the Red Mountain switch case from LoadLevelFiles to use our own set/cam/level files
	// This effectively removes what Dreamcast Conversion does in it
	WriteJump((void*)0x422D0A, HookLoadLevelFilesRM);

	// Fix an obvious error in ReleaseModule (OR instead of AND)
	// Vanilla levels don't use the level destructor in SADX PC since it doesn't load levels externally so it doesn't crash.
	WriteJump(ReleaseModule, ReleaseModule_r);
	
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
	RoundMasterList[LevelIDs_RedMountain] = HillTopZone_Init;

	// Deathzones
	KillingCollisionModelsListList[LevelIDs_RedMountain][0] = hilltope0_deathzones;
	KillingCollisionModelsListList[LevelIDs_RedMountain][1] = hilltope1_deathzones;
	KillingCollisionModelsListList[LevelIDs_RedMountain][2] = hilltope2_deathzones;
	KillingCollisionModelsListList[LevelIDs_RedMountain][3] = hilltope0_deathzones;

	// Music
	if (helperFunctions.Version >= 9)
	{
		MusicIDs_hilltop1 = helperFunctions.RegisterMusicFile(Music_HillTop1);
		MusicIDs_hilltop2 = helperFunctions.RegisterMusicFile(Music_HillTop2);
	}
	else
	{
		MusicList[MusicIDs_redmntn1] = Music_HillTop1;
		MusicList[MusicIDs_redmntn2] = Music_HillTop2;
	}

	// Sky color
	BackColorList[LevelIDs_RedMountain] = { 0xFF1844FF, 0xFF2149FF, 0xFF002EFF };

	// Events
	HookRedMountainEvent();

	// Fix skybox transparency
	WriteJump(SetScrollTask, SetScrollTask_r);

	// Read config
	CFG_NoBoss = config->getBool("Boss", "NoBoss", false);
}