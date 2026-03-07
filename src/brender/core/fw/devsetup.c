#include "devsetup.h"

#include "devlist.h"
#include "tokenval.h"

#include "../core/std/brstdlib.h"

#include "../core/v1db/sys_conf.h"

#include <stdarg.h>
#include <stdio.h>

#include "c2_stdio.h"
#include "c2_stdlib.h"


// GLOBAL: CARMA2_HW 0x006ad9ac
br_pixelmap* last_begin_screen;

// FUNCTION: CARMA2_HW 0x00528d50
br_pixelmap* C2_HOOK_CDECL BrDevLastBeginQuery(void) {

    return last_begin_screen;
}

// FUNCTION: CARMA2_HW 0x00528d60
void C2_HOOK_CDECL BrDevLastBeginSet(br_pixelmap* pm) {

    last_begin_screen = pm;
}

// FUNCTION: CARMA2_HW 0x00528d70
br_error C2_HOOK_CDECL BrDevBeginVar(br_pixelmap** ppm, const char* setup_string, ...) {
    va_list vl;
    br_uint_32 i;
    br_token_value tv[64];

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
    return res;
}

// FUNCTION: CARMA2_HW 0x00528df0
br_error C2_HOOK_CDECL BrDevBegin(br_pixelmap** ppm, const char* setup_string) {

    return BrDevBeginTV(ppm, setup_string, NULL);
}

// FUNCTION: CARMA2_HW 0x00528e10
br_error C2_HOOK_CDECL BrDevBeginTV(br_pixelmap** ppm, const char* setup_string, br_token_value* setup_tv) {
    char str[512];
    char* args;
    char* devname;
    char* devargs;
    char devices_str[512];
    char* device_name;
    char* device_args;
    char* next_device;
    br_token_value args_tv[64];
    br_token_value* tv;
    br_output_facility* output_facility;
    br_device* device;
    br_pixelmap* screen;
    int i;
    int n;
    br_error r;

    tv = NULL;
    if (setup_string == NULL) {
        BrSystemConfigQueryString(BRT_DEFAULT_DEVICE_STR, str, BR_ASIZE(str));
    } else {
        BrStrNCpy(str, setup_string, BR_ASIZE(str));
        str[BR_ASIZE(str) - 1] = '\0';
    }
    for (args = str; *args != '\0' && *args != ','; args++) {
    }
    if (*args != '\0') {
        *args = '\0';
        args++;
    }
    for (devname = str; BrIsSpace(*devname); devname++) {
    }
    n = BrStrLen(devname);
    /* Fix to avoid writing to negative indices */
    while (n != 0 && BrIsSpace(devname[n - 1])) {
        devname[n - 1] = '\0';
        n--;
    }
    if (n == 0 || (n == 1 && devname[0] == '*')) {
        if (*args != '\0' && BrStringToTokenValue(args_tv, sizeof(args_tv), args) == 0) {
            tv = args_tv;
        }
        if (setup_tv != NULL) {
            if (tv == NULL) {
                tv = setup_tv;
            } else {
                for (tv = setup_tv; tv->t != BR_NULL_TOKEN; tv++) {
                    for (i = 0; args_tv[i].t != BR_NULL_TOKEN; i++) {
                        if (args_tv[i].t == tv->t) {
                            break;
                        }
                    }
                    if (args_tv[i].t == BR_NULL_TOKEN) {
                        if (i == BR_ASIZE(args_tv) - 1) {
                            BrFailure("Too many token value pairs");
                        }
                        args_tv[i + 1].t = BR_NULL_TOKEN;
                        args_tv[i + 1].v.p = NULL;
                    }
                    args_tv[i] = *tv;
                }
                tv = args_tv;
            }
        }
        r = BrDevContainedFind((br_object **)&output_facility, BRT_OUTPUT_FACILITY, NULL, tv);
        if (r != 0) {
            return r;
        }
    } else {
        if (BrDevFind(&device, devname) != 0) {
            devargs = NULL;
            if (BrSystemConfigQueryString(BRT_BRENDER_DEVICES_STR, devices_str, BR_ASIZE(devices_str)) == 0) {
                for (device_name = devices_str; *device_name != '\0'; device_name = next_device) {
                    for (device_args = device_name; *device_args != '\0' && *device_args != ',' && *device_args != ';'; device_args++) {
                    }
                    if (*device_args == ',') {
                        *device_args = '\0';
                        device_args++;
                    }
                    for (next_device = device_args; *next_device != '\0' && *next_device != ';'; next_device++) {
                    }
                    if (*next_device != '\0') {
                        *next_device = '\0';
                        next_device++;
                    }
                    while (BrIsSpace(*device_name)) {
                        device_name++;
                    }
                    n = BrStrLen(device_name);
                    /* Fix to avoid writing to negative indices */
                    while (n != 0 && BrIsSpace(device_name[n - 1])) {
                        device_name[n - 1] = '\0';
                        n--;
                    }
                    if (BrStrICmp(devname, device_name) == 0) {
                        devargs = device_args;
                        break;
                    }
                }
            }

            r = BrDevAdd(&device, devname, devargs);
            if (r != 0) {
                return r;
            }
        }
        if(*args && BrStringToTokenValue(args_tv, sizeof(args_tv), args) == 0) {
            tv = args_tv;
        }
        if (setup_tv != NULL) {
            if (tv == NULL) {
                tv = setup_tv;
            } else {
                for (tv = setup_tv; tv->t != BR_NULL_TOKEN; tv++) {
                    for (i = 0; args_tv[i].t != BR_NULL_TOKEN; i++) {
                        if (args_tv[i].t == tv->t) {
                            break;
                        }
                    }
                    if (args_tv[i].t == BR_NULL_TOKEN) {
                        if (i == BR_ASIZE(args_tv) - 1) {
                            BrFailure("Too many token value pairs");
                        }
                        args_tv[i+1].t = BR_NULL_TOKEN;
                        args_tv[i+1].v.p = NULL;
                    }
                    args_tv[i] = *tv;
                }
                tv = args_tv;
            }
        }
        r = device->dispatch->_find((br_object_container*)device, (br_object**)&output_facility, BRT_OUTPUT_FACILITY, NULL, tv);
        if (r != 0) {
            return r;
        }
    }
    r = output_facility->dispatch->_pixelmapNew(output_facility, (br_device_pixelmap**)&screen, tv);
    if (r != 0) {
        return r;
    }
    BrDevLastBeginSet(screen);
    *ppm = screen;
    return 0;
}

