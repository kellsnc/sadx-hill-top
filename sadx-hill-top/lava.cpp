#include "pch.h"
#include "objects/o_lavafall.h"

/*

Draw a secondary landtable for animated lava geometries

*/

static NJS_TEXNAME CurrentLavaNames[2] = { 0 };
NJS_TEXLIST CurrentLavaTex = { CurrentLavaNames, 2 };

static LandTable* LavaTables[3] = { nullptr };

void __cdecl HillTopLava_Display(ObjectMaster* obj) {
	if (!MissedFrames) {
		EntityData1* data = obj->Data1;
		LandTable* land = LavaTables[CurrentAct];

		// Hacky thing to draw the texture we want
		CurrentLavaNames[0] = YOUGAN_ANIM_TEXNAMES[data->Index];
		CurrentLavaNames[1] = LAVAFALL_TexList.textures[data->Index];

		njSetTexture(land->TexList);

		if (land) {
			for (int i = 0; i < land->COLCount; ++i) {
				COL* col = &land->Col[i];

				njPushMatrixEx();
				njTranslateEx((NJS_VECTOR*)&col->Model->pos);
				njRotateEx((Angle*)&col->Model->ang, 0);
				njDrawModel_SADX(col->Model->basicdxmodel);
				njPopMatrixEx();
			}
		}
	}
}

void __cdecl HillTopLava_Main(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;

	// Texture Animation

	if (FrameCounterUnpaused % 2 == 0) {
		++data->Index; // Texture ID
	}

	if (data->Index >= 12) {
		data->Index = 0;
	}

	obj->DisplaySub(obj);
}

void LoadLavaManager() {
	ObjectMaster* lavaobj = LoadObject(LoadObj_Data1, 1, HillTopLava_Main);
	lavaobj->DisplaySub = HillTopLava_Display;
}

void LoadLavaLandTables(const HelperFunctions& helperFunctions) {
	LavaTables[0] = LoadLandTable(helperFunctions, "system\\hilltopzone0_lava.sa1lvl", &CurrentLavaTex);
	LavaTables[1] = LoadLandTable(helperFunctions, "system\\hilltopzone1_lava.sa1lvl", &CurrentLavaTex);
	LavaTables[2] = LavaTables[0];
}