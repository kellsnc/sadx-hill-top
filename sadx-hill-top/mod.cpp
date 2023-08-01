#include "pch.h"
#include "mod.h"
#include "multi.h"

void Level_Init(const HelperFunctions& helperFunctions, const IniFile* config);
void Objects_Init(const HelperFunctions& helperFunctions);
void Cameras_Init(const HelperFunctions& helperFunctions);
void Boss_Init(const HelperFunctions& helperFunctions, const IniFile* config);
void Graphics_Init(const char* path, const HelperFunctions& helperFunctions);

const HelperFunctions* gHelperFunctions;

extern "C"
{
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions)
	{
		const IniFile* config = new IniFile(std::string(path) + "\\config.ini");
		gHelperFunctions = &helperFunctions;

		Multi_Init(helperFunctions);
		Level_Init(helperFunctions, config);
		Objects_Init(helperFunctions);
		Cameras_Init(helperFunctions);
		Boss_Init(helperFunctions, config);
		Graphics_Init(path, helperFunctions);

		delete config;
	}

	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}