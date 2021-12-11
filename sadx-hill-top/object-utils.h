#pragma once

#define TARGET_DYNAMIC(name) ((decltype(name##_r)*)name##_t->Target())

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

NJS_VECTOR GetPositionBetweenPoints(NJS_VECTOR* orig, NJS_VECTOR* dest, float dist);
float GetDistance(NJS_POINT3* orig, NJS_POINT3* dest);
bool CheckCollisionPointSphere(NJS_POINT3* center, NJS_POINT3* pos, float radius);
int IsPlayerInRange(NJS_POINT3* center, float range);
bool CheckCollisionP_num(NJS_POINT3* center, float radius, int player);
int IsPlayerOnDyncol(task* tp);
void ForEveryCollidingPlayer(task* tp, void(__cdecl* function)(task*, taskwk*));
void ForEveryPlayerOnDyncol(task* tp, void(__cdecl* function)(task*, taskwk*));
void ForcePlayerPos(int id, float x, float y, float z);
void ForcePlayerPos(int id, NJS_POINT3* pos);
bool CheckJump(int id);
NJS_OBJECT* GetModelSibling(NJS_OBJECT* object, int id);
void SetLevelTexture();
void SetSecondObjectTexture();