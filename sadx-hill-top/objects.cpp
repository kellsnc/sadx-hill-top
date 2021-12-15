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

NJS_TEXNAME HillTopOBJ_TexNames[14];
NJS_TEXLIST HillTopOBJ_TexList = { arrayptrandlength(HillTopOBJ_TexNames) };

_OBJ_ITEMENTRY HillTopObjectList_list[] = {
	{ IM_TWK, 3, 0, 360000.0f, 0, Ring, "RING   " },
	{ IM_TWK, 2, 0, 360000.0f, 0, ObjectSpring, "SPRING " },
	{ IM_TWK, 2, 0, 360000.0f, 0, ObjectSpringB, "SPRINGB" },
	{ IM_MWK | IM_TWK, 3, 0, 360000.0f, 0, ObjectAxelPanel, "O AXPNL" },
	{ IM_TWK | IM_FWK, 3, 0, 360000.0f, 0, ObjectIronBall, "O IRONB" },
	{ IM_TWK, 3, 0, 360000.0f, 0, ObjectIronBallJG, "O FeBJG" },
	{ IM_TWK, 3, 0, 360000.0f, 0, ObjectToge, "O TOGE" },
	{ IM_MWK | IM_TWK, 3, 0, 360000.0f, 0, ObjectEmeraldP, "O EME P" },
	{ IM_TWK, 3, 0, 360000.0f, 0, ObjectReleaseBox, "O RELEASE" },
	{ IM_TWK | IM_FWK, 3, 0, 360000.0f, 0, ObjectSwitch, "O SWITCH" },
	{ IM_TWK | IM_AWK, 3, 0, 160000.0f, 0, ObjectCmnKusa, "CMN KUSA" },
	{ IM_TWK | IM_FWK | IM_AWK, 3, 0, 360000.0f, 0, ObjectCmnDushRing, "CMN_DRING" },
	{ IM_TWK, 3, 0, 360000.0f, 0, ObjectBaloon, "O BALOON" },
	{ IM_TWK, 3, 0, 160000.0f, 0, ObjectItemBox, "O ITEMBOX" },
	{ IM_TWK | IM_FWK | IM_AWK, 2, 0, 360000.0f, 0, ObjectRocketBaseH, "Rocket H" },
	{ IM_TWK | IM_FWK | IM_AWK, 2, 0, 360000.0f, 0, ObjectRocketBaseHS, "Rocket HS" },
	{ IM_TWK | IM_FWK | IM_AWK, 2, 0, 360000.0f, 0, ObjectRocketBaseV, "Rocket V" },
	{ IM_TWK | IM_FWK | IM_AWK, 2, 0, 360000.0f, 0, ObjectRocketBaseVS, "Rocket VS" },
	{ IM_TWK, 2, 1, 30000000.0f, 0, ObjectJumpPanel, "O JPanel" },
	{ IM_MWK | IM_TWK | IM_FWK | IM_AWK, 6, 0, 360000.0f, 0, ObjectSavePoint, "O Save Point" },
	{ IM_TWK, 3, 0, 0.0f, 0, ColliWall, "WALL   " },
	{ IM_TWK, 3, 1, 1000000.0f, 0, Object_Dynamite, "O DYNAMITE" },
	{ IM_TWK, 2, 0, 0.0f, 0, ColliSphere, "C SPHERE" },
	{ IM_TWK, 2, 0, 0.0f, 0, ColliCylinder, "C CYLINDER" },
	{ IM_TWK, 2, 0, 0.0f, 0, ColliCube, "C CUBE" },
	{ IM_TWK, 2, 0, 0.0f, 0, ColliOttottoRange, "OTTOTTO" },
	{ IM_TWK, 2, 0, 0.0f, 0, ObjectTikalHint, "O TIKAL" },
	{ IM_MWK | IM_TWK | IM_FWK | IM_AWK, 4, 0, 0.0f, 0, ObjectItemboxAir, "O ItemBoxAir" },
	{ IM_TWK, 3, 5, 250000.0f, 0, EnemySaru, "E SARU  " },
	{ IM_TWK, 3, 4, 0.0f, 0, EnemySpinnaA, "SPINA A" },
	{ IM_TWK, 3, 4, 0.0f, 0, EnemySpinnaB, "SPINA B" },
	{ IM_TWK, 3, 4, 0.0f, 0, EnemySpinnaC, "SPINA C" },
	{ IM_TWK, 3, 5, 250000.0f, 0, EnemyUniA, "E UNI A" },
	{ IM_TWK, 3, 5, 250000.0f, 0, EnemyUniB, "E UNI B" },
	{ IM_TWK, 3, 5, 250000.0f, 0, EnemyUniC, "E UNI C" }, // 34
	{ IM_TWK, 3, 1, 4000000.0f, 0, EnemyMountainE104, "E E_104" },
	{ IM_TWK, 3, 4, 0.0f, 0, Spiker, "SPIKER" },
	{ IM_TWK, 3, 4, 0.0f, 0, Rexon, "REXON" },
	{ IM_TWK | IM_FWK, 3, 1, 640000.0f, 0, ObjectMountainTuribasi1, "O TURI 1" }, // 38
	{ IM_TWK | IM_FWK, 3, 1, 638400.0f, 0, Object_Mountain_Zako, "O ZAKO" },
	{ IM_TWK | IM_FWK, 3, 1, 638400.0f, 0, Object_Mountain_ZakoB, "O ZAKO B" },
	{ IM_TWK, 3, 1, 460000.0f, 0, ObjectMountainSaku, "O SAKU" },
	{ IM_TWK, 3, 1, 10000.0f, 0, Object_Mountain_Blow, "O BLOW" },
	{ IM_TWK, 3, 1, 10000.0f, 0, Object_Mountain_Blow2, "O BLOW2" },
	{ IM_TWK, 3, 1, 638400.0f, 0, HillTree, "O HILLLTREE" },
	{ IM_TWK | IM_AWK, 3, 1, 1000000.0f, 0, HillTransporter, "O TRANSPORTER" },
	{ IM_TWK, 4, 1, 460000.0f, 0, HillFlowers, "O HILLFLOWERS" },
	{ IM_TWK, 3, 1, 460000.0f, 0, LavaFall, "O LAVAFALL" },
	{ IM_TWK, 4, 1, 460000.0f, 0, LavaEffect, "O EFFLAVA" },
	{ IM_TWK, 3, 1, 460000.0f, 0, HillGrass, "O HILLGRASS" },
	{ IM_TWK, 3, 1, 460000.0f, 0, HillLantern, "O HILLLANTERN" },
	{ IM_TWK, 4, 1, 460000.0f, 0, HillBush, "O HILLBUSH" },
	{ IM_MWK | IM_TWK, 3, 1, 460000.0f, 0, PickRock, "O PICKROCK" },
	{ IM_TWK, 3, 0, 0.0f, 0, FireBallLauncher, "E FIREBALL" },
	{ IM_TWK, 3, 1, 460000.0f, 0, HillSeesaw, "O HILLSEESAW" },
	{ IM_TWK, 3, 1, 640000.0f, 0, HillPole, "O HILLPOLE" },
	{ IM_TWK, 3, 1, 640000.0f, 0, VinePulley, "O VINEPULLEY" },
	{ IM_TWK, 3, 1, 1280000.0f, 0, HillPlatform, "O HILLPLATFORM" },
	{ IM_TWK, 3, 1, 460000.0f, 0, ObjectGroupRing, "O_GRING" },
	{ IM_TWK, 3, 1, 5280000.0f, 0, GrowLava, "O GROWLAVA" },
	{ IM_TWK, 3, 1, 1280000.0f, 0, GrowLavaPlatform, "O GRAWLAVAPLATFORM" },
	{ IM_TWK, 2, 0, 0.0f, 0, GrowLavaTrigger, "O GRAWLAVATRIGGER" },
	{ IM_TWK, 2, 0, 0.0f, 0, KillCeiling, "O KILLCEILING" }
};

_OBJ_ITEMTABLE HillTopObjectList = { LengthOfArray(HillTopObjectList_list), 0, HillTopObjectList_list };

TEX_PVMTABLE HillTopObjectTextures[] = {
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
	{ "MOUNTAIN_MEXPLOSION", (NJS_TEXLIST*)0x2498810 },
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

	ListofPvmList[LevelIDs_RedMountain] = HillTopObjectTextures;
	objItemTable[LevelIDs_RedMountain * 8] = &HillTopObjectList;
	objItemTable[LevelIDs_RedMountain * 8 + 1] = &HillTopObjectList;
	objItemTable[LevelIDs_RedMountain * 8 + 2] = &HillTopObjectList;
	objItemTable[LevelIDs_RedMountain * 8 + 3] = &HillTopObjectList; // custom act

	ScrollMasterList[LevelIDs_RedMountain] = HillTopZone_SkyBox;

	// Adjust E104 fight positions
	EpsilonBoss_Init();
}