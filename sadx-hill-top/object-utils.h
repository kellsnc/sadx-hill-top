#pragma once

ModelInfo* LoadModel(ModelInfo** info, const char* name, ModelFormat format);
NJS_VECTOR GetPositionBetweenPoints(NJS_VECTOR* orig, NJS_VECTOR* dest, float dist);

float GetLength(NJS_VECTOR* orig, NJS_VECTOR* dest);
float GetDistance(NJS_VECTOR* orig, NJS_VECTOR* dest);
bool IsPointInsideSphere(NJS_VECTOR* center, NJS_VECTOR* pos, float radius);
int IsPlayerInsideSphere_(NJS_VECTOR* center, float radius);
int IsPlayerInsideSphere_(float x, float y, float z, float radius);
bool IsSpecificPlayerInSphere(float x, float y, float z, float radius, uint8_t player);
bool IsSpecificPlayerInSphere(NJS_VECTOR* center, float radius, uint8_t player);
bool AreSpheresColliding(NJS_VECTOR* sphereA, float radiusA, NJS_VECTOR* sphereB, float radiusB);
EntityData1* IsPlayerOnDyncol(ObjectMaster* obj);
void ForEveryPlayerOnDyncol(ObjectMaster* obj, void(__cdecl* function)(ObjectMaster*, EntityData1*));