#include "pch.h"

// 968.0f, 1309.0f, 2446.0f
// 1215.0f, 1239.0f, 3120.0f
// 33.0f 266.0f 1360.0f
// 65, 277, 1481   0xB000 0xA500

extern StartPosition StartPoses[4];

void EV_RedMountainChaseEvt_r(EVTSTATES state) {
	switch (state) {
	case EVTSTATES::Init:
		*(int*)0x3C83A08 = 0;
		break;
	case EVTSTATES::Main:
		RedMountain_SetViewData(ClipLevel);
		EV_InitCamera();
		DisableControl();
		EV_SetCurrentEvt();
		EV_AllocMem(4);
		LoadSoundList(78);
		
		// Load Egg Carrier model & animation
		EV_LoadObject(-3857.7, 2400.6001, -2000.0, 0, 0xA000, 0, 0.1, 0.1, 0.1, (NJS_OBJECT*)0x24960A8, (NJS_TEXLIST*)0x2BF4F2C, 0);
		EV_LoadMotion(0, (NJS_ACTION*)0x24983CC, 0);
		EV_SetObjectThing(0, 0.0f, 1.0f, 0xFFFFFFFF);

		// Move egg carrier from pos to other pos in 500 frames, this is done in a "Control Object"
		EV_MoveObjectFromTo(EV_GetObject(0), 886.0f, 518.0f, -289.0f, 1721.0F, 600.0F, 476.0f, 500);
		
		// Egg Carrier sound
		EV_PlaySoundQueue(1, 1344, 1800);
		sub_64FD80(1, 106, 1);
		
		EventWait(1);

		EV_SetPlayerPos(j_GetCharacterObject(0), StartPoses[0].Position.x, StartPoses[0].Position.y, StartPoses[0].Position.z);
		EV_RotatePlayer(j_GetCharacterObject(0), 0, StartPoses[0].YRot, 0);

		// Start position of the camera
		EV_MoveCamera(1, 0, 206, 122, 352.0f);
		EV_RotateCamera(1, 0, -0x1000, 0x1000, 0);

		// Move until frame 400 to destination
		EV_MoveCamera(1, 400, 498.0F, 122.0F, 215.0F);
		EV_RotateCamera(1, 400, 0x1000, 0xC725, 0);

		EventWait(420);

		break;
	case EVTSTATES::Free:
		EV_FreeCamera();
		EnableControl();
		EV_FreeMem();
		EV_ResetPlayer(0);
		EV_FreeControlObjects();
		EV_FreeObject(0);
		EV_SetPlayerPos(j_GetCharacterObject(0), StartPoses[0].Position.x, StartPoses[0].Position.y, StartPoses[0].Position.z);
		EV_RotatePlayer(j_GetCharacterObject(0), 0, StartPoses[0].YRot, 0);
		EventWait(1);
		break;
	}
}

void HookRedMountainEvent() {
	WriteJump((void*)0x64FE70, EV_RedMountainChaseEvt_r);
}