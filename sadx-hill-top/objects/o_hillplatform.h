#pragma once

extern ModelInfo* ht_platform;

void HillPlatform_LoadAssets();
void HillPlatform_FreeAssets();

void __cdecl HillPlatform(task* tp);