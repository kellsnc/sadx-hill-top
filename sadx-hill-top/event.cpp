#include "pch.h"

extern StartPosition StartPoses[4];
void HillTop_SetViewData();

void EV_RedMountainChaseEvt_r(EVTSTATES state)
{
	switch (state)
	{
	case EVTSTATES::Init:
		*(int*)0x3C83A08 = 0;
		HillTop_SetViewData();
		break;
	case EVTSTATES::Main:
		EV_CameraOn();
		PadReadOff();
		EV_CanselOn();
		EventSe_Init(4);
		LoadSoundList(78);

		// Load Egg Carrier model & animation
		createModelEC(-3857.7, 2400.6001, -2000.0, 0, 0xA000, 0, 0.1, 0.1, 0.1, (NJS_OBJECT*)0x24960A8, (NJS_TEXLIST*)0x2BF4F2C, 0);
		addmotModel(0, (NJS_ACTION*)0x24983CC, 0);
		playModel(0, 0.0f, 1.0f, 0xFFFFFFFF);

		// Move egg carrier from pos to other pos in 500 frames, this is done in a "Control Object"
		moveObject(getobjModel(0), 886.0f, 518.0f, -289.0f, 1721.0F, 600.0F, 476.0f, 500);

		// Egg Carrier sound
		EventSe_Play(1, 1344, 1800);
		EventSe_Volume(1, 106, 1);

		EV_Wait(1);

		EV_SetPos(EV_GetPlayer(0), StartPoses[0].Position.x, StartPoses[0].Position.y, StartPoses[0].Position.z);
		EV_SetAng(EV_GetPlayer(0), 0, StartPoses[0].YRot, 0);

		// Start position of the camera
		EV_CameraPos(1, 0, 206, 122, 352.0f);
		EV_CameraAng(1, 0, -0x1000, 0x1000, 0);

		// Move until frame 400 to destination
		EV_CameraPos(1, 400, 498.0F, 122.0F, 215.0F);
		EV_CameraAng(1, 400, 0x1000, 0xC725, 0);

		EV_Wait(420);

		break;
	case EVTSTATES::Free:
		EV_CameraOff();
		PadReadOn();
		EventSe_Close();
		EV_InitPlayer(0);
		stopObjectAll();
		//EV_FreeObject(0);
		EV_SetPos(EV_GetPlayer(0), StartPoses[0].Position.x, StartPoses[0].Position.y, StartPoses[0].Position.z);
		EV_SetAng(EV_GetPlayer(0), 0, StartPoses[0].YRot, 0);
		EV_Wait(1);
		break;
	}
}

void HookRedMountainEvent()
{
	WriteJump((void*)0x64FE70, EV_RedMountainChaseEvt_r);
}