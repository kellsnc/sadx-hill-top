#include "pch.h"

const char* mdlformatstrings[] = {
	"basic",
	"chunk"
};

const char* mdlformatfiletypes[] = {
	".sa1mdl",
	".sa2mdl"
};

void LoadModelFile(ModelInfo** info, const char* name, ModelFormat format) {
	PrintDebug("[Hill Top] Loading %s model: %s... ", mdlformatstrings[format - 1], name);

	std::string fullPath = "system\\models\\";
	fullPath = fullPath + name + mdlformatfiletypes[format - 1];

	ModelInfo* mdl = new ModelInfo(HelperFunctionsGlobal.GetReplaceablePath(fullPath.c_str()));

	if (mdl->getformat() != format) {
		PrintDebug("Failed!\n");
		delete mdl;
		*info = nullptr;
	}
	else {
		PrintDebug("Done.\n");
		*info = mdl;
	}
}

void LoadAnimationFile(AnimationFile** info, const char* name) {
	PrintDebug("[Hill Top] Loading animation: %s... ", name);

	std::string fullPath = "system\\models\\";
	fullPath = fullPath + name + ".saanim";

	AnimationFile* anm = new AnimationFile(HelperFunctionsGlobal.GetReplaceablePath(fullPath.c_str()));

	if (anm->getmodelcount() == 0) {
		PrintDebug("Failed!\n");
		delete anm;
		*info = nullptr;
	}
	else {
		PrintDebug("Done.\n");
		*info = anm;
	}
}

void LoadLandTableFile(LandTableInfo** info, const char* name, NJS_TEXLIST* texlist) {
	PrintDebug("[Hill Top] Loading geometry file: %s... ", name);

	LandTableInfo* lnd = new LandTableInfo(HelperFunctionsGlobal.GetReplaceablePath(name));

	if (lnd->getlandtable() == nullptr) {
		PrintDebug("Failed!\n");
		delete lnd;
		*info = nullptr;
	}
	else {
		PrintDebug("Done.\n");
		lnd->getlandtable()->TexList = texlist;
		*info = lnd;
	}
}

void FreeLandTableFile(LandTableInfo** info) {
	if (*info) {
		delete* info;
		info = nullptr;
	}
}

void FreeModelFile(ModelInfo** info) {
	if (*info) {
		delete* info;
		info = nullptr;
	}
}

void FreeAnimationFile(AnimationFile** info) {
	if (*info) {
		delete* info;
		info = nullptr;
	}
}

NJS_VECTOR GetPositionBetweenPoints(NJS_VECTOR* orig, NJS_VECTOR* dest, float dist) {
	NJS_VECTOR result;
	result.x = (dest->x - orig->x) * dist + orig->x;
	result.y = (dest->y - orig->y) * dist + orig->y;
	result.z = (dest->z - orig->z) * dist + orig->z;

	return result;
}

float GetLength(NJS_VECTOR* orig, NJS_VECTOR* dest) {
	return powf(dest->x - orig->x, 2) + powf(dest->y - orig->y, 2) + powf(dest->z - orig->z, 2);
}

float GetDistance(NJS_VECTOR* orig, NJS_VECTOR* dest) {
	return sqrtf(GetLength(orig, dest));
}

bool IsPointInsideSphere(NJS_VECTOR* center, NJS_VECTOR* pos, float radius) {
	return GetDistance(center, pos) <= radius;
}

int IsPlayerInRange(NJS_VECTOR* center, float range) {
	for (uint8_t player = 0; player < MaxPlayers; ++player) {
		if (!EntityData1Ptrs[player]) continue;

		if (ObjectInRange(&EntityData1Ptrs[player]->Position, center->x, center->y, center->z, range)) {
			return player + 1;
		}
	}

	return 0;
}

