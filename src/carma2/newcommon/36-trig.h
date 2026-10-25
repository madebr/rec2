#ifndef GUARD_36_TRIG_H
#define GUARD_36_TRIG_H

#include "brender/brender.h"
#include "c2_hooks.h"

extern float gFloat_sine_table[91];

// FastFloatSin

// FastFloatCos

// FastScalarSin

// FastScalarCos

// FastScalarSinAngle

// FastScalarCosAngle

extern float C2_HOOK_STDCALL FastFloatArcSin(float pValue);

extern float C2_HOOK_STDCALL FastFloatArcCos(float pValue);

extern float C2_HOOK_STDCALL FastFloatArcTan2(float pY, float pX);

extern br_scalar C2_HOOK_STDCALL FastScalarArcTan2(br_scalar pY, br_scalar pX);

extern br_angle C2_HOOK_STDCALL FastFloatArcTan2Angle(float pY, float pX);

extern br_angle C2_HOOK_STDCALL FastScalarArcTan2Angle(br_scalar pY, br_scalar pX);

// DRMatrix34RotateX

// DRMatrix34RotateY

// DRMatrix34RotateZ

// DRMatrix34PostRotateX

extern void C2_HOOK_FASTCALL DRMatrix34PostRotateY(br_matrix34* mat, br_angle ry);

// DRMatrix34PreRotateZ

// DRMatrix34PostRotateZ

// DRMatrix23Rotate

// DRMatrix23PostRotate

#endif // GUARD_36_TRIG_H
