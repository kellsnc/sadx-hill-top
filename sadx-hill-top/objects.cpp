#include "pch.h"

#include "objects/o_htskybox.h"
#include "objects/o_hilltree.h"
#include "objects/o_transporter.h"
#include "objects/o_hillflowers.h"
#include "objects/o_lavafall.h"
#include "objects/o_efflava.h"
#include "objects/o_hillgrass.h"
#include "objects/o_lantern.h"
#include "objects/o_hillbush.h"
#include "objects/o_pickrock.h"
#include "objects/o_seesaw.h"
#include "objects/o_hillpole.h"
#include "objects/o_vinepulley.h"
#include "objects/o_hillplatform.h"
#include "objects/o_growlava.h"

#include "enemies/e_spiker.h"
#include "enemies/e_rexon.h"
#include "enemies/e_fireball.h"
#include "enemies/e_epsilon.h"

static constexpr int MinDistance = 460000;

NJS_TEXNAME HillTopOBJ_TexNames[13];
NJS_TEXLIST HillTopOBJ_TexList = { arrayptrandlength(HillTopOBJ_TexNames) };

ObjectListEntry HillTopObjectList_list[] = {
	{ LoadObj_Data1, 3, 0, 360000, 0, Ring_Main, "RING   " },
	{ LoadObj_Data1, 2, 0, 360000, 0, Spring_Main, "SPRING " },
	{ LoadObj_Data1, 2, 0, 360000, 0, SpringB_Main, "SPRINGB" },
	{ LoadObj_Data2 | LoadObj_Data1, 3, 0, 360000, 0, DashPanel_Main, "O AXPNL" },
	{ LoadObj_Data1 | LoadObj_UnknownA, 3, 0, 360000, 0, SwingSpikeBall_Load, "O IRONB" },
	{ LoadObj_Data1, 3, 0, 360000, 0, FallingSpikeBall_Load, "O FeBJG" },
	{ LoadObj_Data1, 3, 0, 360000, 0, Spikes_Main, "O TOGE" },
	{ LoadObj_Data2 | LoadObj_Data1, 3, 0, 360000, 0, EmeraldPiece_Load, "O EME P" },
	{ LoadObj_Data1, 3, 0, 360000, 0, Capsule_Load, "O RELEASE" },
	{ LoadObj_Data1 | LoadObj_UnknownA, 3, 0, 360000, 0, Switch_Main, "O SWITCH" },
	{ LoadObj_Data1 | LoadObj_UnknownB, 3, 0, 160000, 0, Weed_Main, "CMN KUSA" },
	{ LoadObj_Data1 | LoadObj_UnknownA | LoadObj_UnknownB, 3, 0, 360000, 0, DashHoop_Main, "DashHoop" },
	{ LoadObj_Data1, 3, 0, 360000, 0, Balloon_Main, "O BALOON" },
	{ LoadObj_Data1, 3, 0, 160000, 0, ItemBox_Main, "O ITEMBOX" },
	{ LoadObj_Data1 | LoadObj_UnknownA | LoadObj_UnknownB, 2, 0, 360000, 0, RocketH_Main, "Rocket H" },
	{ LoadObj_Data1 | LoadObj_UnknownA | LoadObj_UnknownB, 2, 0, 360000, 0, RocketHS_Main, "Rocket HS" },
	{ LoadObj_Data1 | LoadObj_UnknownA | LoadObj_UnknownB, 2, 0, 360000, 0, RocketV_Main, "Rocket V" },
	{ LoadObj_Data1 | LoadObj_UnknownA | LoadObj_UnknownB, 2, 0, 360000, 0, RocketVS_Main, "Rocket VS" },
	{ LoadObj_Data1, 2, 1, 30000000, 0, JumpPanel_Load, "O JPanel" },
	{ LoadObj_Data2 | LoadObj_Data1 | LoadObj_UnknownA | LoadObj_UnknownB, 6, 0, 360000, 0, CheckPoint_Main, "O Save Point" },
	{ LoadObj_Data1, 3, 0, 0, 0, Wall_Main, "WALL   " },
	{ LoadObj_Data1, 3, 1, 1000000, 0, ODynamite, "O DYNAMITE" },
	{ LoadObj_Data1, 2, 0, 0, 0, CSphere, "C SPHERE" },
	{ LoadObj_Data1, 2, 0, 0, 0, ColCylinder_Main, "C CYLINDER" },
	{ LoadObj_Data1, 2, 0, 0, 0, ColCube_Main, "C CUBE" },
	{ LoadObj_Data1, 2, 0, 0, 0, Ottotto_Main, "OTTOTTO" },
	{ LoadObj_Data1, 2, 0, 0, 0, TikalHint_Load, "O TIKAL" },
	{ LoadObj_Data2 | LoadObj_Data1 | LoadObj_UnknownA | LoadObj_UnknownB, 4, 0, 0, 0, ItemBoxAir_Main, "O ItemBoxAir" },
	{ LoadObj_Data1, 3, 5, 250000, 0, Kiki_Load, "E SARU  " },
	{ LoadObj_Data1, 3, 4, 0, 0, SpinnerA_Main, "SPINA A" },
	{ LoadObj_Data1, 3, 4, 0, 0, SpinnerB_Main, "SPINA B" },
	{ LoadObj_Data1, 3, 4, 0, 0, SpinnerC_Main, "SPINA C" },
	{ LoadObj_Data1, 3, 5, 250000, 0, UnidusA_Main, "E UNI A" },
	{ LoadObj_Data1, 3, 5, 250000, 0, UnidusB_Main, "E UNI B" },
	{ LoadObj_Data1, 3, 5, 250000, 0, UnidusC_Main, "E UNI C" }, // 34
	{ LoadObj_Data1, 3, 1, 4000000, 0, E104Enemy_Main, "E E_104" },
	{ LoadObj_Data1, 3, 4, 0, 0, Spiker, "SPIKER" },
	{ LoadObj_Data1, 3, 4, 0, 0, Rexon, "REXON" },
	{ LoadObj_Data1 | LoadObj_UnknownA, 3, 1, 640000, 0, OTuri1_Main, "O TURI 1" }, // 38
	{ LoadObj_Data1 | LoadObj_UnknownA, 3, 1, 638401, 0, OZako_Main, "O ZAKO" },
	{ LoadObj_Data1 | LoadObj_UnknownA, 3, 1, 638401, 0, OZako_b_Main, "O ZAKO B" },
	{ LoadObj_Data1, 3, 1, MinDistance, 0, OSaku_Main, "O SAKU" },
	{ LoadObj_Data1, 3, 1, 10000, 0, OBlow_Main, "O BLOW" },
	{ LoadObj_Data1, 3, 1, 10000, 0, OBlow2_Main, "O BLOW2" },
	{ LoadObj_Data1, 3, 1, 638401, 0, HillTree, "O HILLLTREE" },
	{ LoadObj_Data1, 3, 1, 1000000, 0, HillTransporter, "O TRANSPORTER" },
	{ LoadObj_Data1, 4, 1, MinDistance, 0, (ObjectFuncPtr)HillFlowers, "O HILLFLOWERS" },
	{ LoadObj_Data1, 3, 1, MinDistance, 0, LavaFall, "O LAVAFALL" },
	{ LoadObj_Data1, 4, 1, MinDistance, 0, (ObjectFuncPtr)LavaEffect, "O EFFLAVA" },
	{ LoadObj_Data1, 3, 1, MinDistance, 0, HillGrass, "O HILLGRASS" },
	{ LoadObj_Data1, 3, 1, MinDistance, 0, HillLantern, "O HILLLANTERN" },
	{ LoadObj_Data1, 4, 1, MinDistance, 0, (ObjectFuncPtr)HillBush, "O HILLBUSH" },
	{ LoadObj_Data2 | LoadObj_Data1, 3, 1, MinDistance, 0, (ObjectFuncPtr)PickRock, "O PICKROCK" },
	{ LoadObj_Data1, 3, 0, 0, 0, FireBallLauncher, "E FIREBALL" },
	{ LoadObj_Data1, 3, 1, MinDistance, 0, HillSeesaw, "O HILLSEESAW" },
	{ LoadObj_Data1, 3, 1, 640000, 0, HillPole, "O HILLPOLE" },
	{ LoadObj_Data1, 3, 1, 640000, 0, VinePulley, "O VINEPULLEY" },
	{ LoadObj_Data1, 3, 1, 1280000, 0, HillPlatform, "O HILLPLATFORM" },
	{ LoadObj_Data1, 3, 1, MinDistance, 0, RingGroup_Main, "O_GRING" },
	{ LoadObj_Data1, 3, 1, 5280000, 0, GrowLava, "O GROWLAVA" },
	{ LoadObj_Data1, 3, 1, 1280000, 0, GrowLavaPlatform, "O GRAWLAVAPLATFORM" },
	{ LoadObj_Data1, 2, 0, 0, 0, GrowLavaTrigger, "O GRAWLAVATRIGGER" },
	{ LoadObj_Data1, 2, 0, 0, 0, KillCeiling, "O KILLCEILING" }
};

