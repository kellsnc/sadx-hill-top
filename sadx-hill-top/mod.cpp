#include "pch.h"
#include "mod.h"

void Level_Init(const HelperFunctions& helperFunctions, const IniFile* config);
void Objects_Init(const HelperFunctions& helperFunctions);
void Cameras_Init(const HelperFunctions& helperFunctions);
void Boss_Init(const HelperFunctions& helperFunctions, const IniFile* config);
void Menu_Init();

const HelperFunctions* gHelperFunctions;

extern "C"
{
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions)
	{
		const IniFile* config = new IniFile(std::string(path) + "\\config.ini");

		gHelperFunctions = &helperFunctions;
		Level_Init(helperFunctions, config);
		Objects_Init(helperFunctions);
		Cameras_Init(helperFunctions);
		Boss_Init(helperFunctions, config);
		Menu_Init();
	}

	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}