#ifndef REC2_MATRIX23_H
#define REC2_MATRIX23_H

#include "c2_hooks.h"

#include "brender/br_types.h"

void C2_HOOK_CDECL BrMatrix23Copy(br_matrix23* A, br_matrix23* B);

void C2_HOOK_CDECL BrMatrix23Mul(br_matrix23* A, br_matrix23* B, br_matrix23* C);

void C2_HOOK_CDECL BrMatrix23Identity(br_matrix23* mat);

void C2_HOOK_CDECL BrMatrix23Rotate(br_matrix23* mat, br_angle rz);

void C2_HOOK_CDECL BrMatrix23Translate(br_matrix23* mat, br_scalar dx, br_scalar dy);

void C2_HOOK_CDECL BrMatrix23Scale(br_matrix23* mat, br_scalar sx, br_scalar sy);

void C2_HOOK_CDECL BrMatrix23ShearX(br_matrix23* mat, br_scalar sy);

void C2_HOOK_CDECL BrMatrix23ShearY(br_matrix23* mat, br_scalar sx);

br_scalar C2_HOOK_CDECL BrMatrix23Inverse(br_matrix23* B, br_matrix23* A);

void C2_HOOK_CDECL BrMatrix23LPInverse(br_matrix23* B, br_matrix23* A);

void C2_HOOK_CDECL BrMatrix23LPNormalise(br_matrix23* A, br_matrix23* B);

void C2_HOOK_CDECL BrMatrix23ApplyP(br_vector2* A, br_vector2* B, br_matrix23* C);

void C2_HOOK_CDECL BrMatrix23ApplyV(br_vector2* A, br_vector2* B, br_matrix23* C);

void C2_HOOK_CDECL BrMatrix23TApplyP(br_vector2* A, br_vector2* B, br_matrix23* C);

void C2_HOOK_CDECL BrMatrix23TApplyV(br_vector2* A, br_vector2* B, br_matrix23* C);

void C2_HOOK_CDECL BrMatrix23Pre(br_matrix23* mat, br_matrix23* A);

void C2_HOOK_CDECL BrMatrix23Post(br_matrix23* mat, br_matrix23* A);

void C2_HOOK_CDECL BrMatrix23PreRotate(br_matrix23* mat, br_angle rz);

void C2_HOOK_CDECL BrMatrix23PostRotate(br_matrix23* mat, br_angle rz);

void C2_HOOK_CDECL BrMatrix23PreTranslate(br_matrix23* mat, br_scalar x, br_scalar y);

void C2_HOOK_CDECL BrMatrix23PostTranslate(br_matrix23* A, br_scalar x, br_scalar y);

void C2_HOOK_CDECL BrMatrix23PreScale(br_matrix23* mat, br_scalar sx, br_scalar sy);

void C2_HOOK_CDECL BrMatrix23PostScale(br_matrix23* mat, br_scalar sx, br_scalar sy);

void C2_HOOK_CDECL BrMatrix23PreShearX(br_matrix23* mat, br_scalar sy);

void C2_HOOK_CDECL BrMatrix23PostShearX(br_matrix23* mat, br_scalar sy);

void C2_HOOK_CDECL BrMatrix23PreShearY(br_matrix23* mat, br_scalar sx);

void C2_HOOK_CDECL BrMatrix23PostShearY(br_matrix23* mat, br_scalar sx);

#endif // REC2_MATRIX23_H
