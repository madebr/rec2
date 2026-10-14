#ifndef GUARD_36_TRIG_H
#define GUARD_36_TRIG_H

#include "brender/brender.h"
#include "c2_hooks.h"

extern float gFloat_sine_table[91];
extern br_matrix23 mat23tmp1;
extern br_matrix23 mat23tmp2;
extern br_matrix34 mattmp1__trig;
extern br_matrix34 mattmp2__trig;

extern float C2_HOOK_FASTCALL FastFloatSin(int pAngle_in_degrees);

extern float C2_HOOK_FASTCALL FastFloatCos(int pAngle_in_degrees);

extern br_scalar C2_HOOK_FASTCALL FastScalarSin(int pAngle_in_degrees);

extern br_scalar C2_HOOK_FASTCALL FastScalarCos(int pAngle_in_degrees);

extern br_scalar C2_HOOK_FASTCALL FastScalarSinAngle(br_angle pBR_angle);

extern br_scalar C2_HOOK_FASTCALL FastScalarCosAngle(br_angle pBR_angle);

extern double C2_HOOK_STDCALL FastFloatArcTan2(float pY, float pX);

extern double C2_HOOK_STDCALL FastScalarArcTan2(br_scalar pY, br_scalar pX);

extern br_angle C2_HOOK_STDCALL FastFloatArcTan2Angle(float pY, float pX);

extern br_angle C2_HOOK_STDCALL FastScalarArcTan2Angle(br_scalar pY, br_scalar pX);

extern void C2_HOOK_FASTCALL DRMatrix34RotateX(br_matrix34* mat, br_angle rx);

extern void C2_HOOK_FASTCALL DRMatrix34RotateY(br_matrix34* mat, br_angle ry);

extern void C2_HOOK_FASTCALL DRMatrix34RotateZ(br_matrix34* mat, br_angle rz);

extern void C2_HOOK_FASTCALL DRMatrix34PreRotateX(br_matrix34* mat, br_angle rx);

extern void C2_HOOK_FASTCALL DRMatrix34PostRotateX(br_matrix34* mat, br_angle rx);

extern void C2_HOOK_FASTCALL DRMatrix34PreRotateY(br_matrix34* mat, br_angle ry);

extern void C2_HOOK_FASTCALL DRMatrix34PostRotateY(br_matrix34* mat, br_angle ry);

extern void C2_HOOK_FASTCALL DRMatrix34PreRotateZ(br_matrix34* mat, br_angle rz);

extern void C2_HOOK_FASTCALL DRMatrix34PostRotateZ(br_matrix34* mat, br_angle rz);

extern void C2_HOOK_FASTCALL DRMatrix23Rotate(br_matrix23* mat, br_angle rz);

extern void C2_HOOK_FASTCALL DRMatrix23PreRotate(br_matrix23* mat, br_angle rz);

extern void C2_HOOK_FASTCALL DRMatrix23PostRotate(br_matrix23* mat, br_angle rz);

#endif // GUARD_36_TRIG_H
