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

NJS_TEXNAME HillTopOBJ_TexNames[9];
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
	{ LoadObj_Data1 | LoadObj_UnknownA | LoadObj_UnknownB, 3, 0, 360000, 0, DashHoop_Main, "CMN_DRING" },
	{ LoadObj_Data1, 3, 0, 360000, 0, Balloon_Main, "O BALOON" },
	{ LoadObj_Data1, 3, 0, 160000, 0, ItemBox_Main, "O ITEMBOX" },
	{ LoadObj_Data1 | LoadObj_UnknownA | LoadObj_UnknownB, 2, 0, 360000, 0, RocketH_Main, "Rocket H" },
	{ LoadObj_Data1 | LoadObj_UnknownA | LoadObj_UnknownB, 2, 0, 360000, 0, RocketHS_Main, "Rocket HS" },
	{ LoadObj_Data1 | LoadObj_UnknownA | LoadObj_UnknownB, 2, 0, 360000, 0, RocketV_Main, "Rocket V" },
	{ LoadObj_Data1 | LoadObj_UnknownA | LoadObj_UnknownB, 2, 0, 360000, 0, RocketVS_Main, "Rocket VS" },
	{ LoadObj_Data1, 2, 0, 4000000, 0, JumpPanel_Load, "O JPanel" },
	{ LoadObj_Data2 | LoadObj_Data1 | LoadObj_UnknownA | LoadObj_UnknownB, 6, 0, 360000, 0, CheckPoint_Main, "O Save Point" },
	{ LoadObj_Data1, 3, 0, 0, 0, Wall_Main, "WALL   " },
	{ LoadObj_Data1, 3, 1, 1000000, 0, ODynamite, "O DYNAMITE" },
	{ LoadObj_Data1, 2, 0, 0, 0, CSphere, "C SPHERE" },
	{ LoadObj_Data1, 2, 0, 0, 0, ColCylinder_Main, "C CYLINDER" },
	{ LoadObj_Data1, 2, 0, 0, 0, ColCube_Main, "C CUBE" },
	{ LoadObj_Data1, 2, 0, 0, 0, Ottotto_Main, "OTTOTTO" },
	{ LoadObj_Data1, 2, 0, 0, 0, TikalHint_Load, "O TIKAL" },
	{ LoadObj_Data2 | LoadObj_Data1 | LoadObj_UnknownA | LoadObj_UnknownB, 3, 0, 0, 0, ItemBoxAir_Main, "O ItemBoxAir" },
	
	{ LoadObj_Data1, 3, 5, 250000, 0, Kiki_Load, "E SARU  " },
	{ LoadObj_Data1, 3, 4, 0, 0, SpinnerA_Main, "SPINA A" },
	{ LoadObj_Data1, 3, 4, 0, 0, SpinnerB_Main, "SPINA B" },
	{ LoadObj_Data1, 3, 4, 0, 0, SpinnerC_Main, "SPINA C" },
	{ LoadObj_Data1, 3, 5, 250000, 0, UnidusA_Main, "E UNI A" },
	{ LoadObj_Data1, 3, 5, 250000, 0, UnidusB_Main, "E UNI B" },
	{ LoadObj_Data1, 3, 5, 250000, 0, UnidusC_Main, "E UNI C" },
	{ LoadObj_Data1, 3, 1, 4000000, 0, E104Enemy_Main, "E E_104" },
	{ LoadObj_Data1, 3, 4, 0, 0, nullptr, "SPIKER" },
	{ LoadObj_Data1, 3, 4, 0, 0, nullptr, "REXON" },

	{ LoadObj_Data1 | LoadObj_UnknownA, 3, 0, 0, 0, OErupt_Main, "O Erupt" },
	{ LoadObj_Data1, 3, 1, 638401, 0, OBreakstep_Main, "O BREAKSTEP" },
	{ LoadObj_Data1, 3, 1, 638401, 0, OBreakstep2_Main, "O BREAKSTEP2" },
	{ LoadObj_Data2 | LoadObj_Data1 | LoadObj_UnknownA, 3, 0, 0, 0, Magmacnt_Main, "MagmaCnt" },
	{ LoadObj_Data1, 3, 1, 10000, 0, OBlow_Main, "O BLOW" },
	{ LoadObj_Data1, 3, 1, 10000, 0, OBlow2_Main, "O BLOW2" },

	{ LoadObj_Data1, 3, 1, 638401, 0, HillTree, "O HILLLTREE" },
	{ LoadObj_Data1, 3, 1, 1000000, 0, HillBush, "O TRANSPORTER" },
	{ LoadObj_Data1, 4, 1, 130000, 0, HillFlowers, "O HILLFLOWERS" },
	{ LoadObj_Data1, 3, 1, 130000, 0, LavaFall, "O LAVAFALL" },
	{ LoadObj_Data1, 4, 1, 130000, 0, LavaEffect, "O EFFLAVA" },
	{ LoadObj_Data1, 3, 1, 130000, 0, HillGrass, "O HILLGRASS" },
	{ LoadObj_Data1, 3, 1, 230000, 0, HillLantern, "O HILLLANTERN" },
	{ LoadObj_Data1, 4, 1, 230000, 0, HillBush, "O HILLBUSH" }
};

