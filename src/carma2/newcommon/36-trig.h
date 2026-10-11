#ifndef GUARD_36_TRIG_H
#define GUARD_36_TRIG_H

#include "brender/brender.h"
#include "c2_hooks.h"

extern float gFloat_sine_table[91];

extern float C2_HOOK_FASTCALL FastFloatSin(int pAngle_in_degrees);

extern float C2_HOOK_FASTCALL FastFloatCos(int pAngle_in_degrees);

extern br_scalar C2_HOOK_FASTCALL FastScalarSin(int pAngle_in_degrees);

extern br_scalar C2_HOOK_FASTCALL FastScalarCos(int pAngle_in_degrees);

extern br_scalar C2_HOOK_FASTCALL FastScalarSinAngle(br_angle pBR_angle);

extern br_scalar C2_HOOK_FASTCALL FastScalarCosAngle(br_angle pBR_angle);

// FastFloatArcTan2

// FastScalarArcTan2

// FastFloatArcTan2Angle

// FastScalarArcTan2Angle

extern void C2_HOOK_FASTCALL DRMatrix34RotateX(br_matrix34* mat, br_angle rx);

extern void C2_HOOK_FASTCALL DRMatrix34RotateY(br_matrix34* mat, br_angle ry);

extern void C2_HOOK_FASTCALL DRMatrix34RotateZ(br_matrix34* mat, br_angle rz);

// DRMatrix34PostRotateX

// DRMatrix34PostRotateY

// DRMatrix34PreRotateZ

// DRMatrix34PostRotateZ

extern void C2_HOOK_FASTCALL DRMatrix23Rotate(br_matrix23* mat, br_angle rz);

// DRMatrix23PostRotate

#endif // GUARD_36_TRIG_H
