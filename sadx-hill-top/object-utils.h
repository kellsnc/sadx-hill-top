#pragma once

void LoadModelFile(ModelInfo** info, const char* name, ModelFormat format);
void LoadAnimationFile(AnimationFile** info, const char* name);
void LoadLandTableFile(LandTableInfo** info, const char* name, NJS_TEXLIST* texlist);

void FreeLandTableFile(LandTableInfo** info);
void FreeModelFile(ModelInfo** info);
void FreeAnimationFile(AnimationFile** info);

NJS_VECTOR GetPositionBetweenPoints(NJS_VECTOR* orig, NJS_VECTOR* dest, float dist);

float GetLength(NJS_VECTOR* orig, NJS_VECTOR* dest);
float GetDistance(NJS_VECTOR* orig, NJS_VECTOR* dest);
bool IsPointInsideSphere(NJS_VECTOR* center, NJS_VECTOR* pos, float radius);
int IsPlayerInRange(NJS_VECTOR* center, float range);
int IsPlayerInsideSphere_(NJS_VECTOR* center, float radius);
int IsPlayerInsideSphere_(float x, float y, float z, float radius);
bool IsSpecificPlayerInSphere(float x, float y, float z, float radius, int player);
bool IsSpecificPlayerInSphere(NJS_VECTOR* center, float radius, int player);
bool AreSpheresColliding(NJS_VECTOR* sphereA, float radiusA, NJS_VECTOR* sphereB, float radiusB);
EntityData1* IsPlayerOnDyncol(ObjectMaster* obj);
void ForEveryPlayerOnDyncol(ObjectMaster* obj, void(__cdecl* function)(ObjectMaster*, EntityData1*));
int IsPlayerInGlobalCylinder(NJS_VECTOR* center, float x, float y);
void SetPlayerPosition(int id, float x, float y, float z);
void SetPlayerPosition(int id, NJS_VECTOR* pos);
bool CheckJump(int id);
NJS_OBJECT* GetModelSibling(NJS_OBJECT* object, int id);