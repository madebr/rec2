#include "devsetup.h"

#include "devlist.h"

#include "../core/std/brstdlib.h"

#include <stdarg.h>
#include <stdio.h>

#include "c2_stdio.h"
#include "c2_stdlib.h"

C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, last_begin_screen, 0x006ad9ac);

br_pixelmap* C2_HOOK_CDECL BrDevLastBeginQuery(void) {

    return C2V(last_begin_screen);
}
C2_HOOK_FUNCTION(0x00528d50, BrDevLastBeginQuery)

void C2_HOOK_CDECL BrDevLastBeginSet(br_pixelmap* pm) {

    C2V(last_begin_screen) = pm;
}
C2_HOOK_FUNCTION(0x00528d60, BrDevLastBeginSet)

br_error C2_HOOK_CDECL BrDevBeginVar(br_pixelmap** ppm, char* setup_string, ...) {
    va_list vl;
    br_uint_32 i;
    br_token_value tv[64];

    C2_HOOK_START();
    va_start(vl, setup_string);
    for (i = 0; ; i++) {
        if (i == BR_ASIZE(tv)) {
            BrFailure("Too many token value pairs");
        }
        tv[i].t = va_arg(vl, br_token);
        if (tv[i].t == BR_NULL_TOKEN) {
            break;
        }
        tv[i].v = va_arg(vl, br_value);
    }
    va_end(vl);
    tv[i].t = BR_NULL_TOKEN;
    tv[i].v.p = NULL;
    br_error res = BrDevBeginTV(ppm, setup_string, tv);
    C2_HOOK_FINISH();
    return res;
}
C2_HOOK_FUNCTION(0x00528d70, BrDevBeginVar)

br_error C2_HOOK_CDECL BrDevBegin(br_pixelmap** ppm, char* setup_string) {

    return BrDevBeginTV(ppm, setup_string, NULL);
}
C2_HOOK_FUNCTION(0x00528df0, BrDevBegin)

br_error (C2_HOOK_CDECL * BrDevBeginTV_original)(br_pixelmap** ppm, char* setup_string, br_token_value* setup_tv);
br_error C2_HOOK_CDECL BrDevBeginTV(br_pixelmap** ppm, char* setup_string, br_token_value* setup_tv) {
#if defined(C2_HOOKS_ENABLED)
    return BrDevBeginTV_original(ppm, setup_string, setup_tv);
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00528e10, BrDevBeginTV, BrDevBeginTV_original)

br_pixelmap* (C2_HOOK_CDECL * BrDevBeginOld_original)(char* setup_string);
br_pixelmap* C2_HOOK_CDECL BrDevBeginOld(char* setup_string) {
#if 0//defined(C2_HOOKS_ENABLED)
    return BrDevBeginOld_original(setup_string);
#else
    br_pixelmap* s;

    if (BrDevLastBeginQuery() != NULL) {
        BrFailure("Device already active");
    }
    if (BrDevBeginTV(&s, setup_string, NULL) != 0) {
        BrFailure("Could not start driver");
    }
    return s;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00529270, BrDevBeginOld, BrDevBeginOld_original)

void (C2_HOOK_CDECL * BrDevEndOld_original)(void);
void C2_HOOK_CDECL BrDevEndOld(void) {
#if 0//defined(C2_HOOKS_ENABLED)
    BrDevEndOld_original();
#else

    if (BrDevLastBeginQuery() != NULL) {
        BrPixelmapFree(BrDevLastBeginQuery());
    }
    BrDevLastBeginSet(NULL);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005292c0, BrDevEndOld, BrDevEndOld_original)

