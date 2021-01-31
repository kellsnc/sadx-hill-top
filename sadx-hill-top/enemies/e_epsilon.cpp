#include "pch.h"
#include "e_epsilon.h"

int __cdecl E104_CheckPlayer(ObjectMaster* obj) {
	EntityData1* player = GetCharacterObject(0)->Data1;
	EntityData1* data = obj->Data1; // eax

	float x = data->Position.x - player->Position.x;
	float z = data->Position.z - player->Position.z;

	return (z * z + x * x) < 22500.0f && player->Status & Status_Ground && player->Position.y > 960.0f;
}

void EpsilonBoss_Init() {
	// New fight boundaries
	GammaCamBounds[0] = { 867.7648f, 996.625f, -600.5748f };
	GammaCamBounds[1] = { 990.7285f, 996.625f, -368.2955f };
	GammaCamBounds[2] = { 965.4641f, 996.625f, -203.8346f };
	GammaCamBounds[3] = { 845.3178f, 996.625f, -312.3701f };
	GammaCamBounds[4] = { 698.6425f, 1016.625f, -588.8475f };

	// Only start the fight if the player is above of the platform
	WriteJump((void*)0x604650, E104_CheckPlayer);
}