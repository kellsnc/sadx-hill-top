#include "pch.h"
#include "o_efflava.h"

/*

Particule effet above lava.

AngZ: amount (divided in 10 groups)
Scl: range

*/

extern NJS_TEXLIST HillTopOBJ_TexList;

NJS_TEXANIM	LAVAEFF_TEXANIM = { 0x2, 0x2, 0, 0, 0, 0, 0x100, 0x100, 7, 0x20 };
NJS_SPRITE LAVAEFF_SPRITE = { { 0.0f, 0.0f, 0.0f }, 1.0f, 1.0f, 0, &HillTopOBJ_TexList, &LAVAEFF_TEXANIM };

int round_up(int num, int factor)
{
	return num - 1 - (num - 1) % factor + factor;
}

// Sets a particule position to anywhere on a xz plane
void SpawnLavaParticule(NJS_VECTOR* offset, Float x_range, Float z_range)
{
	offset->x = (x_range / 2.0f) - static_cast<Float>((rand() % static_cast<int>(x_range)));
	offset->z = (z_range / 2.0f) - static_cast<Float>((rand() % static_cast<int>(z_range)));
	offset->y = 0;
}

// Draw a set of lava sprites
void DrawLavaSprites(NJS_VECTOR* vecs, Angle3* rot, int count, float limit, float offx, float offz)
{
	for (int i = 0; i < count; ++i)
	{
		njPushMatrixEx();
		njTranslate(0, vecs[i].x + offx, vecs[i].y, vecs[i].z + offz);
		njRotateY_(-rot->y + Camera_Data1->Rotation.y); // adjust to camera
		njRotateX_(-rot->x + Camera_Data1->Rotation.x); // adjust to camera
		njScalef(1.0f - vecs[i].y / limit); // reduce size as it approaches the limit
		njDrawSprite3D(&LAVAEFF_SPRITE, 0, NJD_SPRITE_ALPHA); // Draw the sprite with alpha & color
		njPopMatrixEx();
	}
}

void __cdecl LavaEffectDisplay(task* tp)
{
	if (!MissedFrames)
	{
		auto twp = tp->twp;

		SetSecondObjectTexture();
		njPushMatrixEx();
		njTranslateEx(&twp->pos);

		njRotateY_(twp->ang.y);
		njRotateX_(twp->ang.x);

		SetMaterial(1.0f, 1.0f, 1.0f, 1.0f);
		njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
		njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);

		auto vecs = (NJS_VECTOR*)twp->value.ptr; // stored offsets
		int count = twp->ang.z / 10; // get amount per groups

		// Divide in 10 groups
		for (int i = 0; i < 10; ++i)
		{
			// Some kind of animation for the particules
			float sin = njSin(3500 * (i + FrameCounterUnpaused)) * 0.1f;
			float elapse = i * 0.125f + 0.1f;

			// Draw
			DrawLavaSprites(&vecs[i * count], &twp->ang, count, twp->scl.y, sin * (1.0f - elapse), (elapse + 1.0f) * sin);
		}

		SetDefaultAlphaBlend();
		ResetMaterial();
		njPopMatrixEx();
	}
}

void __cdecl LavaEffectExec(task* tp)
{
	if (!CheckRangeOut(tp))
	{
		auto twp = tp->twp;

		auto vecs = (NJS_VECTOR*)twp->value.ptr; // stored offsets
		int count = twp->ang.z / 10; // get amount per groups

		// Register sound if close to the object
		if (CheckCollisionP(&twp->pos, 500.0f))
		{
			dsPlay_Dolby_time(459, 0, 0, 60, 120, twp);
		}
		
		// Divide in 10 groups, each group moves at a different speed
		for (int i = 0; i < 10; ++i)
		{
			for (int j = 0; j < count; ++j)
			{
				auto vec = &vecs[i * count + j];

				if (vec->y > twp->scl.y)
				{
					SpawnLavaParticule(vec, twp->scl.x, twp->scl.z); // if higher than the y limit, respawn
				}
				else
				{
					vec->y += 0.10f + (0.01f * i); // y speed
				}
			}
		}

		tp->disp(tp);
	}
}

void __cdecl LavaEffectDestroy(task* tp)
{
	if (tp->twp)
	{
		delete[] tp->twp->value.ptr; // release allocated memory
	}
}

void __cdecl LavaEffect(task* tp)
{
	auto twp = tp->twp;

	// If z range is empty, use x range
	if (twp->scl.z == 0.0f)
	{
		twp->scl.z = twp->scl.x;
	}

	// Keep the amount as a multiple of ten to avoid integer rounding
	int amount = twp->ang.z = round_up(twp->ang.z, 10);

	auto offsets = new NJS_VECTOR[amount](); // allocate one offset per particule
	twp->value.ptr = offsets; // store the allocated memory pointer in twp->value

	// Spawn every particule
	for (int i = 0; i < amount; ++i)
	{
		SpawnLavaParticule(&offsets[i], twp->scl.x, twp->scl.z);

		// Random height in Y range on first load
		offsets[i].y = (twp->scl.y / 2.0f) - static_cast<Float>((rand() % static_cast<int>(twp->scl.y)));
	}

	tp->dest = LavaEffectDestroy;
	tp->exec = LavaEffectExec;
	tp->disp = LavaEffectDisplay;
}