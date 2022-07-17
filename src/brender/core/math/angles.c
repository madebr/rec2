#include "angles.h"

#include "matrix34.h"
#include "matrix4.h"
#include "quat.h"

order_info OrderAxes[32];

br_matrix34* (C2_HOOK_CDECL * BrEulerToMatrix34_original)(br_matrix34* mat, const br_euler* euler);
br_matrix34* C2_HOOK_CDECL BrEulerToMatrix34(br_matrix34* mat, const br_euler* euler) {
#if defined(C2_HOOKS_ENABLED)
    return BrEulerToMatrix34_original(mat, euler);
#else
    br_uint_8 o;
    br_angle ti;
    br_angle tj;
    br_angle th;
    br_scalar ci;
    br_scalar cj;
    br_scalar ch;
    br_scalar si;
    br_scalar sj;
    br_scalar sh;
    br_scalar cc;
    br_scalar cs;
    br_scalar sc;
    br_scalar ss;
    int a0;
    int a1;
    int a2;
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00531de0, BrEulerToMatrix34, BrEulerToMatrix34_original)

br_euler* (C2_HOOK_CDECL * BrMatrix34ToEuler_original)(br_euler* euler, const br_matrix34* mat);
br_euler* C2_HOOK_CDECL BrMatrix34ToEuler(br_euler* euler, const br_matrix34* mat) {

#if defined(C2_HOOKS_ENABLED)
    return BrMatrix34ToEuler_original(euler, mat);
#else
    br_uint_8 o;
    int a0;
    int a1;
    int a2;
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005320d0, BrMatrix34ToEuler, BrMatrix34ToEuler_original)

br_matrix4* C2_HOOK_CDECL BrEulerToMatrix4(br_matrix4* mat, const br_euler* euler) {
    br_matrix34 tmp;

    BrEulerToMatrix34(&tmp, euler);
    BrMatrix4Copy34(mat, &tmp);
    return mat;
}
C2_HOOK_FUNCTION(0x00532320, BrEulerToMatrix4)

br_euler* C2_HOOK_CDECL BrMatrix4ToEuler(br_euler* dest, const br_matrix4* mat) {
    br_matrix34 tmp;

    BrMatrix34Copy4(&tmp, mat);
    return BrMatrix34ToEuler(dest, &tmp);
}
C2_HOOK_FUNCTION(0x00532350, BrMatrix4ToEuler)

br_quat* (C2_HOOK_CDECL * BrEulerToQuat_original)(br_quat* q, const br_euler* euler);
br_quat* C2_HOOK_CDECL BrEulerToQuat(br_quat* q, const br_euler* euler) {
#if defined(C2_HOOKS_ENABLED)
    return BrEulerToQuat_original(q, euler);
#else
    br_uint_8 o;
    br_angle ti;
    br_angle tj;
    br_angle th;
    br_scalar ci;
    br_scalar cj;
    br_scalar ch;
    br_scalar si;
    br_scalar sj;
    br_scalar sh;
    br_scalar cc;
    br_scalar cs;
    br_scalar sc;
    br_scalar ss;
    int a0;
    int a1;
    int a2;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00532380, BrEulerToQuat, BrEulerToQuat_original)

br_euler* C2_HOOK_CDECL BrQuatToEuler(br_euler* euler, const br_quat* q) {
    br_matrix34 mat;

    BrQuatToMatrix34(&mat, q);
    return BrMatrix34ToEuler(euler, &mat);
}
C2_HOOK_FUNCTION(0x005325a0, BrQuatToEuler)
