#include "pch.h"
#include "e_epsilon.h"

int __cdecl E104_CheckPlayer(ObjectMaster* obj)
{
	return IsSpecificPlayerInSphere(&obj->Data1->Position, 150.0f, 0) == true && (GetCharacterObject(0)->Data1->Status & Status_Ground);
}

void GammaOutOfBounds(EntityData1* data)
{
	if (data->Position.y < 945.0f)
	{
		data->Index = 1;
	}

	if (data->Index == 1)
	{
		data->Position.y += 1.0f;

		if (data->Position.y > 970.0f)
		{
			data->Index = 0;
		}
	}
}

void __cdecl E104Enemy_Main_j(ObjectMaster* obj)
{
	GammaOutOfBounds(obj->Data1);
	E104Enemy_Main(obj);
}

void EpsilonBoss_Init()
{
	// New fight boundaries
	GammaCamBounds[0] = { 867.7648f, 996.625f, -600.5748f };
	GammaCamBounds[1] = { 990.7285f, 996.625f, -368.2955f };
	GammaCamBounds[2] = { 965.4641f, 996.625f, -203.8346f };
	GammaCamBounds[3] = { 845.3178f, 996.625f, -312.3701f };
	GammaCamBounds[4] = { 698.6425f, 1016.625f, -588.8475f };

	// Only start the fight if the player is above the platform
	WriteJump((void*)0x604650, E104_CheckPlayer);
	WriteData((ObjectFuncPtr*)0x605C25, E104Enemy_Main_j);
}