ObjectList HillTopObjectList = { arraylengthandptrT(HillTopObjectList_list, int) };

PVMEntry HillTopObjectTextures[] = {
	{ "OBJ_MOUNTAIN", &OBJ_MOUNTAIN_TEXLIST },
	{ "OBJ_HILLTOP", &HillTopOBJ_TexList },
	{ "BG_HILLTOP", &HillTopBG_TexList },
	{ "LAVAFALL", &LAVAFALL_TexList },
	{ "YOUGAN_ANIM", &YOUGAN_ANIM_TEXLIST },
	{ "EC_LIGHT", (NJS_TEXLIST*)0x2BF4F2C },
	{ "SPIKER", &SPIKER_TexList },
	{ "REXON", &REXON_TexList },
	{ "E_BOMB", &E_BOMB_TEXLIST },
	{ "E_SARU", &E_SARU_TEXLIST },
	{ "SUPI_SUPI", &SUPI_SUPI_TEXLIST },
	{ "UNI_A_UNIBODY", &UNI_A_UNIBODY_TEXLIST },
	{ "UNI_C_UNIBODY", &UNI_C_UNIBODY_TEXLIST },
	{ "TOGEBALL_TOGEBALL", &TOGEBALL_TOGEBALL_TEXLIST },
	{ "MOUNTAIN_E104", &MOUNTAIN_E104_TEXLIST },
	{ "MOUNTAIN_MEXPLOSION", (TexList*)0x2498810 },
	{ NULL, &b32ascii_TEXLIST },
	{ "CHAOS_LIFEGAUGE", &CHAOS_LIFEGAUGE_TEXLIST },
	{ "OUM", &OUM_TEXLIST },
	{ "GORI", &GORI_TEXLIST },
	{ "LION", &LION_TEXLIST },
	{ "MOGU", &MOGU_TEXLIST },
	{ "ZOU", &ZOU_TEXLIST },
	{ "TUBA", &TUBA_TEXLIST },

	{ 0 }
};

