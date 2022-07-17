#ifndef RE2C_CUSTSUPT_H
#define RE2C_CUSTSUPT_H

#include "c2_hooks.h"

#include "brender/br_types.h"

C2_HOOK_VARIABLE_DECLARE(br_scalar, gScreenZOffset);

void C2_HOOK_CDECL BrModelToScreenQuery(br_matrix4* dest);

void C2_HOOK_CDECL BrModelToViewQuery(br_matrix34* dest);

br_uint_8 C2_HOOK_CDECL BrOriginToScreenXY(br_vector2* screen);

br_uint_32 C2_HOOK_CDECL BrOriginToScreenXYZO(br_vector3* screen);

br_uint_8 C2_HOOK_CDECL BrPointToScreenXY(br_vector2* screen, const br_vector3* point);

br_uint_32 C2_HOOK_CDECL BrPointToScreenXYZO(br_vector3* screen, const br_vector3* point);

void C2_HOOK_CDECL BrPointToScreenXYMany(br_vector2* screens, const br_vector3* points, br_uint_32 npoints);

void C2_HOOK_CDECL BrPointToScreenXYZOMany(br_vector3* screens, br_uint_32* outcodes, br_vector3* points, br_uint_32 npoints);

br_scalar C2_HOOK_CDECL BrZbDepthToScreenZ(br_uint_32 depth_z, br_camera* camera);

br_uint_32 C2_HOOK_CDECL BrZbScreenZToDepth(br_scalar sz, br_camera* camera);

br_scalar C2_HOOK_CDECL BrZsDepthToScreenZ(br_scalar depth_z, br_camera* camera);

br_scalar C2_HOOK_CDECL BrZsScreenZToDepth(br_scalar sz, br_camera* camera);

br_scalar C2_HOOK_CDECL BrScreenZToCamera(br_actor* camera, br_scalar sz);

void C2_HOOK_CDECL BrScreenXYZToCamera(br_vector3* point, br_actor* camera, br_pixelmap* screen_buffer, br_int_16 x, br_int_16 y, br_scalar sz);

#endif // RE2C_CUSTSUPT_H
