#include "devlist.h"

//br_error C2_HOOK_CDECL AddRequestedDrivers() {
//    char devstr[256];
//    static br_boolean bAlreadyDone;
//    LOG_TRACE("()");
//    NOT_IMPLEMENTED();
//}

br_error (C2_HOOK_STDCALL * devAdd_original)(br_device** pdev, br_device_begin_fn* dev_begin, char* args, br_image* image);
br_error C2_HOOK_STDCALL devAdd(br_device** pdev, br_device_begin_fn* dev_begin, char* args, br_image* image) {
#if defined(C2_HOOKS_ENABLED)
return devAdd_original(pdev, dev_begin, args, image);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00528490, devAdd, devAdd_original)

br_error (C2_HOOK_CDECL * BrDevAdd_original)(br_device** pdev, char* image, char* args);
br_error C2_HOOK_CDECL BrDevAdd(br_device** pdev, char* image, char* args) {
#if defined(C2_HOOKS_ENABLED)
    return BrDevAdd_original(pdev, image, args);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00528430, BrDevAdd, BrDevAdd_original)

br_error C2_HOOK_CDECL BrDevAddStatic(br_device** pdev, br_device_begin_fn* dev_begin, char* args) {
    return devAdd(pdev, dev_begin, args, NULL);
}
C2_HOOK_FUNCTION(0x005285a0, BrDevAddStatic)

br_error (C2_HOOK_CDECL * BrDevCheckAdd_original)(br_device** pdev, char* name, char* args);
br_error C2_HOOK_CDECL BrDevCheckAdd(br_device** pdev, char* name, char* args) {
#if defined(C2_HOOKS_ENABLED)
    return BrDevCheckAdd_original(pdev, name, args);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005285c0, BrDevCheckAdd, BrDevCheckAdd_original)

br_error (C2_HOOK_CDECL * BrDevAddConfig_original)(char* config);
br_error C2_HOOK_CDECL BrDevAddConfig(char* config) {
#if defined(C2_HOOKS_ENABLED)
    return BrDevAddConfig_original(config);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005286d0, BrDevAddConfig, BrDevAddConfig_original)

br_error (C2_HOOK_CDECL * BrDevRemove_original)(br_device* dev);
br_error C2_HOOK_CDECL BrDevRemove(br_device* dev) {
#if defined(C2_HOOKS_ENABLED)
    return BrDevRemove_original(dev);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00528800, BrDevRemove, BrDevRemove_original)

br_error (C2_HOOK_CDECL * BrDevFind_original)(br_device** pdev, char* pattern);
br_error C2_HOOK_CDECL BrDevFind(br_device** pdev, char* pattern) {
#if defined(C2_HOOKS_ENABLED)
    C2_HOOK_START();
    br_error res = BrDevFind_original(pdev, pattern);
    C2_HOOK_DEBUGF("pattern=%s res=%d", pattern, res);
    C2_HOOK_FINISH();
    return res;
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00528860, BrDevFind, BrDevFind_original)

br_error (C2_HOOK_CDECL * BrDevFindMany_original)(br_device** devices, br_int_32* ndevices, br_int_32 max_devices, char* pattern);
br_error C2_HOOK_CDECL BrDevFindMany(br_device** devices, br_int_32* ndevices, br_int_32 max_devices, char* pattern) {
#if defined(C2_HOOKS_ENABLED)
    return BrDevFindMany_original(devices, ndevices, max_devices, pattern);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00528930, BrDevFindMany, BrDevFindMany_original)

br_error (C2_HOOK_CDECL * BrDevCount_original)(br_int_32* ndevices, char* pattern);
br_error C2_HOOK_CDECL BrDevCount(br_int_32* ndevices, char* pattern) {
#if defined(C2_HOOKS_ENABLED)
    return BrDevCount_original(ndevices, pattern);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00528a10, BrDevCount, BrDevCount_original)

br_error (C2_HOOK_CDECL * BrDevContainedFind_original)(br_object** ph, br_token type, char* pattern, br_token_value* tv);
br_error C2_HOOK_CDECL BrDevContainedFind(br_object** ph, br_token type, char* pattern, br_token_value* tv) {
#if defined(C2_HOOKS_ENABLED)
    return BrDevContainedFind_original(ph, type, pattern, tv);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00528ac0, BrDevContainedFind, BrDevContainedFind_original)

br_error (C2_HOOK_CDECL * BrDevContainedFindMany_original)(br_object** objects, br_int_32 max_objects, br_int_32* pnum_objects, br_token type, char* pattern, br_token_value* tv);
br_error C2_HOOK_CDECL BrDevContainedFindMany(br_object** objects, br_int_32 max_objects, br_int_32* pnum_objects, br_token type, char* pattern, br_token_value* tv) {
#if defined(C2_HOOKS_ENABLED)
    return BrDevContainedFindMany_original(objects, max_objects, pnum_objects, type, pattern, tv);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00528b90, BrDevContainedFindMany, BrDevContainedFindMany_original)

br_error (C2_HOOK_CDECL * BrDevContainedCount_original)(br_int_32* pcount, br_token type, char* pattern, br_token_value* tv);
br_error C2_HOOK_CDECL BrDevContainedCount(br_int_32* pcount, br_token type, char* pattern, br_token_value* tv) {
#if defined(C2_HOOKS_ENABLED)
    return BrDevContainedCount_original(pcount, type, pattern, tv);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00528c80, BrDevContainedCount, BrDevContainedCount_original)