void LoadObjectFiles()
{
	LoadSetFile(0, "HT00");
	LoadSetFile(1, "HT01");
	LoadSetFile(2, "HT02");
	LoadSetFile(3, "HT03"); // custom act

	LoadCamFile(0, "HT00");
	LoadCamFile(1, "HT01");
	LoadCamFile(2, "HT02");
	LoadCamFile(3, "HT03"); // custom act

	SkyBox_LoadAssets();
	HillTree_LoadAssets();
	HillTransporter_LoadAssets();
	HillFlowers_LoadAssets();
	LavaFall_LoadAssets();
	HillGrass_LoadAssets();
	Lantern_LoadAssets();
	HillBush_LoadAssets();
	PickRock_LoadAssets();
	HillSeesaw_LoadAssets();
	HillPlatform_LoadAssets();
	GrowLava_LoadAssets();

	Spiker_LoadAssets();
	Rexon_LoadAssets();
	FireBall_LoadAssets();
}

void FreeObjectFiles()
{
	SkyBox_FreeAssets();
	HillTree_FreeAssets();
	HillTransporter_FreeAssets();
	HillFlowers_FreeAssets();
	LavaFall_FreeAssets();
	HillGrass_FreeAssets();
	Lantern_FreeAssets();
	HillBush_FreeAssets();
	PickRocket_FreeAssets();
	HillSeesaw_FreeAssets();
	HillPlatform_FreeAssets();
	GrowLava_FreeAssets();

	Spiker_FreeAssets();
	Rexon_FreeAssets();
	FireBall_FreeAssets();
}

void Objects_Init(const HelperFunctions& helperFunctions)
{
	// Add a new act to Red Mountain set/cam files (originally 0x210)
	SetDataThings[LevelIDs_RedMountain] = 0x3210;
	CamDataThings[LevelIDs_RedMountain] = 0x3210;

	// Compatibility with DC Conversion
	helperFunctions.ReplaceFile("system\\SET0500S_DC.bin", "system\\SETHT00S.bin");
	helperFunctions.ReplaceFile("system\\SET0501S_DC.bin", "system\\SETHT01S.bin");
	helperFunctions.ReplaceFile("system\\SET0501E_DC.bin", "system\\SETHT01E.bin");
	helperFunctions.ReplaceFile("system\\SET0502K_DC.bin", "system\\SETHT02K.bin");

	TexLists_Obj[LevelIDs_RedMountain] = HillTopObjectTextures;
	ObjLists[LevelIDs_RedMountain * 8] = &HillTopObjectList;
	ObjLists[LevelIDs_RedMountain * 8 + 1] = &HillTopObjectList;
	ObjLists[LevelIDs_RedMountain * 8 + 2] = &HillTopObjectList;
	ObjLists[LevelIDs_RedMountain * 8 + 3] = &HillTopObjectList; // custom act

	SkyboxObjects[LevelIDs_RedMountain] = HillTopZone_SkyBox;

	// Adjust E104 fight positions
	EpsilonBoss_Init();
}