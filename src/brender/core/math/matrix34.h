#ifndef REC2_MATRIX34_H
#define REC2_MATRIX34_H

#include "c2_hooks.h"

#include "brender/br_types.h"

void C2_HOOK_CDECL BrMatrix34Copy(br_matrix34* A, const br_matrix34* B);

void C2_HOOK_CDECL BrMatrix34Mul(br_matrix34* A, const br_matrix34* B, const br_matrix34* C);

void C2_HOOK_CDECL BrMatrix34Identity(br_matrix34* mat);

void C2_HOOK_CDECL BrMatrix34RotateX(br_matrix34* mat, br_angle rx);

void C2_HOOK_CDECL BrMatrix34RotateY(br_matrix34* mat, br_angle ry);

void C2_HOOK_CDECL BrMatrix34RotateZ(br_matrix34* mat, br_angle rz);

void C2_HOOK_CDECL BrMatrix34Rotate(br_matrix34* mat, br_angle r, const br_vector3* a);

void C2_HOOK_CDECL BrMatrix34Translate(br_matrix34* mat, br_scalar dx, br_scalar dy, br_scalar dz);

void C2_HOOK_CDECL BrMatrix34Scale(br_matrix34* mat, br_scalar sx, br_scalar sy, br_scalar sz);

void C2_HOOK_CDECL BrMatrix34ShearX(br_matrix34* mat, br_scalar sy, br_scalar sz);

void C2_HOOK_CDECL BrMatrix34ShearY(br_matrix34* mat, br_scalar sx, br_scalar sz);

void C2_HOOK_CDECL BrMatrix34ShearZ(br_matrix34* mat, br_scalar sx, br_scalar sy);

br_scalar BrMatrix34Inverse(br_matrix34* B, const br_matrix34* A);

void C2_HOOK_CDECL BrMatrix34LPInverse(br_matrix34* A, const br_matrix34* B);

void C2_HOOK_CDECL BrMatrix34LPNormalise(br_matrix34* A, const br_matrix34* B);

void C2_HOOK_CDECL BrMatrix34RollingBall(br_matrix34* mat, int dx, int dy, int radius);

br_matrix34* BrBoundsToMatrix34(br_matrix34* mat, const br_bounds* bounds);

void C2_HOOK_CDECL BrMatrix34Copy4(br_matrix34* A, const br_matrix4* B);

void C2_HOOK_CDECL BrMatrix34TApplyFV(br_vector3* A, const br_fvector3* B, const br_matrix34* C);

void C2_HOOK_CDECL BrMatrix34Apply(br_vector3* A, const br_vector4* B, const br_matrix34* C);

void C2_HOOK_CDECL BrMatrix34ApplyP(br_vector3* A, const br_vector3* B, const br_matrix34* C);

void C2_HOOK_CDECL BrMatrix34ApplyV(br_vector3* A, const br_vector3* B, const br_matrix34* C);

void C2_HOOK_CDECL BrMatrix34TApply(br_vector4* A, const br_vector4* B, const br_matrix34* C);

void C2_HOOK_CDECL BrMatrix34TApplyP(br_vector3* A, const br_vector3* B, const br_matrix34* C);

void C2_HOOK_CDECL BrMatrix34TApplyV(br_vector3* A, const br_vector3* B, const br_matrix34* C);

void C2_HOOK_CDECL BrMatrix34Pre(br_matrix34* mat, const br_matrix34* A);

void C2_HOOK_CDECL BrMatrix34Post(br_matrix34* mat, const br_matrix34* A);

void C2_HOOK_CDECL BrMatrix34PreRotateX(br_matrix34* mat, br_angle rx);

void C2_HOOK_CDECL BrMatrix34PostRotateX(br_matrix34* mat, br_angle rx);

void C2_HOOK_CDECL BrMatrix34PreRotateY(br_matrix34* mat, br_angle ry);

void C2_HOOK_CDECL BrMatrix34PostRotateY(br_matrix34* mat, br_angle ry);

void C2_HOOK_CDECL BrMatrix34PreRotateZ(br_matrix34* mat, br_angle rz);

void C2_HOOK_CDECL BrMatrix34PostRotateZ(br_matrix34* mat, br_angle rz);

void C2_HOOK_CDECL BrMatrix34PreRotate(br_matrix34* mat, br_angle r, const br_vector3* axis);

void C2_HOOK_CDECL BrMatrix34PostRotate(br_matrix34* mat, br_angle r, const br_vector3* axis);

void C2_HOOK_CDECL BrMatrix34PreTranslate(br_matrix34* mat, br_scalar x, br_scalar y, br_scalar z);

void C2_HOOK_CDECL BrMatrix34PostTranslate(br_matrix34* mat, br_scalar x, br_scalar y, br_scalar z);

void C2_HOOK_CDECL BrMatrix34PreScale(br_matrix34* mat, br_scalar sx, br_scalar sy, br_scalar sz);

void C2_HOOK_CDECL BrMatrix34PostScale(br_matrix34* mat, br_scalar sx, br_scalar sy, br_scalar sz);

void C2_HOOK_CDECL BrMatrix34PreShearX(br_matrix34* mat, br_scalar sy, br_scalar sz);

void C2_HOOK_CDECL BrMatrix34PostShearX(br_matrix34* mat, br_scalar sy, br_scalar sz);

void C2_HOOK_CDECL BrMatrix34PreShearY(br_matrix34* mat, br_scalar sx, br_scalar sz);

void C2_HOOK_CDECL BrMatrix34PostShearY(br_matrix34* mat, br_scalar sx, br_scalar sz);

void C2_HOOK_CDECL BrMatrix34PreShearZ(br_matrix34* mat, br_scalar sx, br_scalar sy);

void C2_HOOK_CDECL BrMatrix34PostShearZ(br_matrix34* mat, br_scalar sx, br_scalar sy);

#endif // REC2_MATRIX34_H