int IsPlayerInsideSphere_(NJS_VECTOR* center, float radius) {
	for (uint8_t player = 0; player < MaxPlayers; ++player) {
		if (!EntityData1Ptrs[player]) continue;

		if (IsPointInsideSphere(center, &EntityData1Ptrs[player]->Position, radius)) {
			return player + 1;
		}
	}

	return 0;
}

int IsPlayerInsideSphere_(float x, float y, float z, float radius) {
	NJS_VECTOR vec = { x, y, z };
	return IsPlayerInsideSphere_(&vec, radius);
}

bool IsSpecificPlayerInSphere(float x, float y, float z, float radius, int player) {
	NJS_VECTOR vec = { x, y, z };
	return IsPointInsideSphere(&vec, &EntityData1Ptrs[player]->Position, radius);
}

bool IsSpecificPlayerInSphere(NJS_VECTOR* center, float radius, int player) {
	return IsPointInsideSphere(center, &EntityData1Ptrs[player]->Position, radius);
}

bool AreSpheresColliding(NJS_VECTOR* sphereA, float radiusA, NJS_VECTOR* sphereB, float radiusB) {
	return GetDistance(sphereB, sphereA) < (radiusA + radiusB);
}

EntityData1* IsPlayerOnDyncol(ObjectMaster* obj) {
	for (int i = 0; i < MaxPlayers; ++i) {
		CharObj2* co2 = CharObj2Ptrs[i];
		
		if (co2 && co2->field_6C == obj) {
			return EntityData1Ptrs[i];
		}
	}

	return nullptr;
}

void ForEveryPlayerOnDyncol(ObjectMaster* obj, void(__cdecl* function)(ObjectMaster*, EntityData1*)) {
	for (int i = 0; i < MaxPlayers; ++i) {
		CharObj2* co2 = CharObj2Ptrs[i];

		if (co2 && co2->field_6C == obj) {
			function(obj, EntityData1Ptrs[i]);
		}
	}
}

int IsPlayerInGlobalCylinder(NJS_VECTOR* center, float x, float y) {
	for (uint8_t player = 0; player < MaxPlayers; ++player) {
		if (!EntityData1Ptrs[player]) continue;

		NJS_VECTOR* pos = &EntityData1Ptrs[player]->Position;

		if ((powf(pos->x - center->x, 2) + pow(pos->z - center->z, 2)) <= pow(x, 2) &&
			pos->y > center->y - y / 2 && pos->y < center->y + y / 2) {
			return player + 1;
		}
	}

	return 0;
}

void SetPlayerPosition(int id, float x, float y, float z) {
	EntityData1Ptrs[id]->Position = { x, y, z };
}

void SetPlayerPosition(int id, NJS_VECTOR* pos) {
	SetPlayerPosition(id, pos->x, pos->y, pos->z);
}

bool CheckJump(int id) {
	if (PressedButtons[id] & Buttons_A) {
		EntityData1* pdata = EntityData1Ptrs[id];
		CharObj2* pco2 = CharObj2Ptrs[id];

		switch (pdata->CharID) {
		case Characters_Sonic:
			pdata->Action = 8;
			pdata->Status = pdata->Status & ~Status_Unknown1 | Status_Attack | Status_Ball;
			pco2->Speed.y = pco2->PhysicsData.JumpSpeed;
			pco2->SpindashSpeed = 5.0f;
			Sonic_Spin(pco2);
			break;
		case Characters_Tails:
		case Characters_Knuckles:
			pdata->Action = 6;
			pdata->Status = pdata->Status & ~Status_Unknown1 | Status_Attack | Status_Ball;
			pco2->Speed.y = pco2->PhysicsData.JumpSpeed;
			pco2->SpindashSpeed = 2.0f;
			pco2->AnimationThing.Index = 14;
			break;
		}

		return true;
	}

	return false;
}

NJS_OBJECT* GetModelSibling(NJS_OBJECT* object, int id) {
	int i = 0;

	while (object) {
		if (i == id) {
			return object;
		}

		++i;
		object = object->sibling;
	}
}