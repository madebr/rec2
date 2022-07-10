#ifndef REC2_QUAT_H
#define REC2_QUAT_H

#include "c2_hooks.h"

#include "brender/br_types.h"

br_quat* C2_HOOK_CDECL BrQuatMul(br_quat* q, br_quat* l, br_quat* r);

br_quat* C2_HOOK_CDECL BrQuatNormalise(br_quat* q, br_quat* qq);

br_quat* C2_HOOK_CDECL BrQuatInvert(br_quat* q, br_quat* qq);

br_quat* C2_HOOK_CDECL BrQuatSlerp(br_quat* q, br_quat* l, br_quat* r, br_scalar a, br_int_16 spins);

br_matrix34* C2_HOOK_CDECL BrQuatToMatrix34(br_matrix34* mat, br_quat* q);

br_quat* C2_HOOK_CDECL BrMatrix34ToQuat(br_quat* q, br_matrix34* mat);

br_matrix4* C2_HOOK_CDECL BrQuatToMatrix4(br_matrix4* mat, br_quat* q);

br_quat* C2_HOOK_CDECL BrMatrix4ToQuat(br_quat* q, br_matrix4* mat);

#endif // REC2_QUAT_H
