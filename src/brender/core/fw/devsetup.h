#ifndef REC2_DEVSETUP_H
#define REC2_DEVSETUP_H

#include <brender/brender.h>

#include "c2_hooks.h"

br_pixelmap* C2_HOOK_CDECL BrDevLastBeginQuery(void);

void C2_HOOK_CDECL BrDevLastBeginSet(br_pixelmap* pm);

br_error C2_HOOK_CDECL BrDevBeginVar(br_pixelmap** ppm, char* setup_string, ...);

br_error C2_HOOK_CDECL BrDevBeginTV(br_pixelmap** ppm, char* setup_string, br_token_value* setup_tv);

br_error C2_HOOK_CDECL BrDevBegin(br_pixelmap** ppm, char* setup_string);

br_pixelmap* C2_HOOK_CDECL BrDevBeginOld(char* setup_string);

void C2_HOOK_CDECL BrDevEndOld(void);

void C2_HOOK_CDECL BrDevPaletteSetOld(br_pixelmap* pm);

void C2_HOOK_CDECL BrDevPaletteSetEntryOld(int i, br_colour colour);

br_error C2_HOOK_CDECL BrRendererFacilityFind(br_renderer_facility** prf, br_device_pixelmap* destination, br_token scalar_type);

br_error C2_HOOK_CDECL BrPrimitiveLibraryFind(br_primitive_library** ppl, br_device_pixelmap* destination, br_token scalar_type);

br_error C2_HOOK_CDECL BrGeometryFormatFind(br_geometry** pgf, br_renderer* renderer, br_renderer_facility* renderer_facility, br_token scalar_type, br_token format_type);

#endif //REC2_DEVSETUP_H
