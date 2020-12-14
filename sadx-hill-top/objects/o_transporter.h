#pragma once

extern ModelInfo* ht_transporter;
extern ModelInfo* ht_transportercol;
extern ModelInfo* ht_vine;

void DrawVine(NJS_OBJECT* vine, NJS_VECTOR* orig, float Y, float Z, float progress);

void __cdecl HillTransporter(ObjectMaster* obj);