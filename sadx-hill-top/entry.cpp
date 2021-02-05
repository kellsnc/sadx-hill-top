#include "pch.h"

void Level_Init(const HelperFunctions& helperFunctions);
void Objects_Init(const HelperFunctions& helperFunctions);
void Cameras_Init(const HelperFunctions& helperFunctions);
void Boss_Init(const HelperFunctions& helperFunctions);
void Menu_Init();

HelperFunctions HelperFunctionsGlobal;

extern "C"
{
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions)
	{
		HelperFunctionsGlobal = helperFunctions;
		Level_Init(helperFunctions);
		Objects_Init(helperFunctions);
		Cameras_Init(helperFunctions);
		Boss_Init(helperFunctions);
		Menu_Init();
	}

	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}