void (C2_HOOK_CDECL * BrDevPaletteSetOld_original)(br_pixelmap* pm);
void C2_HOOK_CDECL BrDevPaletteSetOld(br_pixelmap* pm) {
#if 0//defined(C2_HOOKS_ENABLED)
    BrDevPaletteSetOld_original(pm);
#else

    if (BrDevLastBeginQuery() != NULL) {
        BrPixelmapPaletteSet(BrDevLastBeginQuery(), pm);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005292e0, BrDevPaletteSetOld, BrDevPaletteSetOld_original)

void (C2_HOOK_CDECL * BrDevPaletteSetEntryOld_original)(int i, br_colour colour);
void C2_HOOK_CDECL BrDevPaletteSetEntryOld(int i, br_colour colour) {
#if 0//defined(C2_HOOKS_ENABLED)
    BrDevPaletteSetEntryOld_original(i, colour);
#else
    if (BrDevLastBeginQuery() != NULL) {
        BrPixelmapPaletteEntrySet(BrDevLastBeginQuery(), i, colour);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00529300, BrDevPaletteSetEntryOld, BrDevPaletteSetEntryOld_original)

br_error (C2_HOOK_CDECL * BrRendererFacilityFind_original)(br_renderer_facility** prf, br_device_pixelmap* destination, br_token scalar_type);
br_error C2_HOOK_CDECL BrRendererFacilityFind(br_renderer_facility** prf, br_device_pixelmap* destination, br_token scalar_type) {
#if 0//defined(C2_HOOKS_ENABLED)
    return BrRendererFacilityFind_original(prf, destination, scalar_type);
#else
    br_renderer_facility* renderer_facility;
    br_error r;
    br_output_facility* ot;
    char object_name[23] = "Default-Renderer-00000";
    char image_name[9] = "softrnd0";
    br_boolean scalar_is_valid;

    renderer_facility = NULL;
    scalar_is_valid = 0;
    switch (scalar_type) {
    case BRT_FIXED:
        BrStrCpy(&object_name[17], "Fixed");
        image_name[7] = 'x';
        scalar_is_valid = 1;
        break;
    case BRT_FLOAT:
        BrStrCpy(&object_name[17], "Float");
        image_name[7] = 'f';
        scalar_is_valid = 1;
        break;
    default:
        c2_abort();
    }
    if (renderer_facility == NULL && destination != NULL) {
        destination->dispatch->_query((br_object*)destination, &renderer_facility, BRT_RENDERER_FACILITY_O);
    }
    if (renderer_facility == NULL && destination != NULL) {
        ot = NULL;
        r = destination->dispatch->_query((br_object*)destination, &ot, BRT_OUTPUT_FACILITY_O);
        if (r == 0 && ot != NULL) {
            ot->dispatch->_query((br_object*)ot, &renderer_facility, BRT_RENDERER_FACILITY_O);
        }
    }
    if (scalar_is_valid && renderer_facility == NULL) {
        BrDevContainedFind((br_object**)&renderer_facility, BRT_RENDERER_FACILITY, object_name, NULL);
    }
    if (renderer_facility == NULL) {
        BrDevContainedFind((br_object**)&renderer_facility, BRT_RENDERER_FACILITY, "Default-Renderer", NULL);
    }
    if (scalar_is_valid && renderer_facility == NULL) {
        BrDevCheckAdd(0, image_name, 0);
        BrDevContainedFind((br_object**)&renderer_facility, BRT_RENDERER_FACILITY, object_name, NULL);
    }
    if (renderer_facility == NULL) {
        BrDevCheckAdd(0, "softrend", 0);
        BrDevContainedFind((br_object**)&renderer_facility, BRT_RENDERER_FACILITY, "Default-Renderer", NULL);
    }
    if (renderer_facility == NULL) {
        return 0x1002;
    }
    *prf = renderer_facility;
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00529320, BrRendererFacilityFind, BrRendererFacilityFind_original)

br_error (C2_HOOK_CDECL * BrPrimitiveLibraryFind_original)(br_primitive_library** ppl, br_device_pixelmap* destination, br_token scalar_type);
br_error C2_HOOK_CDECL BrPrimitiveLibraryFind(br_primitive_library** ppl, br_device_pixelmap* destination, br_token scalar_type) {
#if 0//defined(C2_HOOKS_ENABLED)
    return BrPrimitiveLibraryFind_original(ppl, destination, scalar_type);
#else
    br_primitive_library* primitive_library;
    br_error r;
    br_output_facility* ot;
    char object_name[25] = "Default-Primitives-00000";
    char image_name[9] = "softprm0";
    br_boolean scalar_is_valid;

    primitive_library = NULL;
    scalar_is_valid = 0;
    switch (scalar_type) {
    case BRT_FIXED:
        BrStrCpy(&object_name[19], "Fixed");
        image_name[7] = 'x';
        scalar_is_valid = 1;
        break;
    case BRT_FLOAT:
        BrStrCpy(&object_name[19], "Float");
        image_name[7] = 'f';
        scalar_is_valid = 1;
        break;
    default:
        c2_abort();
    }
    if (primitive_library == NULL && destination != NULL) {
        destination->dispatch->_query((br_object*)destination, &primitive_library, BRT_PRIMITIVE_LIBRARY_O);
    }
    if (primitive_library == NULL && destination != NULL) {
        ot = NULL;
        r = destination->dispatch->_query((br_object*)destination, &ot, BRT_OUTPUT_FACILITY_O);
        if (r == 0 && ot != NULL) {
            ot->dispatch->_query((br_object*)ot, &primitive_library, BRT_PRIMITIVE_LIBRARY_O);
        }
    }
    if (scalar_is_valid && primitive_library == NULL) {
        BrDevContainedFind((br_object**)&primitive_library, BRT_PRIMITIVE_LIBRARY, object_name, NULL);
    }
    if (primitive_library == NULL) {
        BrDevContainedFind((br_object**)&primitive_library, BRT_PRIMITIVE_LIBRARY, "Default-Primitives", NULL);
    }
    if (scalar_is_valid && primitive_library == NULL) {
        BrDevCheckAdd(0, image_name, 0);
        BrDevContainedFind((br_object**)&primitive_library, BRT_PRIMITIVE_LIBRARY, object_name, NULL);
    }
    if (primitive_library == NULL) {
        BrDevCheckAdd(0, "softprim", 0);
        BrDevContainedFind((br_object**)&primitive_library, BRT_PRIMITIVE_LIBRARY, "Default-Primitives", NULL);
    }
    if (primitive_library == NULL) {
        return 0x1002;
    }
    *ppl = primitive_library;
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005294d0, BrPrimitiveLibraryFind, BrPrimitiveLibraryFind_original)

br_error (C2_HOOK_CDECL * BrGeometryFormatFind_original)(br_geometry** pgf, br_renderer* renderer, br_renderer_facility* renderer_facility, br_token scalar_type, br_token format_type);
br_error C2_HOOK_CDECL BrGeometryFormatFind(br_geometry** pgf, br_renderer* renderer, br_renderer_facility* renderer_facility, br_token scalar_type, br_token format_type) {
#if 0//defined(C2_HOOKS_ENABLED)
    return BrGeometryFormatFind_original(pgf, renderer, renderer_facility, scalar_type, format_type);
#else
    br_error r;
    br_geometry* gf;
    char object_name[21] = "Default-Format-00000";
    char image_name[9] = "softfrm0";
    br_boolean scalar_is_valid;

    scalar_is_valid = 0;
    switch (scalar_type) {
    case BRT_FIXED:
        BrStrCpy(&object_name[15], "Fixed");
        image_name[7] = 'x';
        scalar_is_valid = 1;
        break;
    case BRT_FLOAT:
        BrStrCpy(&object_name[15], "Float");
        image_name[7] = 'f';
        scalar_is_valid = 1;
        break;
    default:
        c2_abort();
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_renderer_dispatch, _query, 0x2c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_renderer_dispatch, _find, 0x60);

    r = renderer->dispatch->_find((br_object_container*)renderer, (br_object**)&gf, format_type, NULL, NULL);
    if (r == 0) {
        *pgf = gf;
        return 0;
    }
    if (renderer_facility == NULL) {
        renderer->dispatch->_query((br_object*)renderer, (br_object**)&renderer_facility, BRT_RENDERER_FACILITY_O);
    }
    if (renderer_facility != NULL) {
        r = renderer_facility->dispatch->_find((br_object_container*)renderer_facility, (br_object**)&gf, format_type, 0, 0);
        if (r == 0) {
            *pgf = gf;
            return 0;
        }
    }
    if (scalar_is_valid) {
        r = BrDevContainedFind((br_object**)&gf, format_type, object_name, NULL);
        if (r == 0) {
            *pgf = gf;
            return 0;
        }
    }
    r = BrDevContainedFind((br_object**)&gf, format_type, object_name, NULL);
    if (r == 0) {
        *pgf = gf;
        return 0;
    }
    if (scalar_is_valid) {
        BrDevCheckAdd(0, image_name, 0);
        r = BrDevContainedFind((br_object**)&gf, format_type, object_name, NULL);
        if (r == 0) {
            *pgf = gf;
            return 0;
        }
    }
    BrDevCheckAdd(0, "softform", 0);
    r = BrDevContainedFind((br_object**)&gf, format_type, object_name, NULL);
    if (r == 0) {
        *pgf = gf;
        return 0;
    }
    return 0x1002;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00529680, BrGeometryFormatFind, BrGeometryFormatFind_original)
