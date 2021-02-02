#include "pch.h"
#include "o_efflava.h"

/*

Particule effet above lava.

RotZ: amount
Scale: range

*/

extern NJS_TEXLIST HillTopOBJ_TexList;

NJS_TEXANIM	LAVAEFF_TEXANIM = { 0x2, 0x2, 0, 0, 0, 0, 0x100, 0x100, 7, 0x20 };
NJS_SPRITE LAVAEFF_SPRITE = { { 0.0f, 0.0f, 0.0f }, 1.0f, 1.0f, 0, &HillTopOBJ_TexList, &LAVAEFF_TEXANIM };

int round_up(int num, int factor) {
	return num - 1 - (num - 1) % factor + factor;
}

// Sets a particule position to anywhere on a xz plane
void SpawnLavaParticule(NJS_VECTOR* offset, Float x_range, Float z_range) {
	offset->x = (x_range / 2.0f) - static_cast<Float>((rand() % static_cast<int>(x_range)));
	offset->z = (z_range / 2.0f) - static_cast<Float>((rand() % static_cast<int>(z_range)));
	offset->y = 0;
}

// Draw a set of lava sprites
void DrawLavaSprites(NJS_VECTOR* vecs, Rotation3* rot, int count, float limit, float offx, float offz) {
	for (int i = 0; i < count; ++i) {
		njPushMatrixEx();
		njTranslate(0, vecs[i].x + offx, vecs[i].y, vecs[i].z + offz);
		njRotateY_(-rot->y + Camera_Data1->Rotation.y); // adjust to camera
		njRotateX_(-rot->x + Camera_Data1->Rotation.x); // adjust to camera

		njScalef(1.0f - vecs[i].y / limit); // reduce size as it approaches the limit

		njDrawSprite3D(&LAVAEFF_SPRITE, 0, NJD_SPRITE_ALPHA); // Draw the sprite with alpha & color
		njPopMatrixEx();
	}
}

void __cdecl LavaEffect_Display(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;

	if (!MissedFrames) {
		njSetTexture(&HillTopOBJ_TexList);
		njPushMatrixEx();
		njTranslateEx(&data->Position);

		njRotateY_(data->Rotation.y);
		njRotateX_(data->Rotation.x);

		njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
		njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);

		NJS_VECTOR* vecs = (NJS_VECTOR*)data->LoopData;
		int count = static_cast<int>(data->Rotation.z) / 10;

		// Divide sprites in a group of ten different speeds
		for (int i = 0; i < 10; ++i) {

			// Some kind of animation for the particules
			float sin = njSin(3500 * (i + FrameCounterUnpaused)) * 0.1000000014901161;
			float elapse = i * 0.125 + 0.1000000014901161;

			// Draw
			DrawLavaSprites(&vecs[i * count], &data->Rotation, count, data->Scale.y, sin * (1.0 - elapse), (elapse + 1.0) * sin);
		}

		njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
		njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);

		njPopMatrixEx();
	}
}

void __cdecl LavaEffect_Main(ObjectMaster* obj) {
	if (!ClipSetObject(obj)) {
		EntityData1* data = obj->Data1;

		NJS_VECTOR* vecs = (NJS_VECTOR*)data->LoopData;
		int count = static_cast<int>(data->Rotation.z) / 10;

		// Divide sprites in a group of ten different speeds
		for (int i = 0; i < 10; ++i) {
			for (Uint16 j = 0; j < count; ++j) {
				NJS_VECTOR* vec = &vecs[i * count + j];

				if (vec->y > data->Scale.y) {
					SpawnLavaParticule(vec, data->Scale.x, data->Scale.z); // if higher than the limit, respawn
				}
				else {
					vec->y += 0.10f + (0.01 * i); // y speed
				}
			}
		}

		obj->DisplaySub(obj);
	}
}

void __cdecl LavaEffect_Delete(ObjectMaster* obj) {
	delete[] obj->Data1->LoopData; // release allocated memory
}

void __cdecl LavaEffect(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;

	// If z range is empty, use x range
	if (data->Scale.z == 0) {
		data->Scale.z = data->Scale.x;
	}

	// Keep the amount as a multiple of ten to avoid integer rounding
	data->Rotation.z = round_up(data->Rotation.z, 10);

	int amount = static_cast<int>(data->Rotation.z);

	NJS_VECTOR* offsets = new NJS_VECTOR[amount](); //alocate one offset per particule
	data->LoopData = (Loop*)offsets; // store the allocated memory pointer in LoopData

	// Spawn every particule
	for (int i = 0; i < amount; ++i) {
		SpawnLavaParticule(&offsets[i], data->Scale.x, data->Scale.z);

		// random height when the object loads
		offsets[i].y = (data->Scale.y / 2.0f) - static_cast<Float>((rand() % static_cast<int>(data->Scale.y)));
	}

	obj->DeleteSub = LavaEffect_Delete;
	obj->MainSub = LavaEffect_Main;
	obj->DisplaySub = LavaEffect_Display;
}