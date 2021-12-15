#pragma once

void LoadModelFile(ModelInfo** info, const char* name, ModelFormat format);
void LoadAnimationFile(AnimationFile** info, const char* name);
void LoadLandTableFile(LandTableInfo** info, const char* name, NJS_TEXLIST* texlist);

template<typename T>
void FreeFileInfo(T** info)
{
	if (*info)
	{
		delete* info;
		*info = nullptr;
	}
}

NJS_POINT3 LerpPoints(NJS_POINT3* v1, NJS_POINT3* v2, float dist);
float GetDistance(NJS_POINT3* v1, NJS_POINT3* v2);
bool ComparePoints(NJS_POINT3* v1, NJS_POINT3* v2);
bool CheckCollisionPointSphere(NJS_POINT3* center, NJS_POINT3* pos, float radius);
int IsPlayerInRange(NJS_POINT3* center, float range);
bool CheckCollisionP_num(NJS_POINT3* center, float radius, int player);
void ForEveryCollidingPlayer(task* tp, void(__cdecl* function)(task*, taskwk*));
void ForcePlayerPos(int id, float x, float y, float z);
void ForcePlayerPos(int id, NJS_POINT3* pos);
bool CheckJump(int id);
NJS_OBJECT* GetModelSibling(NJS_OBJECT* object, int id);
int GetUVCount(NJS_MESHSET_SADX* meshset);
int IsPlayerOnGeoCol(task* tp);
void MoveGeoCollision(task* tp, NJS_OBJECT* object, NJS_POINT3* pos);
void RotYGeoCollision(task* tp, NJS_OBJECT* object, Angle y);
void SetLevelTexture();
void SetSecondObjectTexture();