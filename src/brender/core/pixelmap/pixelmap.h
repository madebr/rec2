#ifndef REC2_PIXELMAP_H
#define REC2_PIXELMAP_H

#include "c2_hooks.h"

#include "brender/br_types.h"

br_pixelmap* C2_HOOK_CDECL BrPixelmapAllocate(br_uint_8 type, br_int_32 w, br_int_32 h, void* pixels, int flags);

#endif // REC2_PIXELMAP_H
