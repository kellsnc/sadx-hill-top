#include "pch.h"

const char* mdlformatstrings[] = {
	"basic",
	"chunk"
};

std::string mdlformatfiletypes[] = {
	".sa1mdl",
	".sa2mdl"
};

void LoadModelFile(ModelInfo** info, const char* name, ModelFormat format)
{
	std::string fullPath = "system\\models\\" + (std::string)name + mdlformatfiletypes[format - 1];
	auto mdl = new ModelInfo(gHelperFunctions->GetReplaceablePath(fullPath.c_str()));

	if (mdl->getformat() != format)
	{
		PrintDebug("[Hill Top] Failed to load %s model: %s\n", mdlformatstrings[(int)format - 1], name);
		delete mdl;
		*info = nullptr;
	}
	else
	{
		PrintDebug("[Hill Top] Successfully loaded %s model: %s\n", mdlformatstrings[(int)format - 1], name);
		*info = mdl;
	}
}

void LoadAnimationFile(AnimationFile** info, const char* name)
{
	std::string fullPath = "system\\models\\" + (std::string)name + ".saanim";
	auto anm = new AnimationFile(gHelperFunctions->GetReplaceablePath(fullPath.c_str()));

	if (anm->getmodelcount() == 0)
	{
		PrintDebug("[Hill Top] Failed to load animation: %s... ", name);
		delete anm;
		*info = nullptr;
	}
	else
	{
		PrintDebug("[Hill Top] Successfully loaded animation: %s... ", name);
		*info = anm;
	}
}

void LoadLandTableFile(LandTableInfo** info, const char* name, NJS_TEXLIST* texlist)
{
	auto lnd = new LandTableInfo(gHelperFunctions->GetReplaceablePath(name));

	if (lnd->getlandtable() == nullptr)
	{
		PrintDebug("[Hill Top] Failed to load geometry file: %s... ", name);
		delete lnd;
		*info = nullptr;
	}
	else
	{
		PrintDebug("[Hill Top] Successfully loaded geometry file: %s... ", name);
		lnd->getlandtable()->TexList = texlist;
		*info = lnd;
	}
}

NJS_VECTOR GetPositionBetweenPoints(NJS_VECTOR* orig, NJS_VECTOR* dest, float dist)
{
	NJS_VECTOR result;
	result.x = (dest->x - orig->x) * dist + orig->x;
	result.y = (dest->y - orig->y) * dist + orig->y;
	result.z = (dest->z - orig->z) * dist + orig->z;

	return result;
}

float GetDistance(NJS_POINT3* orig, NJS_POINT3* dest)
{
	NJS_POINT3 v = *dest;
	njSubVector(&v, orig);
	return njScalor(&v);
}

bool CheckCollisionPointSphere(NJS_POINT3* center, NJS_POINT3* pos, float radius)
{
	return GetDistance(center, pos) <= radius;
}

bool CheckCollisionP_num(NJS_POINT3* center, float radius, int player)
{
	return playertwp[player] ? CheckCollisionPointSphere(center, &playertwp[player]->pos, radius) : false;
}

int IsPlayerInRange(NJS_POINT3* center, float range)
{
	for (int player = 0; player < MaxPlayers; ++player)
	{
		if (!playertwp[player]) continue;

		if (ObjectInRange(&playertwp[player]->pos, center->x, center->y, center->z, range))
		{
			return player + 1;
		}
	}

	return 0;
}

int IsPlayerOnDyncol(task* tp)
{
	for (int i = 0; i < MaxPlayers; ++i)
	{
		auto pwp = playerpwp[i];

		if (pwp && pwp->ttp == tp)
		{
			return i + 1;
		}
	}

	return 0;
}

void ForEveryCollidingPlayer(task* tp, void(__cdecl* function)(task*, taskwk*))
{
	auto cwp = tp->twp->cwp;

	for (int i = 0; i < 16; ++i)
	{
		auto hit = &cwp->hit_info[i];

		if (hit->hit_num == -1) break;

		if (hit->hit_twp->cwp->id == 0)
		{
			function(tp, hit->hit_twp);
		}
	}
}

void ForEveryPlayerOnDyncol(task* tp, void(__cdecl* function)(task*, taskwk*))
{
	for (int i = 0; i < MaxPlayers; ++i)
	{
		auto pwp = playerpwp[i];
		
		if (pwp && pwp->ttp == tp)
		{
			function(tp, playertwp[i]);
		}
	}
}

void ForcePlayerPos(int id, float x, float y, float z)
{
	playertwp[id]->pos = { x, y, z };
}

void ForcePlayerPos(int id, NJS_POINT3* pos)
{
	ForcePlayerPos(id, pos->x, pos->y, pos->z);
}

bool CheckJump(int id)
{
	if (PressedButtons[id] & Buttons_A)
	{
		EntityData1* pdata = EntityData1Ptrs[id];
		CharObj2* pco2 = CharObj2Ptrs[id];

		switch (pdata->CharID) {
		case Characters_Sonic:
			pdata->Action = 8;
			pdata->Status = pdata->Status & ~Status_OnColli | Status_Attack | Status_Ball;
			pco2->Speed.y = pco2->PhysicsData.JumpSpeed;
			pco2->SpindashSpeed = 5.0f;
			Sonic_Spin(pco2);
			break;
		case Characters_Tails:
		case Characters_Knuckles:
			pdata->Action = 6;
			pdata->Status = pdata->Status & ~Status_OnColli | Status_Attack | Status_Ball;
			pco2->Speed.y = pco2->PhysicsData.JumpSpeed;
			pco2->SpindashSpeed = 2.0f;
			pco2->AnimationThing.Index = 14;
			break;
		}

		return true;
	}

	return false;
}

NJS_OBJECT* GetModelSibling(NJS_OBJECT* object, int id)
{
	int i = 0;

	while (object)
	{
		if (i == id)
		{
			return object;
		}

		++i;
		object = object->sibling;
	}
}

int GetUVCount(NJS_MESHSET_SADX* meshset)
{
	switch (meshset->type_matId)
	{
	case NJD_MESHSET_3:
		return meshset->nbMesh * 3;
	case NJD_MESHSET_4:
		return meshset->nbMesh * 4;
	case NJD_MESHSET_N:
	case NJD_MESHSET_TRIMESH:
		int indices = 0;
		int currentindex = 0;
		int currentmesh = 0;

		while (currentmesh < meshset->nbMesh) {
			int numvrt = meshset->meshes[currentindex] & 0x7FFF;
			indices += numvrt;
			currentindex += numvrt + 1;
			currentmesh++;
		}

		return indices;
	}

	return 0;
}

void SetLevelTexture()
{
	njSetTexture(CurrentLandTable->TexList);
}

void SetSecondObjectTexture()
{
	njSetTexture(LevelObjTexlists[1]);
}