#include "pch.h"
#include "Trampoline.h"

/*

Code to replace only a specific level card
We simply load a PVM(X) with textures using the same GBIX as the textures being replaced
The game will load our textures, and ignore the original textures since they use the same gbix.

Each menu load their texture using lists of texture indices
We loop through the passed list until we find 11 (AVA_STNAM)

*/

NJS_TEXNAME HTMENU_TEXNAMES[2] = {};
NJS_TEXLIST HTMENU_TEXLIST = {arrayptrandlength(HTMENU_TEXNAMES)};

Trampoline* LoadMenuTexs_t = nullptr;

void __cdecl LoadMenuTexs_r(int* IDs) {
	int* ID = &IDs[0];

	while (*ID != 30) {
		if (*ID == 11) {
			LoadPVM("HT_STAGENAME", &HTMENU_TEXLIST);
		}

		++ID;
	}

	NonStaticFunctionPointer(void, original, (int*), LoadMenuTexs_t->Target());
	original(IDs);
}

static const int sub_505F40 = 0x505F40;

static inline int sub_505F40_call(int* IDs) {
	int result;
	__asm {
		mov eax, IDs
		call sub_505F40
		mov result, eax
	}
	return result;
}

int __cdecl ReleaseMenuTexs_r(int* IDs) {
	int* ID = &IDs[0];

	while (*ID != 30) {
		if (*ID == 11) {
			njReleaseTexture(&HTMENU_TEXLIST);
		}

		++ID;
	}

	return sub_505F40_call(IDs);
}

static void __declspec(naked) sub_505F40_asm() {
	__asm {
		push eax
		call ReleaseMenuTexs_r
		add esp, 4
		retn
	}
}

void Menu_Init() {
	LoadMenuTexs_t = new Trampoline(0x505ED0, 0x505ED5, LoadMenuTexs_r);
	WriteCall((void*)0x5060BC, sub_505F40_asm);
	WriteCall((void*)0x505FFB, sub_505F40_asm);
}