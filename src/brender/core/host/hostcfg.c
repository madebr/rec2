#include "hostcfg.h"

#include "core/fw/diag.h"

#include <windows.h>

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

#if 0//defined(C2_HOOKS_ENABLED)
    return HostIniSectionExists_original(ini_file, section_name);
#else
    char buffer[5];

    return GetPrivateProfileSectionA(section_name, buffer, sizeof(buffer), ini_file) != 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053fbb0, HostIniSectionExists, HostIniSectionExists_original)


br_error (C2_HOOK_CDECL * HostIniQuery_original)(char* ini_file, char* section_name, char* entry, char* Buffer, int max, br_uint_16* size);
br_error C2_HOOK_CDECL HostIniQuery(char* ini_file, char* section_name, char* entry, char* Buffer, int max, br_uint_16* size) {

#if 0//defined(C2_HOOKS_ENABLED)
    return HostIniQuery_original(ini_file, section_name, entry, Buffer, max, size);
#else
    static char default_entry[] = "";

    *size = (br_uint_16)GetPrivateProfileStringA(section_name, entry, default_entry, Buffer, max, ini_file);
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053fbe0, HostIniQuery, HostIniQuery_original)


br_error (C2_HOOK_CDECL * HostRegistryQuery_original)(void* hKey, char* Path, char* entry, char* Buffer, br_uint_16 max, br_uint_16* size);
br_error C2_HOOK_CDECL HostRegistryQuery(void* hKey, char* Path, char* entry, char* Buffer, br_uint_16 max, br_uint_16* size) {

#if 0//defined(C2_HOOKS_ENABLED)
    return HostRegistryQuery_original(hKey, Path, entry, Buffer, max, size);
#else
    DWORD cbData;
    DWORD type;

    if (hKey == NULL) {
        hKey = HKEY_LOCAL_MACHINE;
    }
    if (RegOpenKeyExA(hKey, Path, 0, KEY_READ, (PHKEY)&hKey) != ERROR_SUCCESS) {
        return 0x1002;
    }
    type = REG_NONE;
    cbData = max;
    if (RegQueryValueExA((HKEY)hKey, entry, NULL, &type, (LPBYTE)Buffer, &cbData) != ERROR_SUCCESS) {
        return 0x1002;
    }
    if (type != REG_SZ) {
        BrFailure("Registry entry type is not a string");
    }
    *size = (br_uint_16)cbData;
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053fc10, HostRegistryQuery, HostRegistryQuery_original)