ObjectList HillTopObjectList = { arraylengthandptrT(HillTopObjectList_list, int) };

PVMEntry HillTopObjectTextures[] = {
	{ "OBJ_MOUNTAIN", (TexList*)0x24208A8 },
	{ "OBJ_HILLTOP", &HillTopOBJ_TexList },
	{ "BG_HILLTOP", &HillTopBG_TexList },
	{ "LAVAFALL", &LAVAFALL_TexList },
	
	{ "E_BOMB", (TexList*)0x96B464 },
	{ "E_SARU", (TexList*)0x962560 },
	{ "SUPI_SUPI", (TexList*)0x96F518 },
	{ "UNI_C_UNIBODY", (TexList*)0x96DC48 },
	{ "TOGEBALL_TOGEBALL", (TexList*)0x96BC54 },
	{ "MOUNTAIN_E104", (TexList*)0x2499328 },

	{ "OUM", (TexList*)0x9334EC },
	{ "GORI", (TexList*)0x945964 },
	{ "LION", (TexList*)0x944094 },
	{ "MOGU", (TexList*)0x93ECEC },
	{ "ZOU", (TexList*)0x94160C },
	{ "TUBA", (TexList*)0x92F2BC },

	{ 0 }
};

void Objects_Init(const HelperFunctions& helperFunctions) {
	helperFunctions.ReplaceFile("system\\SET0500S.bin", "system\\SETHT00S.bin");
	helperFunctions.ReplaceFile("system\\SET0501S.bin", "system\\SETHT01S.bin");
	helperFunctions.ReplaceFile("system\\SET0501E.bin", "system\\SETHT01E.bin");
	helperFunctions.ReplaceFile("system\\SET0502K.bin", "system\\SETHT02K.bin");

	TexLists_Obj[LevelIDs_RedMountain] = HillTopObjectTextures;
	ObjLists[LevelIDs_RedMountain * 8] = &HillTopObjectList;
	ObjLists[LevelIDs_RedMountain * 8 + 1] = &HillTopObjectList;
	ObjLists[LevelIDs_RedMountain * 8 + 2] = &HillTopObjectList;

	SkyboxObjects[LevelIDs_RedMountain] = HillTopZone_SkyBox;

	LoadModel(&ht_skybox, "ht_skybox", ModelFormat_Basic);
	LoadModel(&ht_tree, "ht_tree", ModelFormat_Basic);
	LoadModel(&ht_transporter, "ht_transporter", ModelFormat_Basic);
	LoadModel(&ht_transportercol, "ht_transportercol", ModelFormat_Basic);
	LoadModel(&ht_vine, "ht_vine", ModelFormat_Chunk);

	HillFlowers_LoadAssets();
	LavaFall_LoadAssets();
	HillGrass_LoadAssets();
	Lantern_LoadAssets();
	HillBush_LoadAssets();
}