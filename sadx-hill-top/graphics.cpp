#include "pch.h"
#include "SADXModLoader.h"

#define ReplaceTex(pvm, pvr, folder, pngname, gbix, x, y) helperFunctions.ReplaceTexture(pvm, pvr, (std::string(path) + "\\textures\\" folder "\\" pngname ".png").c_str(), gbix, x, y);
#define ReplacePVR(a, b) helperFunctions.ReplaceFile("system\\" a ".PVR", b);

void Load_StageNames(const char* path, const HelperFunctions& helperFunctions, bool hd)
{
	if (hd)
	{
		ReplaceTex("AVA_STNAM", "TX_ST10", "StageNames", "HD_HillTop", 365040, 128, 16);
		ReplaceTex("AVA_STNAM_E", "TX_ST10_e", "StageNames", "HD_HillTop_E", 10000418, 128, 16);
	}
	else
	{
		ReplaceTex("AVA_STNAM", "TX_ST10", "StageNames", "SD_HillTop", 365040, 128, 16);
		ReplaceTex("AVA_STNAM_E", "TX_ST10_e", "StageNames", "SD_HillTop_E", 10000418, 128, 16);
	}
}

void Load_TitleCards(const char* path, const HelperFunctions& helperFunctions, bool hd)
{
	std::string index = path;
	if (hd)
	{
		index += "\\textures\\TitleCards_HD\\index.txt";
		const char* hd_index = index.c_str();
		ReplacePVR("S_STAGE07", hd_index);
		ReplacePVR("S_STAGE07_E", hd_index);
		ReplacePVR("K_STAGE03", hd_index);
		ReplacePVR("K_STAGE03_E", hd_index);
		ReplacePVR("E_STAGE04", hd_index);
		ReplacePVR("E_STAGE04_E", hd_index);
	}
	else
	{
		index += "\\textures\\TitleCards_SD\\index.txt";
		const char* sd_index = index.c_str();
		ReplacePVR("S_STAGE07", sd_index);
		ReplacePVR("S_STAGE07_E", sd_index);
		ReplacePVR("K_STAGE03", sd_index);
		ReplacePVR("K_STAGE03_E", sd_index);
		ReplacePVR("E_STAGE04", sd_index);
		ReplacePVR("E_STAGE04_E", sd_index);
		ReplacePVR("S_STAGE07_DC", sd_index);
		ReplacePVR("S_STAGE07_E_DC", sd_index);
		ReplacePVR("K_STAGE03_DC", sd_index);
		ReplacePVR("K_STAGE03_E_DC", sd_index);
		ReplacePVR("E_STAGE04_DC", sd_index);
		ReplacePVR("E_STAGE04_E_DC", sd_index);
	}
}

void Load_StageCredits(const char* path, const HelperFunctions& helperFunctions, bool hd, bool dc)
{	
	if (hd)
	{
		ReplaceTex("ENDBG_SONIC_1", "s_ev018", "StageCredits", "HD_HillTop-S00", 366017, 256, 256);
		ReplaceTex("ENDBG_SUPERSONIC_0", "ss_ev005", "StageCredits", "HD_HillTop-S01", 366153, 256, 256);
		ReplaceTex("ENDBG_TAILS_1", "t_ev011", "StageCredits", "HD_HillTop-T00", 366137, 256, 256);
		ReplaceTex("ENDBG_TAILS_1", "t_ev013", "StageCredits", "HD_HillTop-T01", 366139, 256, 256);
		ReplaceTex("ENDBG_KNUCKLES_0", "k_ev008", "StageCredits", "HD_HillTop-K00", 366057, 256, 256);
		ReplaceTex("ENDBG_KNUCKLES_0", "k_ev009", "StageCredits", "HD_HillTop-K01", 366058, 256, 256);
		ReplaceTex("ENDBG_E102_2", "e_ev020", "StageCredits", "HD_HillTop-E00", 366120, 256, 256);
		ReplaceTex("ENDBG_E102_2", "e_ev021", "StageCredits", "HD_HillTop-E01", 366121, 256, 256);
		ReplaceTex("ENDBG_E102_2", "e_ev022", "StageCredits", "HD_HillTop-E02", 366122, 256, 256);
	}

	else if (dc)
	{
		ReplaceTex("ENDBG_SONIC_1", "s_ev018", "StageCredits", "DC_HillTop-S00", 366017, 256, 256);
		ReplaceTex("ENDBG_SUPERSONIC_0", "ss_ev005", "StageCredits", "DC_HillTop-S01", 366153, 256, 256);
		ReplaceTex("ENDBG_TAILS_1", "t_ev011", "StageCredits", "DC_HillTop-T00", 366137, 256, 256);
		ReplaceTex("ENDBG_TAILS_1", "t_ev013", "StageCredits", "DC_HillTop-T01", 366139, 256, 256);
		ReplaceTex("ENDBG_KNUCKLES_0", "k_ev008", "StageCredits", "DC_HillTop-K00", 366057, 256, 256);
		ReplaceTex("ENDBG_KNUCKLES_0", "k_ev009", "StageCredits", "DC_HillTop-K01", 366058, 256, 256);
		ReplaceTex("ENDBG_E102_2", "e_ev020", "StageCredits", "DC_HillTop-E00", 366120, 256, 256);
		ReplaceTex("ENDBG_E102_2", "e_ev021", "StageCredits", "DC_HillTop-E01", 366121, 256, 256);
		ReplaceTex("ENDBG_E102_2", "e_ev022", "StageCredits", "DC_HillTop-E02", 366122, 256, 256);
	}

	else
	{
		ReplaceTex("ENDBG_SONIC_2", "sonic_21", "StageCredits", "DX_HillTop-S00", 366020, 512, 512);
		ReplaceTex("ENDBG_SONIC_2", "sonic_27", "StageCredits", "DX_HillTop-S01", 366026, 512, 512);
		ReplaceTex("ENDBG_TAILS_1", "tails_17", "StageCredits", "DX_HillTop-T00", 366143, 512, 512);
		ReplaceTex("ENDBG_KNUCKLES_1", "knuckles_11", "StageCredits", "DX_HillTop-K00", 366060, 512, 512);
		ReplaceTex("ENDBG_E102_1", "e_18", "StageCredits", "DX_HillTop-E00", 366118, 512, 512);
		ReplaceTex("ENDBG_E102_1", "e_19", "StageCredits", "DX_HillTop-E01", 366119, 512, 512);
	}
}

void Load_StageThumbnails(const char* path, const HelperFunctions& helperFunctions)
{
	ReplaceTex("AVA_MULTI", "multistg0500", "StageThumbnails", "Multi_HillTopS", 54926006, 128, 72);
	ReplaceTex("AVA_MULTI", "multistg0502", "StageThumbnails", "Multi_HillTopK", 71757013, 128, 72);
	ReplaceTex("AVA_MULTI", "multistg0501E", "StageThumbnails", "Multi_HillTopE", 71757029, 128, 72);
}

void Graphics_Init(const char* path, const HelperFunctions& helperFunctions)
{
	bool hd = helperFunctions.Mods->find("sadx-hd-gui") != nullptr;
	bool dc = helperFunctions.Mods->find("sadx-dreamcast-conversion") != nullptr;
	Load_StageNames(path, helperFunctions, hd);
    Load_TitleCards(path, helperFunctions, hd);
	Load_StageCredits(path, helperFunctions, hd, dc);
	Load_StageThumbnails(path, helperFunctions);
}