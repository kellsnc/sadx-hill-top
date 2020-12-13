#include "pch.h"

#include "objects/o_transporter.h"

Loop HT0_PropPath_Entries[] = {
	{ 0, 0, 977.02916, { 2237.581f,  501.5421f,  592.0755f } },
	{ 0, 0, 362.6166, { 1383.097f,  318.4319f,  1042.736f } },
	{ 0, 0, 242.2421, { 1035.437f,  235.7159f,  1104.229f } },
	{ 0, 0, 283.71554, { 818.5247f,  131.0534f,  1130.23f } },
	{ 0, 0, 81.0502, { 560.077f,  68.81733f,  1229.356f } },
	{ 0, 0, 80.22955, { 495.2714f,  54.95324f,  1276.016f } },
	{ 0, 0, 170.26293, { 441.8804f,  28.43449f,  1329.709f } },
	{ 0, 0, 0, { 376.9286f,  -22.22725f,  1460.423f } },
};

LoopHead HT0_PropPath = { 0, LengthOfArray<int16_t>(HT0_PropPath_Entries), 2197.14608f, HT0_PropPath_Entries, HillTransporterPath };

Loop HT0_LoopPath_Entries[] = {
	{ 0, 0, 31.47127, { 1665.611f,  -1.430883f,  296.7416f } },
	{ 0, 0, 32.14772, { 1695.344f,  6.395791f,  303.4597f } },
	{ 0, 0, 32.09332, { 1723.281f,  21.83468f,  307.286f } },
	{ 0, 0, 32.10897, { 1744.411f,  45.99007f,  307.1389f } },
	{ 0, 0, 27.18877, { 1756.066f,  75.24115f,  300.8523f } },
	{ 0, 0, 29.95169, { 1756.338f,  101.5634f,  294.0483f } },
	{ 0, 0, 31.5541, { 1748.327f,  127.4553f,  281.2993f } },
	{ 0, 0, 32.66741, { 1731.242f,  148.21f,  264.7764f } },
	{ 0, 0, 31.99244, { 1708.054f,  159.1693f,  244.5435f } },
	{ 0, 0, 31.69967, { 1683.183f,  158.0007f,  224.4541f } },
	{ 0, 0, 25.57602, { 1661.095f,  145.3181f,  205.5824f } },
	{ 0, 0, 23.58046, { 1648.198f,  128.4539f,  191.3206f } },
	{ 0, 0, 42.7663, { 1641.418f,  108.6066f,  180.5431f } },
	{ 0, 0, 36.2545, { 1643.715f,  68.83929f,  164.979f } },
	{ 0, 0, 40.8311, { 1659.628f,  36.37862f,  162.2458f } },
	{ 0, 0, 32.19556, { 1690.954f,  10.54801f,  166.5632f } },
	{ 0, 0, 46.06013, { 1721.076f,  1.944771f,  173.9931f } },
	{ 0, 0, 0, { 1764.638f,  1.123087f,  188.9334f } },
};

LoopHead HT0_LoopPath = { 0, LengthOfArray<int16_t>(HT0_LoopPath_Entries), 560.13943f, HT0_LoopPath_Entries, (ObjectFuncPtr)0x4BB1F0 };

LoopHead* hilltop0_paths[]{
	&HT0_PropPath,
	&HT0_LoopPath,
	nullptr
};

PathDataPtr hilltop0_pathdata = { LevelAndActIDs_RedMountain1, hilltop0_paths };

LoopHead* hilltop1_paths[]{
	nullptr
};

PathDataPtr hilltop1_pathdata = { LevelAndActIDs_RedMountain2, hilltop1_paths };

LoopHead* hilltop2_paths[]{
	nullptr
};

PathDataPtr hilltop2_pathdata = { LevelAndActIDs_RedMountain3, hilltop2_paths };