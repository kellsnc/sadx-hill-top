#include "pch.h"
#include "lava.h"
#include "objects/o_lavafall.h"

/*

Draw a secondary landtable for animated lava geometries

*/

static NJS_TEXNAME CurrentLavaNames[2] = { 0 };
NJS_TEXLIST CurrentLavaTex = { CurrentLavaNames, 2 };

static LandTableInfo* LavaTableInfo = nullptr;

void __cdecl HillTopLava_Display(ObjectMaster* obj)
{
	if (!MissedFrames && (CurrentAct == 0 || CurrentAct == 2))
	{
		EntityData1* data = obj->Data1;
		LandTable* land = LavaTableInfo->getlandtable();

		if (land)
		{
			njSetTexture(land->TexList);

			for (int i = 0; i < land->COLCount; ++i)
			{
				COL* col = &land->Col[i];

				njPushMatrixEx();
				njTranslateEx((NJS_VECTOR*)&col->Model->pos);
				njRotateEx((Angle*)&col->Model->ang, 0);
				DrawModel(col->Model->basicdxmodel);
				njPopMatrixEx();
			}
		}
	}
}

void __cdecl HillTopLava_Main(ObjectMaster* obj)
{
	EntityData1* data = obj->Data1;

	if (LevelFrameCount % 2 == 0)
	{
		++data->Index; // Texture ID
	}

	if (data->Index >= 12)
	{
		data->Index = 0;
	}

	CurrentLavaNames[0] = YOUGAN_ANIM_TEXNAMES[data->Index];
	CurrentLavaNames[1] = LAVAFALL_TexList.textures[data->Index];

	obj->DisplaySub(obj);
}

void LoadLavaManager()
{
	ObjectMaster* lavaobj = LoadObject(LoadObj_Data1, 1, HillTopLava_Main);
	lavaobj->DisplaySub = HillTopLava_Display;
}

void LoadLavaLandTables()
{
	LoadLandTableFile(&LavaTableInfo, "system\\hilltopzone0_lava.sa1lvl", &CurrentLavaTex);
}

void FreeLavaLandTables()
{
	FreeFileInfo(&LavaTableInfo);
}