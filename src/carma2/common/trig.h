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

#endif //REC2_TRIG_H
