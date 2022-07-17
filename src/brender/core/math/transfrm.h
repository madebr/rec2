#ifndef REC2_TRANSFRM_H
#define REC2_TRANSFRM_H

#include "c2_hooks.h"

#include "brender/br_types.h"

extern br_uint_8 _CombineTransforms[7][7];
#define BrTransformCombineTypes(a, b) (_CombineTransforms[(a)][(b)])
#define BrTransformTypeIsLP(a) ((a) != BR_TRANSFORM_MATRIX34)
#define BrTransformTypeIsMatrix34(a) ((a) <= BR_TRANSFORM_MATRIX34_LP)

void C2_HOOK_CDECL BrTransformToMatrix34(br_matrix34* mat, const br_transform* xform);

void C2_HOOK_CDECL BrMatrix34PreTransform(br_matrix34* mat, const br_transform* xform);

void C2_HOOK_CDECL BrMatrix34PostTransform(br_matrix34* mat, const br_transform* xform);

void C2_HOOK_CDECL BrMatrix4PreTransform(br_matrix4* mat, const br_transform* xform);

void C2_HOOK_CDECL BrMatrix34ToTransform(br_transform* xform, const br_matrix34* mat);

void C2_HOOK_CDECL BrTransformToTransform(br_transform* dest, const br_transform* src);

#endif // REC2_TRANSFRM_H
