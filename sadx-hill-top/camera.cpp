#include "pch.h"

void Cameras_Init(const HelperFunctions& helperFunctions)
{
	helperFunctions.ReplaceFile("system\\CAM0500S.bin", "system\\CAMHT00S.bin");
	helperFunctions.ReplaceFile("system\\CAM0501S.bin", "system\\CAMHT01S.bin");
	helperFunctions.ReplaceFile("system\\CAM0501E.bin", "system\\CAMHT01E.bin");
	helperFunctions.ReplaceFile("system\\CAM0502K.bin", "system\\CAMHT02K.bin");

	// Compatibility with DC Conversion
	helperFunctions.ReplaceFile("system\\CAM0500S_DC.bin", "system\\CAMHT00S.bin");
	helperFunctions.ReplaceFile("system\\CAM0501S_DC.bin", "system\\CAMHT01S.bin");
	helperFunctions.ReplaceFile("system\\CAM0501E_DC.bin", "system\\CAMHT01E.bin");
	helperFunctions.ReplaceFile("system\\CAM0502K_DC.bin", "system\\CAMHT02K.bin");
}