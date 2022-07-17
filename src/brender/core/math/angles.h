#ifndef REC2_ANGLES_H
#define REC2_ANGLES_H

#include "c2_hooks.h"

#include "brender/br_types.h"

#define BR_EULER_XYZ_S 0
#define BR_EULER_XYX_S 8
#define BR_EULER_XZY_S 4
#define BR_EULER_XZX_S 12
#define BR_EULER_YZX_S 1
#define BR_EULER_YZY_S 9
#define BR_EULER_YXZ_S 5
#define BR_EULER_YXY_S 13
#define BR_EULER_ZXY_S 2
#define BR_EULER_ZXZ_S 10
#define BR_EULER_ZYX_S 6
#define BR_EULER_ZYZ_S 14
#define BR_EULER_ZYX_R 16
#define BR_EULER_XYX_R 24
#define BR_EULER_YZX_R 20
#define BR_EULER_XZX_R 28
#define BR_EULER_XZY_R 17
#define BR_EULER_YZY_R 25
#define BR_EULER_ZXY_R 21
#define BR_EULER_YXY_R 29
#define BR_EULER_YXZ_R 18
#define BR_EULER_ZXZ_R 26
#define BR_EULER_XYZ_R 22
#define BR_EULER_ZYZ_R 30

br_matrix34* C2_HOOK_CDECL BrEulerToMatrix34(br_matrix34* mat, const br_euler* euler);

br_euler* C2_HOOK_CDECL BrMatrix34ToEuler(br_euler* euler, const br_matrix34* mat);

br_matrix4* C2_HOOK_CDECL BrEulerToMatrix4(br_matrix4* mat, const br_euler* euler);

br_euler* C2_HOOK_CDECL BrMatrix4ToEuler(br_euler* dest, const br_matrix4* mat);

br_quat* C2_HOOK_CDECL BrEulerToQuat(br_quat* q, const br_euler* euler);

br_euler* C2_HOOK_CDECL BrQuatToEuler(br_euler* euler, const br_quat* q);

#endif // REC2_ANGLES_H
