#include "pch.h"
#include "SADXModLoader.h"
#include "multi.h"

bool(*multi_is_active)() = nullptr;
void(*camera_set_event_camera)(uint32_t pnum, uint32_t ssCameraMode, uint32_t ucAdjustType) = nullptr;
void(*camera_set_event_camera_func)(uint32_t pnum, CamFuncPtr fnCamera, uint32_t ucAdjustType, uint32_t scCameraDirect) = nullptr;

bool IsMultiplayerActive()
{
	return multi_is_active ? multi_is_active() : false;
}

void CameraSetEventCameraM(uint32_t ssCameraMode, uint32_t ucAdjustType)
{
	if (camera_set_event_camera)
	{
		for (int i = 0; i < 4; ++i)
		{
			camera_set_event_camera(i, ssCameraMode, ucAdjustType);
		}
	}
	else
	{
		CameraSetEventCamera(ssCameraMode, ucAdjustType);
	}
}

void CameraSetEventCameraFuncM(CamFuncPtr fnCamera, uint32_t ucAdjustType, uint32_t scCameraDirect)
{
	if (camera_set_event_camera_func)
	{
		for (int i = 0; i < 4; ++i)
		{
			camera_set_event_camera_func(i, fnCamera, ucAdjustType, scCameraDirect);
		}
	}
	else
	{
		CameraSetEventCameraFunc(fnCamera, ucAdjustType, scCameraDirect);
	}
}

void Multi_Init(const HelperFunctions& helperFunctions)
{
	if (helperFunctions.Version < 16)
	{
		PrintDebug("[HillTop] Mod Loader out of date, cannot perform multiplayer compatibility.\n");
		return;
	}

	auto multi_mod = (Mod*)helperFunctions.Mods->find("sadx-multiplayer");
	if (multi_mod)
	{
		multi_is_active = multi_mod->GetDllExport<decltype(multi_is_active)>("multi_is_active");
		camera_set_event_camera = multi_mod->GetDllExport<decltype(camera_set_event_camera)>("camera_set_event_camera");
		camera_set_event_camera_func = multi_mod->GetDllExport<decltype(camera_set_event_camera_func)>("camera_set_event_camera_func");
	}
}
