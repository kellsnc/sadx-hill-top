#include "pch.h"

void Level_Init(const HelperFunctions& helperFunctions);
void Objects_Init(const HelperFunctions& helperFunctions);

HelperFunctions HelperFunctionsGlobal;

extern "C"
{
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions)
	{
		HelperFunctionsGlobal = helperFunctions;
		Level_Init(helperFunctions);
		Objects_Init(helperFunctions);
	}

	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}