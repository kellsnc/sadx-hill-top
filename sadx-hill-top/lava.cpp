#include "pch.h"
#include "lava.h"
#include "objects/o_lavafall.h"

/*

Draw a secondary landtable for animated lava geometries in act 1 (sonic) and 3 (knuckles)

*/

static NJS_TEXNAME CurrentLavaNames[2] = { 0 };
NJS_TEXLIST CurrentLavaTex = { CurrentLavaNames, 2 };

static LandTableInfo* LavaTableInfo = nullptr;

void __cdecl HillTopLavaDisplay(task* tp)
{
	if (!MissedFrames && (ssActNumber == 0 || ssActNumber == 2))
	{
		auto twp = tp->twp;
		auto land = (_OBJ_LANDTABLE*)LavaTableInfo->getlandtable();

		if (land)
		{
			njSetTexture(land->pTexList);

			for (int i = 0; i < land->ssCount; ++i)
			{
				auto& lnd = land->pLandEntry[i];

				njPushMatrixEx();
				njTranslateEx((NJS_VECTOR*)&lnd.pObject->pos);
				njRotateEx((Angle*)&lnd.pObject->ang, 0);
				dsDrawModel(lnd.pObject->basicdxmodel);
				njPopMatrixEx();
			}
		}
	}
}

void __cdecl HillTopLavaExec(task* tp)
{
	auto twp = tp->twp;

	if (GameTimer % 2 == 0)
	{
		++twp->btimer; // Texture ID
	}

	if (twp->btimer >= 12)
	{
		twp->btimer = 0;
	}

	CurrentLavaTex.textures[0] = YOUGAN_ANIM_TEXNAMES[twp->btimer];
	CurrentLavaTex.textures[1] = LAVAFALL_TexList.textures[twp->btimer];

	tp->disp(tp);
}

void LoadLavaManager()
{
	auto lavatp = CreateElementalTask(IM_TWK, 1, HillTopLavaExec);
	lavatp->disp= HillTopLavaDisplay;
}

void LoadLavaLandTables()
{
	LoadLandTableFile(&LavaTableInfo, "system\\hilltopzone0_lava.sa1lvl", &CurrentLavaTex);
}

void FreeLavaLandTables()
{
	FreeFileInfo(&LavaTableInfo);
}