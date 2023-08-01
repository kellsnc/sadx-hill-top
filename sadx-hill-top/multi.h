#pragma once

bool IsMultiplayerActive();
void SetMultiplayerWinner(int pnum);
void CameraSetEventCameraM(uint32_t ssCameraMode, uint32_t ucAdjustType);
void CameraSetEventCameraFuncM(CamFuncPtr fnCamera, uint32_t ucAdjustType, uint32_t scCameraDirect);
void Multi_Init(const HelperFunctions& helperFunctions);
