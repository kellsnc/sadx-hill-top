#pragma once

void FireBall_LoadAssets();
void FireBall_FreeAssets();

void LoadFireBall(task* tp, NJS_POINT3* pos, Angle angy, Angle angx, Float spd, Float scl, Float grav, int dir);
void FireBallLauncher(task* tp);