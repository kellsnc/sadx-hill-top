#include "pch.h"
#include "o_hillflowers.h"

ModelInfo* ht_flowers = nullptr;

void __cdecl HillFlowers_Display(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;

	if (!MissedFrames) {
		njSetTexture(LevelObjTexlists[1]);
		njPushMatrixEx();
		njTranslateEx(&data->Position);
		njRotateEx((Angle*)&data->Rotation, false);

		njScalef(data->Scale.x);
		njDrawModel_SADX(data->Object->basicdxmodel);

		NJS_OBJECT* branch = data->Object->child;

		while (branch) {
			njTranslate(nullptr, Pos3(branch->pos));
			njRotateXYZ(nullptr, Pos3(branch->ang));
			njScale(nullptr, Pos3(branch->scl));
			njDrawModel_SADX(branch->basicdxmodel);

			branch = branch->child;
		}
		
		njPopMatrixEx();
	}
}

void __cdecl HillFlowers_Main(ObjectMaster* obj) {
	if (!ClipSetObject(obj)) {
		obj->DisplaySub(obj);
	}
}

void __cdecl HillFlowers(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	
	data->Scale.y = 2;
	data->Scale.x = 1;

	// Choose the model and collison based on set information
	switch (static_cast<int>(data->Scale.y)) {
	default:
		data->Object = ht_flowers->getmodel()->child;
		break;
	case 1:
		data->Object = ht_flowers->getmodel()->child->sibling;
		break;
	case 2:
		data->Object = ht_flowers->getmodel()->child->sibling->sibling;
		break;
	}
	
	// If the scale is null, set it to normal
	if (data->Scale.x == 0) {
		data->Scale.x = 1;
	}

	obj->MainSub = HillFlowers_Main;
	obj->DisplaySub = HillFlowers_Display;
}