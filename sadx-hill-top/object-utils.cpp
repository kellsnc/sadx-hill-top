#include "pch.h"

const char* mdlformatstrings[] = {
	"basic",
	"chunk"
};

const char* mdlformatfiletypes[] = {
	".sa1mdl",
	".sa2mdl"
};

ModelInfo* LoadModel(ModelInfo** info, const char* name, ModelFormat format) {
	PrintDebug("[Hill Top] Loading %s model: %s... ", mdlformatstrings[format - 1], name);

	std::string fullPath = "system\\models\\";
	fullPath = fullPath + name + mdlformatfiletypes[format - 1];

	ModelInfo* mdl = new ModelInfo(HelperFunctionsGlobal.GetReplaceablePath(fullPath.c_str()));

	if (mdl->getformat() != format) {
		PrintDebug("Failed!\n");
		delete mdl;
		return nullptr;
	}

	*info = mdl;

	PrintDebug("Done.\n");
	return mdl;
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

bool IsSpecificPlayerInSphere(float x, float y, float z, float radius, uint8_t player) {
	NJS_VECTOR vec = { x, y, z };
	return IsPointInsideSphere(&vec, &EntityData1Ptrs[player]->Position, radius);
}

bool IsSpecificPlayerInSphere(NJS_VECTOR* center, float radius, uint8_t player) {
	return IsPointInsideSphere(center, &EntityData1Ptrs[player]->Position, radius);
}

bool AreSpheresColliding(NJS_VECTOR* sphereA, float radiusA, NJS_VECTOR* sphereB, float radiusB) {
	return GetDistance(sphereB, sphereA) < (radiusA + radiusB);
}