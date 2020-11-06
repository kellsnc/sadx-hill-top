#include "pch.h"

void DrawObject_Queue(NJS_OBJECT* object, QueuedModelFlagsB flags) {
	njPushMatrixEx();
	njTranslate(nullptr, Pos3(object->pos));
	njRotateXYZ(nullptr, Pos3(object->ang));
	DrawModel_Queue(object->basicdxmodel, QueuedModelFlagsB_EnableZWrite);
	njPopMatrixEx();
}

void DrawObjectRoot(NJS_OBJECT* object) {
	njPushMatrixEx();
	njTranslate(nullptr, Pos3(object->pos));
	njRotateXYZ(nullptr, Pos3(object->ang));
	njScale(nullptr, Pos3(object->scl));
	njDrawModel_SADX(object->basicdxmodel);
	njPopMatrixEx();
}

void DrawChunkModel(NJS_CNK_MODEL* model) {
	DrawChunkModel_(model->vlist, model->plist);
}

void njCnkAction_Queue(NJS_ACTION* action, float frame, QueuedModelFlagsB flags) {
	DisplayAnimationFrame(action, frame, flags, 0, (void(__cdecl*)(NJS_MODEL_SADX*, int, int))DrawChunkModel);
}

void njCnkAction(NJS_ACTION* action, float frame) {
	DisplayAnimationFrame(action, frame, (QueuedModelFlagsB)0, 0, (void(__cdecl*)(NJS_MODEL_SADX*, int, int))DrawChunkModel);
}

void SetupWorldMatrix() {
	ProjectToWorldSpace();
	Direct3D_SetWorldTransform();
	memcpy(_nj_current_matrix_ptr_, &ViewMatrix, sizeof(NJS_MATRIX));
}

void njTranslateX(float f) {
	njTranslate(nullptr, f, 0, 0);
}

void njTranslateY(float f) {
	njTranslate(nullptr, 0, f, 0);
}

void njTranslateZ(float f) {
	njTranslate(nullptr, 0, 0, f);
}

void njRotateZXY(Rotation3* rot) {
	njRotateZ(0, rot->z);
	njRotateX(0, rot->x);
	njRotateY(0, rot->y);
}

void njRotateYXZ(Rotation3* rot) {
	njRotateY(0, rot->y);
	njRotateX(0, rot->x);
	njRotateZ(0, rot->z);
}

void njScaleX(float f) {
	njScale(nullptr, f, 1, 1);
}

void njScaleY(float f) {
	njScale(nullptr, 1, f, 1);
}

void njScaleZ(float f) {
	njScale(nullptr, 1, 1, f);
}

void njScalef(float f) {
	njScale(nullptr, f, f, f);
}

void njRotateX_(Angle x) {
	if (x) {
		njRotateX(_nj_current_matrix_ptr_, x);
	}
}

void njRotateY_(Angle y) {
	if (y) {
		njRotateY(_nj_current_matrix_ptr_, y);
	}
}

void njRotateZ_(Angle z) {
	if (z) {
		njRotateZ(_nj_current_matrix_ptr_, z);
	}
}

void njLookAt(NJS_VECTOR* from, NJS_VECTOR* to, Angle* outx, Angle* outy) {
	NJS_VECTOR unit = *to;

	njSubVector(&unit, from);

	if (outy) {
		*outy = static_cast<Angle>(atan2f(unit.x, unit.z) * 65536.0f * 0.1591549762031479f);
	}

	if (outx) {
		if (from->y == to->y) {
			*outx = 0;
		}
		else {
			Float len = 1.0f / squareroot(unit.z * unit.z + unit.x * unit.x + unit.y * unit.y);

			*outx = static_cast<Angle>((acos(len * 3.3499999f) * 65536.0f * 0.1591549762031479f)
				- (acos(-(len * unit.y)) * 65536.0f * 0.1591549762031479f));
		}
	}
}