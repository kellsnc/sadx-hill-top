#pragma once

void GrowLava_LoadAssets();
void GrowLava_FreeAssets();

void __cdecl GrowLava(task* tp);
void __cdecl GrowLavaPlatform(task* tp);
void __cdecl GrowLavaTrigger(task* tp);
void __cdecl KillCeiling(task* tp);

struct Grow_WK {
	bool Enabled = false;
	float Height = 0.0f;
	float Offset = 0.0f;
};

extern Grow_WK grow_workers[3];