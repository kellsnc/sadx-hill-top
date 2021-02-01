#include "pch.h"

NJS_MATERIAL mat_material_colli[] = {
	{ { 0x00000000 }, { 0x00000000 }, 0, 0, NJD_FILTER_BILINEAR | NJD_DA_INV_SRC | NJD_SA_SRC }
};

Sint16 bsc_attach_ht0dz_p0[] = {
	4, 0, 1, 2, 3
};

NJS_MESHSET_SADX bsc_attach_ht0dz_set[] = {
	{ NJD_MESHSET_TRIMESH | 0, 1, bsc_attach_ht0dz_p0, NULL, NULL, NULL, NULL, NULL }
};

NJS_VECTOR bsc_attach_ht0dz_pos[] = {
	{ -2534.107f, 0, 2534.107f },
	{ 2534.107f, 0, 2534.107f },
	{ -2534.107f, 0, -2534.107f },
	{ 2534.107f, 0, -2534.107f }
};

NJS_VECTOR bsc_attach_ht0dz_nrm[] = {
	{ 0, 1, 0 },
	{ 0, 1, 0 },
	{ 0, 1, 0 },
	{ 0, 1, 0 }
};

NJS_MODEL_SADX bsc_attach_ht0dz = { bsc_attach_ht0dz_pos, bsc_attach_ht0dz_nrm, LengthOfArray<Sint32>(bsc_attach_ht0dz_pos), bsc_attach_ht0dz_set, mat_material_colli, LengthOfArray<Uint16>(bsc_attach_ht0dz_set), LengthOfArray<Uint16>(mat_material_colli), { 0 }, 3583.768f, NULL };

NJS_OBJECT ht0_dz = { NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL | NJD_EVAL_BREAK | NJD_EVAL_SHAPE_SKIP, &bsc_attach_ht0dz, 865, -350, 1067, 0, 0, 0, 1, 1, 1, NULL, NULL };

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

NJS_MODEL_SADX attach_020BFD40 = { vertex_020BFCE0, normal_020BFD10, LengthOfArray<Sint32>(vertex_020BFCE0), meshlist_020BFCC4, mat_material_colli, LengthOfArray<Uint16>(meshlist_020BFCC4), LengthOfArray<Uint16>(mat_material_colli), { -1341.45f, -29, 2168.3f }, 3801.96f, NULL };

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

NJS_MODEL_SADX attach_020BFE20 = { vertex_020BFDD8, normal_020BFDFC, LengthOfArray<Sint32>(vertex_020BFDD8), meshlist_020BFDBC, mat_material_colli, LengthOfArray<Uint16>(meshlist_020BFDBC), LengthOfArray<Uint16>(mat_material_colli), { -193.75f, 1000, 3327.9f }, 1719.251f, NULL };

NJS_OBJECT ht1_dz1 = { NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL | NJD_EVAL_BREAK, &attach_020BFE20, 0, 0, 0, 0, 0, 0, 1, 1, 1, NULL, NULL };

Sint16 bsc_attach_ht2dz_p0[] = {
	8, 4, 2, 3, 0, 1, 5, 6, 12,
	0x8000u | 5, 6, 1, 7, 3, 8,
	0x8000u | 5, 8, 9, 7, 10, 11
};

NJS_MESHSET_SADX bsc_attach_ht2dz_set[] = {
	{ NJD_MESHSET_TRIMESH | 0, 3, bsc_attach_ht2dz_p0, NULL, NULL, NULL, NULL, NULL }
};

NJS_VECTOR bsc_attach_ht2dz_pos[] = {
	{ -1338.461f, 0, -128.5364f },
	{ 42.20105f, 0, 284.5764f },
	{ -1577.704f, 0, -1135.796f },
	{ 42.20105f, 0, -1330.1f },
	{ -717.9066f, 0, -1561.996f },
	{ -1403.465f, 0, 1170.448f },
	{ -80.99695f, 0, 1732.086f },
	{ 438.6595f, 0, 163.7772f },
	{ 988.7395f, 0, -1110.688f },
	{ 1663.774f, 0, -331.5251f },
	{ 1663.774f, 0, 190.0316f },
	{ 1099.565f, 0, 413.9822f },
	{ -820.3823f, 0, 1643.74f }
};

NJS_VECTOR bsc_attach_ht2dz_nrm[] = {
	{ 0, 1, 0 },
	{ 0, 1, 0 },
	{ 0, 1, 0 },
	{ 0, 0.9999695f, 0 },
	{ 0, 1, 0 },
	{ 0, 0.9999695f, 0 },
	{ 0, 1, 0 },
	{ 0, 1, 0 },
	{ 0, 1, 0 },
	{ 0, 1, 0 },
	{ 0, 1, 0 },
	{ 0, 1, 0 },
	{ 0, 1, 0 }
};

NJS_MODEL_SADX bsc_attach_ht2dz = { bsc_attach_ht2dz_pos, bsc_attach_ht2dz_nrm, LengthOfArray<Sint32>(bsc_attach_ht2dz_pos), bsc_attach_ht2dz_set, mat_material_colli, LengthOfArray<Uint16>(bsc_attach_ht2dz_set), LengthOfArray<Uint16>(mat_material_colli), { 43.03528f, 0, 85.04474f }, 2029.1f, NULL };

NJS_OBJECT ht2_dz = { NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL | NJD_EVAL_BREAK | NJD_EVAL_SHAPE_SKIP, &bsc_attach_ht2dz, 1279.172f, -350, 968.6458f, 0, 0, 0, 1, 1, 1, NULL, NULL };

DeathZone hilltope0_deathzones[] = {
	{ CharacterFlags_Sonic, &ht0_dz },
	{ 0 }
};

DeathZone hilltope1_deathzones[] = {
	{ CharacterFlags_Sonic | CharacterFlags_Gamma, &ht1_dz0 },
	{ CharacterFlags_Sonic | CharacterFlags_Gamma, &ht1_dz1 },
	{ 0 }
};

DeathZone hilltope2_deathzones[] = {
	{ CharacterFlags_Knuckles, &ht2_dz },
	{ 0 }
};