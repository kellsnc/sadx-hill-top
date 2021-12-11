#include "pch.h"
#include "o_hillpole.h"
#include "o_transporter.h"

/*

Simple object that just draw poles (for bridges.)

*/

CCL_INFO HillPole_Col = { 0, CI_FORM_CAPSULE, 0x77, 0, 0, {0.0f, 20.0f, 0.0f}, 2.0f, 20.0f, 0.0f, 0.0f, 0, 0, 0 };

void __cdecl HillPoleDisplay(task* tp)
{
	if (!MissedFrames)
	{
		auto twp = tp->twp;
		auto object = (NJS_OBJECT*)twp->value.ptr;

		SetSecondObjectTexture();
		njPushMatrixEx();
		njTranslateEx(&twp->pos);
		DrawModel(object->basicdxmodel);
		njPopMatrixEx();
	}
}

void __cdecl HillPoleExec(task* tp)
{
	if (!CheckRangeOut(tp))
	{
		EntryColliList(tp->twp);
		tp->disp(tp);
	}
}

void __cdecl HillPole(task* tp)
{
	tp->twp->value.ptr = ht_transporter->getmodel()->child->sibling;
	CCL_Init(tp, &HillPole_Col, 1, 4);
	tp->exec = HillPoleExec;
	tp->disp = HillPoleDisplay;
}