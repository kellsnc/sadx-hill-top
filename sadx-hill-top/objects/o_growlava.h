#pragma once

void GrowLava_LoadAssets();
void GrowLava_FreeAssets();

void __cdecl GrowLava(ObjectMaster* obj);
void __cdecl GrowLavaPlatform(ObjectMaster* obj);
void __cdecl GrowLavaTrigger(ObjectMaster* obj);
void __cdecl KillCeiling(ObjectMaster* obj);

struct Grow_WK {
	bool Enabled = false;
	float Height = 0.0f;
	float Offset = 0.0f;
};

extern Grow_WK grow_workers[3];