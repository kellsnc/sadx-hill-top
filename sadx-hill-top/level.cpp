#include "pch.h"

#include "paths.h"
#include "deathzones.h"
#include "event.h"

#include "objects/o_lavafall.h"

Angle HT_WindDirection = 0;

NJS_TEXNAME HillTop_TexNames[5];
NJS_TEXLIST HillTop_TexList = { arrayptrandlength(HillTop_TexNames) };

LandTable* LavaTables[3] = { nullptr };

StartPosition StartPoses[] = {
	{ LevelIDs_RedMountain, 0, { 0.0f, 0.0f, 0.0f }, 0x8000 }, // Sonic Act 1
	{ LevelIDs_RedMountain, 1, { 0.0f, 0.0f, 0.0f }, 0 }, // Sonic Act 2
	{ LevelIDs_RedMountain, 1, { 0.0f, 0.0f, 0.0f }, 0 }, // Gamma Act 2
	{ LevelIDs_RedMountain, 2, { 895.0f, 295.0f, 515.0f }, 0 }  // Knuckles Act 3
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

void __cdecl HillTopZone_Display(ObjectMaster* obj) {
	if (CurrentAct == 0) {
		njPushMatrixEx();
		Direct3D_SetNearFarPlanes(SkyboxDrawDistance.Minimum, SkyboxDrawDistance.Maximum);
		njSetTexture((NJS_TEXLIST*)0x2BF4F2C);
		njTranslate(_nj_current_matrix_ptr_, 1215.0f, 839.0f, 3520.0f);
		njRotateY_(0x8725);
		njScalef(0.1f);
		njAction((NJS_ACTION*)0x24983CC, obj->Data1->Scale.x); // Egg Carrier LOD Action
		Direct3D_SetNearFarPlanes(LevelDrawDistance.Minimum, LevelDrawDistance.Maximum);
		njPopMatrixEx();
	}
}

void __cdecl HillTopZone_Main(ObjectMaster* obj) {
	obj->Data1->Scale.x += 0.5f;

	if (obj->DisplaySub) {
		obj->DisplaySub(obj);
	}
}

void __cdecl HillTopZone_Init(ObjectMaster* obj) {
	// This initializes the music after the timer in InvulnerableTime is elpased 
	// and no event is running
	ObjectMaster* musicobj = LoadObject(LoadObj_Data1, 1, LoadMusic_EventDelayed);
	musicobj->Data1->Action = MusicIDs_redmntn1;
	musicobj->Data1->InvulnerableTime = 3;

	// Load the lava handler
	ObjectMaster* lavaobj = LoadObject(LoadObj_Data1, 1, HillTopLava_Main);
	lavaobj->DisplaySub = HillTopLava_Display;

	RedMountain_SetViewData();

	obj->MainSub = HillTopZone_Main;

	if (GetEventFlag(EventFlags_Sonic_RedMountainClear) == false && CurrentAct == 0) {
		obj->DisplaySub = HillTopZone_Display; // display the egg carrier if level never completed
	}
}

LandTable* LoadLandTable(const HelperFunctions& helperFunctions, const char* name) {
	LandTableInfo* info = new LandTableInfo(helperFunctions.GetReplaceablePath(name));

	info->getlandtable()->TexList = &HillTop_TexList;
	
	return info->getlandtable();
}

void Level_Init(const HelperFunctions& helperFunctions) {
	GeoLists[LevelIDs_RedMountain * 8] = LoadLandTable(helperFunctions, "system\\hilltopzone0.sa1lvl");
	GeoLists[LevelIDs_RedMountain * 8 + 1] = LoadLandTable(helperFunctions, "system\\hilltopzone1.sa1lvl");
	GeoLists[LevelIDs_RedMountain * 8 + 2] = GeoLists[LevelIDs_RedMountain * 8];

	LavaTables[0] = LoadLandTable(helperFunctions, "system\\hilltopzone0_lava.sa1lvl");
	LavaTables[1] = LoadLandTable(helperFunctions, "system\\hilltopzone1_lava.sa1lvl");
	LavaTables[2] = LavaTables[0];

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
	DeathZoneList[LevelIDs_RedMountain][2] = hilltope0_deathzones;

	FogData_RedMountain1[0].Layer = 167;
	FogData_RedMountain1[0].Distance = 6000;
	FogData_RedMountain1[0].Color = 0x80FFFFFF;
	FogData_RedMountain1[0].Toggle = 1;

	MusicList[MusicIDs_redmntn1].Name = "hilltop";

	HookRedMountainEvent();
}