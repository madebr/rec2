#ifndef REC2_VECTOR_H
#define REC2_VECTOR_H

#include "c2_hooks.h"

#include "brender/br_types.h"

void C2_HOOK_CDECL BrVector2Copy(br_vector2* v1, br_vector2* v2);

void C2_HOOK_CDECL BrVector2Set(br_vector2* v1, br_scalar s1, br_scalar s2);

void C2_HOOK_CDECL BrVector2SetInt(br_vector2* v1, int i1, int i2);

void C2_HOOK_CDECL BrVector2SetFloat(br_vector2* v1, float f1, float f2);

void C2_HOOK_CDECL BrVector2Negate(br_vector2* v1, br_vector2* v2);

void C2_HOOK_CDECL BrVector2Add(br_vector2* v1, br_vector2* v2, br_vector2* v3);

void C2_HOOK_CDECL BrVector2Accumulate(br_vector2* v1, br_vector2* v2);

void C2_HOOK_CDECL BrVector2Sub(br_vector2* v1, br_vector2* v2, br_vector2* v3);

void C2_HOOK_CDECL BrVector2Scale(br_vector2* v1, br_vector2* v2, br_scalar s);

void C2_HOOK_CDECL BrVector2InvScale(br_vector2* v1, br_vector2* v2, br_scalar s);

br_scalar C2_HOOK_CDECL BrVector2Dot(br_vector2* v1, br_vector2* v2);

br_scalar C2_HOOK_CDECL BrVector2Length(br_vector2* v1);

br_scalar C2_HOOK_CDECL BrVector2LengthSquared(br_vector2* v1);

void C2_HOOK_CDECL BrVector3Copy(br_vector3* v1, br_vector3* v2);

void C2_HOOK_CDECL BrVector3Set(br_vector3* v1, br_scalar s1, br_scalar s2, br_scalar s3);

void C2_HOOK_CDECL BrVector3SetInt(br_vector3* v1, int i1, int i2, int i3);

void C2_HOOK_CDECL BrVector3SetFloat(br_vector3* v1, float f1, float f2, float f3);

void C2_HOOK_CDECL BrVector3Negate(br_vector3* v1, br_vector3* v2);

void C2_HOOK_CDECL BrVector3Add(br_vector3* v1, br_vector3* v2, br_vector3* v3);

void C2_HOOK_CDECL BrVector3Accumulate(br_vector3* v1, br_vector3* v2);

void C2_HOOK_CDECL BrVector3Sub(br_vector3* v1, br_vector3* v2, br_vector3* v3);

void C2_HOOK_CDECL BrVector3Scale(br_vector3* v1, br_vector3* v2, br_scalar s);

void C2_HOOK_CDECL BrVector3InvScale(br_vector3* v1, br_vector3* v2, br_scalar s);

br_scalar C2_HOOK_CDECL BrVector3Dot(br_vector3* v1, br_vector3* v2);

void C2_HOOK_CDECL BrVector3Cross(br_vector3* v1, br_vector3* v2, br_vector3* v3);

br_scalar C2_HOOK_CDECL BrVector3Length(br_vector3* v1);

br_scalar C2_HOOK_CDECL BrVector3LengthSquared(br_vector3* v1);

void C2_HOOK_CDECL BrVector3Normalise(br_vector3* v1, br_vector3* v2);

void C2_HOOK_CDECL BrVector3NormaliseLP(br_vector3* v1, br_vector3* v2);

br_scalar C2_HOOK_CDECL BrVector4Dot(br_vector4* v1, br_vector4* v2);

void C2_HOOK_CDECL BrVector4Copy(br_vector4* v1, br_vector4* v2);

br_scalar C2_HOOK_CDECL BrFVector2Dot(br_fvector2* v1, br_vector2* v2);

void C2_HOOK_CDECL BrFVector3Copy(br_fvector3* v1, br_vector3* v2);

void C2_HOOK_CDECL BrVector3ScaleF(br_vector3* v1, br_fvector3* v2, br_scalar s);

br_scalar C2_HOOK_CDECL BrFVector3Dot(br_fvector3* v1, br_vector3* v2);

void C2_HOOK_CDECL BrFVector3Normalise(br_fvector3* v1, br_vector3* v2);

void C2_HOOK_CDECL BrFVector3NormaliseLP(br_fvector3* v1, br_vector3* v2);

void C2_HOOK_CDECL BrVector2Normalise(br_vector2* v1, br_vector2* v2);

#endif
