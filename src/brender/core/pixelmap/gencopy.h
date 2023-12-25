#ifndef REC2_GENCOPY_H
#define REC2_GENCOPY_H

#include "c2_hooks.h"

#include "brender/br_types.h"

br_error C2_HOOK_CDECL GeneralRectangleCopy(br_device_pixelmap* dest, br_point* p, br_device_pixelmap* src, br_rectangle* r);

br_error C2_HOOK_CDECL GeneralCopy(br_device_pixelmap* dst, br_device_pixelmap* src);

br_error C2_HOOK_CDECL GeneralRectangleStretchCopy(br_device_pixelmap* dst, br_rectangle* d, br_device_pixelmap* src, br_rectangle* s);

#endif // REC2_GENCOPY_H
