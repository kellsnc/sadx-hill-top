#include "pch.h"

NJS_MATERIAL matlist_020BFCA8[] = {
	{ { 0xFFBFBFBF }, { 0xFFFFFFFF }, 6, 0, NJD_D_100 | NJD_DA_INV_SRC | NJD_SA_SRC }
};

Sint16 poly_020BFCBC[] = {
	0, 1, 3, 2
};

NJS_MESHSET_SADX meshlist_020BFCC4[] = {
	{ NJD_MESHSET_4 | 0, 1, poly_020BFCBC, NULL, NULL, NULL, NULL, NULL }
};

NJS_VECTOR vertex_020BFCE0[] = {
	{ -4059.6f, -29, -490 },
	{ -3979.5f, -29, 4531.4f },
	{ 1288.9f, -29, 4826.6f },
	{ 1376.7f, -29, -474.7f }
};

NJS_VECTOR normal_020BFD10[] = {
	{ 0, 1, 0 },
	{ 0, 1, 0 },
	{ 0, 1, 0 },
	{ 0, 1, 0 }
};

NJS_MODEL_SADX attach_020BFD40 = { vertex_020BFCE0, normal_020BFD10, LengthOfArray<Sint32>(vertex_020BFCE0), meshlist_020BFCC4, matlist_020BFCA8, LengthOfArray<Uint16>(meshlist_020BFCC4), LengthOfArray<Uint16>(matlist_020BFCA8), { -1341.45f, -29, 2168.3f }, 3801.96f, NULL };

NJS_OBJECT ht1_dz0 = { NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL | NJD_EVAL_BREAK, &attach_020BFD40, 1855.125f, 0, -2084.625f, 0, 0, 0, 1, 1, 1, NULL, NULL };

Sint16 poly_020BFDB4[] = {
	0, 2, 1
};

NJS_MESHSET_SADX meshlist_020BFDBC[] = {
	{ NJD_MESHSET_3 | 0, 1, poly_020BFDB4, NULL, NULL, NULL, NULL, NULL }
};

NJS_VECTOR vertex_020BFDD8[] = {
	{ 483.7f, 1000, 2161.7f },
	{ 1069.5f, 1000, 4494.1f },
	{ -1457, 1000, 4045.8f }
};

NJS_VECTOR normal_020BFDFC[] = {
	{ 0, 1, 0 },
	{ 0, 1, 0 },
	{ 0, 1, 0 }
};

NJS_MODEL_SADX attach_020BFE20 = { vertex_020BFDD8, normal_020BFDFC, LengthOfArray<Sint32>(vertex_020BFDD8), meshlist_020BFDBC, matlist_020BFCA8, LengthOfArray<Uint16>(meshlist_020BFDBC), LengthOfArray<Uint16>(matlist_020BFCA8), { -193.75f, 1000, 3327.9f }, 1719.251f, NULL };

NJS_OBJECT ht1_dz1 = { NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL | NJD_EVAL_BREAK, &attach_020BFE20, 0, 0, 0, 0, 0, 0, 1, 1, 1, NULL, NULL };

DeathZone hilltope0_deathzones[] = {
	{ 0 }
};

DeathZone hilltope1_deathzones[] = {
	{ CharacterFlags_Sonic | CharacterFlags_Gamma, &ht1_dz0 },
	{ CharacterFlags_Sonic | CharacterFlags_Gamma, &ht1_dz1 },
	{ 0 }
};