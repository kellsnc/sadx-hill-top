#include "pch.h"
#include "o_hilltree.h"

/*

An object that spawn different kind of trees.

ScaleX: Global scale
ScaleY: Additional Y scale
ScaleZ: Tree ID

*/

ModelInfo* ht_tree = nullptr;

static CCL_INFO HillTreeColM = { 0, CI_FORM_CAPSULE, 0x77, 0, 0, {0.0f, 25.0f, 0.0f}, 5.0f, 25.0f, 0.0f, 0.0f, 0, 0, 0 };
static CCL_INFO HillTreeColS = { 0, CI_FORM_CAPSULE, 0x77, 0, 0, {0.0f, 20.0f, 0.0f}, 1.0f, 20.0f, 0.0f, 0.0f, 0, 0, 0 };

void __cdecl HillTreeDisplay(task* tp)
{
	if (!MissedFrames)
	{
		auto twp = tp->twp;

		// Skip if not visible on screen
		if (!IsVisible(&twp->pos, 30.0f * (twp->scl.x + twp->scl.y)))
		{
			return;
		}

		auto object = (NJS_OBJECT*)twp->value.ptr;

		SetSecondObjectTexture();
		njPushMatrixEx();
		njTranslateEx(&twp->pos);
		njRotateEx((Angle*)&twp->ang, false);

		// Draw base:

		njPushMatrixEx();
		njScale(nullptr, twp->scl.x, twp->scl.x + twp->scl.y, twp->scl.x);
		dsDrawModel(object->basicdxmodel);
		njPopMatrixEx();

		// Draw branches:

		NJS_OBJECT* branch = object->child;

		if (branch)
		{
			njScalef(twp->scl.x);
			njTranslateY(15.0f * twp->scl.y);

			while (branch)
			{
				DrawObjectRoot(branch);
				branch = branch->sibling;
			}
		}
		
		njPopMatrixEx();
	}
}

void __cdecl HillTreeExec(task* tp)
{
	if (!CheckRangeOut(tp))
	{
		EntryColliList(tp->twp);
		tp->disp(tp);
	}
}

void __cdecl HillTree(task* tp)
{
	auto twp = tp->twp;

	// Choose the model and collison based on set information
	switch (static_cast<int>(twp->scl.z) % 5)
	{
	case 0:
	default:
		twp->value.ptr = ht_tree->getmodel()->child;
		CCL_Init(tp, &HillTreeColM, 1, 4);
		break;
	case 1:
		twp->value.ptr = ht_tree->getmodel()->child->sibling;
		CCL_Init(tp, &HillTreeColS, 1, 4);
		break;
	case 2:
		twp->value.ptr = ht_tree->getmodel()->child->sibling->sibling;
		CCL_Init(tp, &HillTreeColS, 1, 4);
		break;
	case 3:
		twp->value.ptr = ht_tree->getmodel()->child->sibling->sibling->sibling;
		break;
	case 4:
		twp->value.ptr = ht_tree->getmodel()->child->sibling->sibling->sibling->sibling;
		CCL_Init(tp, &HillTreeColM, 1, 4);
		break;
	}

	if (twp->scl.x == 0)
	{
		twp->scl.x = 1; // default scale
	}

	auto cwp = twp->cwp;

	if (cwp)
	{
		auto cclinfo = cwp->info;

		// Adjust center with scale
		cclinfo->center.y *= twp->scl.x + twp->scl.y;

		// Adjust collision size with scale
		cclinfo->a *= twp->scl.x;
		cclinfo->b *= twp->scl.x + twp->scl.y;
	}

	tp->exec = HillTreeExec;
	tp->disp = HillTreeDisplay;
}

void HillTree_LoadAssets()
{
	LoadModelFile(&ht_tree, "ht_tree", ModelFormat_Basic);
}

void HillTree_FreeAssets()
{
	FreeFileInfo(&ht_tree);
}