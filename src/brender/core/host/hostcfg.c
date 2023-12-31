#include "hostcfg.h"

const char* (C2_HOOK_CDECL * HostDefaultDevice_original)(void);
const char* C2_HOOK_CDECL HostDefaultDevice(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    return HostDefaultDevice_original();
#else
    return "DDRAW";
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053fba0, HostDefaultDevice, HostDefaultDevice_original)


br_boolean (C2_HOOK_CDECL * HostIniSectionExists_original)(char* ini_file, char* section_name);
br_boolean C2_HOOK_CDECL HostIniSectionExists(char* ini_file, char* section_name) {

#if defined(C2_HOOKS_ENABLED)
    return HostIniSectionExists_original(ini_file, section_name);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053fbb0, HostIniSectionExists, HostIniSectionExists_original)


br_error (C2_HOOK_CDECL * HostIniQuery_original)(char* ini_file, char* section_name, char* entry, char* Buffer, int max, br_uint_16* size);
br_error C2_HOOK_CDECL HostIniQuery(char* ini_file, char* section_name, char* entry, char* Buffer, int max, br_uint_16* size) {

#if defined(C2_HOOKS_ENABLED)
    return HostIniQuery_original(ini_file, section_name, entry, Buffer, max, size);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053fbe0, HostIniQuery, HostIniQuery_original)


br_error (C2_HOOK_CDECL * HostRegistryQuery_original)(void* hKey, char* Path, char* entry, char* Buffer, br_uint_16 max, br_uint_16* size);
br_error C2_HOOK_CDECL HostRegistryQuery(void* hKey, char* Path, char* entry, char* Buffer, br_uint_16 max, br_uint_16* size) {

#if defined(C2_HOOKS_ENABLED)
    return HostRegistryQuery_original(hKey, Path, entry, Buffer, max, size);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053fc10, HostRegistryQuery, HostRegistryQuery_original)
