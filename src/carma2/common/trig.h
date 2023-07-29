#ifndef REC2_TRIG_H
#define REC2_TRIG_H

#include "c2_hooks.h"

#include <brender/brender.h>

float C2_HOOK_FASTCALL FastFloatSin(int pAngle_in_degrees);

float C2_HOOK_FASTCALL FastFloatCos(int pAngle_in_degrees);

float C2_HOOK_FASTCALL FastFloatTan(int pAngle_in_degrees);

br_scalar C2_HOOK_FASTCALL FastScalarSin(int pAngle_in_degrees);

br_scalar C2_HOOK_FASTCALL FastScalarCos(int pAngle_in_degrees);

br_scalar C2_HOOK_FASTCALL FastScalarTan(int pAngle_in_degrees);

br_scalar C2_HOOK_FASTCALL FastScalarSinAngle(br_angle pBR_angle);

br_scalar C2_HOOK_FASTCALL FastScalarCosAngle(br_angle pBR_angle);

br_scalar C2_HOOK_FASTCALL FastScalarTanAngle(br_angle pBR_angle);

float C2_HOOK_STDCALL FastFloatArcSin(float pValue);

float C2_HOOK_STDCALL FastFloatArcCos(float pValue);

br_scalar C2_HOOK_STDCALL FastScalarArcSin(br_scalar pValue);

br_scalar C2_HOOK_STDCALL FastScalarArcCos(br_scalar pValue);

float C2_HOOK_STDCALL FastFloatArcTan2(float pY, float pX);

br_scalar C2_HOOK_STDCALL FastScalarArcTan2(br_scalar pY, br_scalar pX);

br_angle C2_HOOK_STDCALL FastFloatArcTan2Angle(float pY, float pX);

br_angle C2_HOOK_STDCALL FastScalarArcTan2Angle(br_scalar pY, br_scalar pX);

void C2_HOOK_FASTCALL DRMatrix34RotateX(br_matrix34* mat, br_angle rx);

void C2_HOOK_FASTCALL DRMatrix34RotateY(br_matrix34* mat, br_angle ry);

void C2_HOOK_FASTCALL DRMatrix34RotateZ(br_matrix34* mat, br_angle rz);

void C2_HOOK_FASTCALL DRMatrix34Rotate(br_matrix34* mat, br_angle r, br_vector3* a);

void C2_HOOK_FASTCALL DRMatrix34PreRotateX(br_matrix34* mat, br_angle rx);

void C2_HOOK_FASTCALL DRMatrix34PostRotateX(br_matrix34* mat, br_angle rx);

void C2_HOOK_FASTCALL DRMatrix34PreRotateY(br_matrix34* mat, br_angle ry);

void C2_HOOK_FASTCALL DRMatrix34PostRotateY(br_matrix34* mat, br_angle ry);

void C2_HOOK_FASTCALL DRMatrix34PreRotateZ(br_matrix34* mat, br_angle rz);

void C2_HOOK_FASTCALL DRMatrix34PostRotateZ(br_matrix34* mat, br_angle rz);

void C2_HOOK_FASTCALL DRMatrix34PreRotate(br_matrix34* mat, br_angle r, br_vector3* axis);

void C2_HOOK_FASTCALL DRMatrix34PostRotate(br_matrix34* mat, br_angle r, br_vector3* axis);

void C2_HOOK_FASTCALL DRMatrix23Rotate(br_matrix23* mat, br_angle rz);

void C2_HOOK_FASTCALL DRMatrix23PreRotate(br_matrix23* mat, br_angle rz);

#endif //REC2_TRIG_H
