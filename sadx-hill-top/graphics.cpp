#include "pch.h"
#include "SADXModLoader.h"

#define ReplaceTex(pvm, pvr, folder, pngname, gbix, x, y) helperFunctions.ReplaceTexture(pvm, pvr, (std::string(path) + "\\textures\\" folder "\\" pngname ".png").c_str(), gbix, x, y);
#define ReplacePVR(a, b) helperFunctions.ReplaceFile("system\\" a ".PVR", b);

void Load_StageNames(const char* path, const HelperFunctions& helperFunctions, bool hd)
{
	if (!hd)
	{
		ReplaceTex("AVA_STNAM", "TX_ST10", "StageNames", "SD_HillTop", 365040, 128, 16);
		ReplaceTex("AVA_STNAM_E", "TX_ST10_e", "StageNames", "SD_HillTop_E", 10000418, 128, 16);
	}
	else
	{
		ReplaceTex("AVA_STNAM", "TX_ST10", "StageNames", "HD_HillTop", 365040, 128, 16);
		ReplaceTex("AVA_STNAM_E", "TX_ST10_e", "StageNames", "HD_HillTop_E", 10000418, 128, 16);
	}
}

void Load_TitleCards(const char* path, const HelperFunctions& helperFunctions, bool hd)
{
	std::string index = path;
    if (!hd)
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
    else
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
}

void Graphics_Init(const char* path, const HelperFunctions& helperFunctions)
{
	bool hd = helperFunctions.Mods->find("sadx-hd-gui") != nullptr;
	Load_StageNames(path, helperFunctions, hd);
    Load_TitleCards(path, helperFunctions, hd);
}