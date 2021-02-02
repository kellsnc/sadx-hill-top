#pragma once

void GrowLava_LoadAssets();
void GrowLava_FreeAssets();

void __cdecl GrowLava(ObjectMaster* obj);
void __cdecl GrowLavaPlatform(ObjectMaster* obj);
void __cdecl GrowLavaTrigger(ObjectMaster* obj);
void __cdecl KillCeiling(ObjectMaster* obj);