#pragma once

void FireBall_LoadAssets();
void FireBall_FreeAssets();

void LoadFireBall(ObjectMaster* obj, NJS_VECTOR* position, Angle roty, Angle rotx, Float Speed, Float size, Float Grav, int dir);
void FireBallLauncher(ObjectMaster* obj);