// FUNCTION: CARMA2_HW 0x00529270
br_pixelmap* C2_HOOK_CDECL BrDevBeginOld(const char* setup_string) {
    br_pixelmap* s;

    if (BrDevLastBeginQuery() != NULL) {
        BrFailure("Device already active");
    }
    if (BrDevBeginTV(&s, setup_string, NULL) != 0) {
        BrFailure("Could not start driver");
    }
    return s;
}

// FUNCTION: CARMA2_HW 0x005292c0
void C2_HOOK_CDECL BrDevEndOld(void) {

    if (BrDevLastBeginQuery() != NULL) {
        BrPixelmapFree(BrDevLastBeginQuery());
    }
    BrDevLastBeginSet(NULL);
}

// FUNCTION: CARMA2_HW 0x005292e0
void C2_HOOK_CDECL BrDevPaletteSetOld(br_pixelmap* pm) {

    if (BrDevLastBeginQuery() != NULL) {
        BrPixelmapPaletteSet(BrDevLastBeginQuery(), pm);
    }
}

// FUNCTION: CARMA2_HW 0x00529300
void C2_HOOK_CDECL BrDevPaletteSetEntryOld(int i, br_colour colour) {

    if (BrDevLastBeginQuery() != NULL) {
        BrPixelmapPaletteEntrySet(BrDevLastBeginQuery(), i, colour);
    }
}

// FUNCTION: CARMA2_HW 0x00529320
br_error C2_HOOK_CDECL BrRendererFacilityFind(br_renderer_facility** prf, br_device_pixelmap* destination, br_token scalar_type) {
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
}

// FUNCTION: CARMA2_HW 0x005294d0
br_error C2_HOOK_CDECL BrPrimitiveLibraryFind(br_primitive_library** ppl, br_device_pixelmap* destination, br_token scalar_type) {
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
}

// FUNCTION: CARMA2_HW 0x00529680
br_error C2_HOOK_CDECL BrGeometryFormatFind(br_geometry** pgf, br_renderer* renderer, br_renderer_facility* renderer_facility, br_token scalar_type, br_token format_type) {
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
}
