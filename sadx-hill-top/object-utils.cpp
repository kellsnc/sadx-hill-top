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

NJS_POINT3 LerpPoints(NJS_POINT3* v1, NJS_POINT3* v2, float dist)
{
	NJS_VECTOR result;
	result.x = (v2->x - v1->x) * dist + v1->x;
	result.y = (v2->y - v1->y) * dist + v1->y;
	result.z = (v2->z - v1->z) * dist + v1->z;
	return result;
}

float GetDistance(NJS_POINT3* v1, NJS_POINT3* v2)
{
	return sqrtf((v1->x - v2->x) * (v1->x - v2->x) + (v1->y - v2->y) * (v1->y - v2->y) + (v1->z - v2->z) * (v1->z - v2->z));
}

// True if equal, false if not
bool ComparePoints(NJS_POINT3* v1, NJS_POINT3* v2)
{
	return v1->x == v2->x && v1->y == v2->y && v1->z == v2->z;
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

int IsPlayerOnGeoCol(task* tp)
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
		auto twp = playertwp[id];
		auto pwp = playerpwp[id];

		twp->flag = twp->flag & ~(Status_OnColli | Status_Ground) | Status_Attack | Status_Ball;
		pwp->spd.y = pwp->p.jmp_y_spd;
		
		switch (TASKWK_CHARID(twp)) {
		case Characters_Sonic:
			twp->mode = 8; // jump mode
			Sonic_Spin((CharObj2*)pwp); // jump animation
			pwp->work.f = 5.0f;
			break;
		case Characters_Tails:
		case Characters_Knuckles:
			twp->mode = 6; // jump mode
			pwp->mj.reqaction = 14; // jump animation
			pwp->work.f = 2.0f;
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

int IsPlayerOnGeoCol(task* tp, int pnum)
{
	auto pwp = playerpwp[pnum];
	return pwp && pwp->ttp == tp;
}

// Move collision and players that are on it
void MoveGeoCollision(task* tp, NJS_OBJECT* object, NJS_POINT3* pos)
{
	NJS_POINT3* prev_pos = (NJS_POINT3*)&object->pos;

	// Ignore if no movement
	if (!ComparePoints(prev_pos, pos))
	{
		// Get difference between previous and current position
		NJS_VECTOR difference = *pos;
		njSubVector(&difference, prev_pos);

		for (int i = 0; i < MaxPlayers; ++i)
		{
			if (IsPlayerOnGeoCol(tp, i))
			{
				njAddVector(&playertwp[i]->pos, &difference); // move player by differnce between previous and current pos
			}
		}

		// Move collision
		object->pos[0] += difference.x;
		object->pos[1] += difference.y;
		object->pos[2] += difference.z;
	}
}

void RotYGeoCollision(task* tp, NJS_OBJECT* object, Angle y)
{
	Angle prev_angle = object->ang[1];
	
	// Ignore if no change
	if (prev_angle != y)
	{
		Angle difference = SubAngle(prev_angle, y);

		for (int i = 0; i < MaxPlayers; ++i)
		{
			if (IsPlayerOnGeoCol(tp, i))
			{
				playertwp[i]->ang.y += difference;
			}
		}

		object->ang[1] += difference;
	}
}

void SetLevelTexture()
{
	njSetTexture(CurrentLandTable->TexList);
}

void SetSecondObjectTexture()
{
	njSetTexture(LevelObjTexlists[1]);
}