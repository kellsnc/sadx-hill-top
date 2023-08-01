#include "pch.h"

//  HD GUI Check:

bool HD_GUI = false;

void Check_HDGUI(const HelperFunctions& helperFunctions)
{
    HD_GUI = helperFunctions.Mods->find("sadx-hd-gui") != nullptr;
}


//  Stage Names:

#define ReplaceTex(pvm, pvr, folder, pngname, gbix, x, y) helperFunctions.ReplaceTexture(pvm, pvr, (std::string(path) + "\\textures\\" folder "\\" pngname ".png").c_str(), gbix, x, y);

void Load_StageNames(const char* path, const HelperFunctions& helperFunctions)
{
    if (!HD_GUI)
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


//	TitleCards:

#define ReplaceTitleCard_SD(a) _snprintf_s(pathbuf, LengthOfArray(pathbuf), "%s\\textures\\TitleCards_SD\\index.txt", path); \
        helperFunctions.ReplaceFile("system\\" a ".PVR", pathbuf);

#define ReplaceTitleCard_HD(a) _snprintf_s(pathbuf, LengthOfArray(pathbuf), "%s\\textures\\TitleCards_HD\\index.txt", path); \
        helperFunctions.ReplaceFile("system\\" a ".PVR", pathbuf);

void Load_TitleCards(const char* path, const HelperFunctions & helperFunctions)
{
	char pathbuf[MAX_PATH];

    if (!HD_GUI)
    {
        ReplaceTitleCard_SD("S_STAGE07");
        ReplaceTitleCard_SD("S_STAGE07_E");
        ReplaceTitleCard_SD("K_STAGE03");
        ReplaceTitleCard_SD("K_STAGE03_E");
        ReplaceTitleCard_SD("E_STAGE04");
        ReplaceTitleCard_SD("E_STAGE04_E");

        // These _DC titlecards are meant to replace the ones DC Conversion uses when you are NOT using HD GUI.
        ReplaceTitleCard_SD("S_STAGE07_DC");
        ReplaceTitleCard_SD("S_STAGE07_E_DC");
        ReplaceTitleCard_SD("K_STAGE03_DC");
        ReplaceTitleCard_SD("K_STAGE03_E_DC");
        ReplaceTitleCard_SD("E_STAGE04_DC");
        ReplaceTitleCard_SD("E_STAGE04_E_DC");
    }
    
    else
    {
        ReplaceTitleCard_HD("S_STAGE07");
        ReplaceTitleCard_HD("S_STAGE07_E");
        ReplaceTitleCard_HD("K_STAGE03");
        ReplaceTitleCard_HD("K_STAGE03_E");
        ReplaceTitleCard_HD("E_STAGE04");
        ReplaceTitleCard_HD("E_STAGE04_E");
    }
}


//  Init HD_GUI Check, StageNames and TitleCards:

void Graphics_Init(const char* path, const HelperFunctions& helperFunctions)
{
    Check_HDGUI(helperFunctions);
    Load_StageNames(path, helperFunctions);
    Load_TitleCards(path, helperFunctions);
}